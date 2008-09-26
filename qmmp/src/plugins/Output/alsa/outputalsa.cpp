/***************************************************************************
 *   Copyright (C) 2006-2008 by Ilya Kotov                                 *
 *   forkotov02@hotmail.ru                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <QObject>
#include <QApplication>
#include <QtGlobal>
#include <QDir>
#include <QSettings>
#include <QTimer>

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#include <qmmp/constants.h>
#include <qmmp/buffer.h>
#include <qmmp/visual.h>
#include <qmmp/statehandler.h>
#include "outputalsa.h"

OutputALSA::OutputALSA(QObject * parent, bool useVolume)
        : Output(parent), m_inited(FALSE), m_pause(FALSE), m_play(FALSE),
        m_userStop(FALSE), m_totalWritten(0), m_currentSeconds(-1),
        m_bps(-1), m_channels(-1), m_precision(-1)
{
    m_frequency = 0;
    QSettings settings(QDir::homePath()+"/.qmmp/qmmprc", QSettings::IniFormat);
    QString dev_name = settings.value("ALSA/device","default").toString();
    m_use_mmap = settings.value("ALSA/use_mmap", FALSE).toBool();
    pcm_name = strdup(dev_name.toAscii().data());
    pcm_handle = 0;
    //alsa mixer
    mixer = 0;
    if (useVolume)
    {
        QString card = settings.value("ALSA/mixer_card","hw:0").toString();
        QString dev = settings.value("ALSA/mixer_device", "PCM").toString();
        setupMixer(card, dev);
    }
}

OutputALSA::~OutputALSA()
{
    uninitialize();
    free (pcm_name);
    if (mixer)
        snd_mixer_close(mixer);
}

void OutputALSA::stop()
{
    m_userStop = TRUE;
}

void OutputALSA::status()
{
    long ct = (m_totalWritten - latency()) / m_bps;

    if (ct < 0)
        ct = 0;

    if (ct > m_currentSeconds)
    {
        m_currentSeconds = ct;
        dispatch(m_currentSeconds, m_totalWritten, m_rate,
                 m_frequency, m_precision, m_channels);
    }
}

qint64 OutputALSA::written()
{
    return m_totalWritten;
}

void OutputALSA::seek(qint64 pos)
{
    m_totalWritten = (pos * m_bps);
    m_currentSeconds = -1;
}

void OutputALSA::configure(quint32 freq, int chan, int prec)
{
    // we need to configure
    if (freq != m_frequency || chan != m_channels || prec != m_precision)
    {
        m_frequency = freq;
        m_channels = chan;
        m_precision = prec;
        m_bps = freq * chan * (prec / 8);
        uint rate = m_frequency; /* Sample rate */
        uint exact_rate = m_frequency;   /* Sample rate returned by */

        /* load settings from config */
        QSettings settings(QDir::homePath()+"/.qmmp/qmmprc", QSettings::IniFormat);
        settings.beginGroup("ALSA");
        uint buffer_time = settings.value("buffer_time",500).toUInt()*1000;
        uint period_time = settings.value("period_time",100).toUInt()*1000;
        settings.endGroup();

        snd_pcm_hw_params_t *hwparams = 0;
        snd_pcm_sw_params_t *swparams = 0;
        int err; //alsa error code

        //hw params
        snd_pcm_hw_params_alloca(&hwparams);
        if ((err = snd_pcm_hw_params_any(pcm_handle, hwparams)) < 0)
        {
            qWarning("OutputALSA: Can not read configuration for PCM device: %s", snd_strerror(err));
            return;
        }
        if (m_use_mmap)
        {
            if ((err = snd_pcm_hw_params_set_access(pcm_handle, hwparams, SND_PCM_ACCESS_MMAP_INTERLEAVED)) < 0)
            {
                qWarning("OutputALSA: Error setting mmap access: %s", snd_strerror(err));
                m_use_mmap = FALSE;
            }
        }
        if (!m_use_mmap)
        {
            if ((err = snd_pcm_hw_params_set_access(pcm_handle, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0)
            {
                qWarning("OutputALSA: Error setting access: %s", snd_strerror(err));
                return;
            }
        }
        snd_pcm_format_t format = SND_PCM_FORMAT_UNKNOWN;
        switch (prec)
        {
        case 8:
            format = SND_PCM_FORMAT_S8;
            break;
        case 16:
            format = SND_PCM_FORMAT_S16_LE;
            break;
        case 24:
            format = SND_PCM_FORMAT_S24_LE;
            break;
        case 32:
            format = SND_PCM_FORMAT_S32_LE;
            break;
        default:
            qWarning("OutputALSA: unsupported format detected");
            return;
        }
        if ((err = snd_pcm_hw_params_set_format(pcm_handle, hwparams, format)) < 0)
        {
            qDebug("OutputALSA: Error setting format: %s", snd_strerror(err));
            return;
        }
        exact_rate = rate;// = 11000;
        qDebug("OutputALSA: frequency=%d, channels=%d", rate, chan);

        if ((err = snd_pcm_hw_params_set_rate_near(pcm_handle, hwparams, &exact_rate, 0)) < 0)
        {
            qWarning("OutputALSA: Error setting rate: %s", snd_strerror(err));
            return;
        }
        if (rate != exact_rate)
        {
            qWarning("OutputALSA: The rate %d Hz is not supported by your hardware.\n==> Using %d Hz instead.", rate, exact_rate);
        }
        uint c = m_channels;
        if ((err = snd_pcm_hw_params_set_channels_near(pcm_handle, hwparams, &c)) < 0)
        {
            qWarning("OutputALSA: Error setting channels: %s", snd_strerror(err));
            return;
        }
        if ((err = snd_pcm_hw_params_set_period_time_near(pcm_handle, hwparams, &period_time ,0)) < 0)
        {
            qWarning("OutputALSA: Error setting period time: %s", snd_strerror(err));
            return;
        }
        if ((err = snd_pcm_hw_params_set_buffer_time_near(pcm_handle, hwparams, &buffer_time ,0)) < 0)
        {
            qWarning("OutputALSA: Error setting buffer time: %s", snd_strerror(err));
            return;
        }
        if ((err = snd_pcm_hw_params(pcm_handle, hwparams)) < 0)
        {
            qWarning("OutputALSA: Error setting HW params: %s", snd_strerror(err));
            return;
        }
        //read some alsa parameters
        snd_pcm_uframes_t buffer_size = 0;
        snd_pcm_uframes_t period_size = 0;
        if ((err = snd_pcm_hw_params_get_buffer_size(hwparams, &buffer_size)) < 0)
        {
            qWarning("OutputALSA: Error reading buffer size: %s", snd_strerror(err));
            return;
        }
        if ((err = snd_pcm_hw_params_get_period_size(hwparams, &period_size, 0)) < 0)
        {
            qWarning("OutputALSA: Error reading period size: %s", snd_strerror(err));
            return;
        }
        //swparams
        snd_pcm_sw_params_alloca(&swparams);
        snd_pcm_sw_params_current(pcm_handle, swparams);
        if ((err = snd_pcm_sw_params_set_start_threshold(pcm_handle, swparams,
                   buffer_size - period_size)) < 0)
            qWarning("OutputALSA: Error setting threshold: %s", snd_strerror(err));
        if ((err = snd_pcm_sw_params(pcm_handle, swparams)) < 0)
        {
            qWarning("OutputALSA: Error setting SW params: %s", snd_strerror(err));
            return;
        }
        //setup needed values
        m_bits_per_frame = snd_pcm_format_physical_width(format) * chan;
        m_chunk_size = period_size;
    }
}

void OutputALSA::reset()
{
    if (pcm_handle)
    {
        snd_pcm_close(pcm_handle);
        pcm_handle = 0;
    }
    if (snd_pcm_open(&pcm_handle, pcm_name, SND_PCM_STREAM_PLAYBACK, SND_PCM_NONBLOCK) < 0)
    {
        qWarning ("OutputALSA: Error opening PCM device %s", pcm_name);
        return;
    }
}


void OutputALSA::pause()
{
    if (!m_play)
        return;
    m_pause = !m_pause;
    Qmmp::State state = m_pause ? Qmmp::Paused: Qmmp::Playing;
    dispatch(state);
}

bool OutputALSA::initialize()
{
    m_inited = m_pause = m_play = m_userStop = FALSE;

    if (pcm_handle)
        return FALSE;

    m_currentSeconds = -1;
    m_totalWritten = 0;
    if (snd_pcm_open(&pcm_handle, pcm_name, SND_PCM_STREAM_PLAYBACK, SND_PCM_NONBLOCK) < 0)
    {
        qWarning ("OutputALSA: Error opening PCM device %s", pcm_name);
        return FALSE;
    }

    m_inited = TRUE;
    return TRUE;
}


qint64 OutputALSA::latency()
{
    long used = 0;

    /*if (! m_pause)
    {
        if (ioctl(audio_fd, SNDCTL_DSP_GETODELAY, &used) == -1)
            used = 0;
    }*/

    return used;
}

void OutputALSA::run()
{

    mutex()->lock ();
    if (! m_inited)
    {
        mutex()->unlock();
        return;
    }

    m_play = TRUE;

    mutex()->unlock();

    Buffer *b = 0;
    bool done = FALSE;
    long m = 0;
    snd_pcm_uframes_t l;

    long prebuffer_size = Buffer::size() + m_bits_per_frame * m_chunk_size / 8;

    unsigned char *prebuffer = (unsigned uchar *)malloc(prebuffer_size);
    ulong prebuffer_fill = 0;

    dispatch(Qmmp::Playing);

    while (!done)
    {
        mutex()->lock ();
        recycler()->mutex()->lock ();

        done = m_userStop;

        while (!done && (recycler()->empty() || m_pause))
        {
            mutex()->unlock();
            recycler()->cond()->wakeOne();
            recycler()->cond()->wait(recycler()->mutex());
            mutex()->lock ();
            done = m_userStop;
        }
        status();

        if (! b)
        {
            b = recycler()->next();
            if (b->rate)
                m_rate = b->rate;
        }

        recycler()->cond()->wakeOne();
        recycler()->mutex()->unlock();

        if (b)
        {
            if ((ulong)prebuffer_size < prebuffer_fill + b->nbytes)
            {
                prebuffer_size = prebuffer_fill + b->nbytes;
                prebuffer = (unsigned char*) realloc(prebuffer, prebuffer_size);
            }


            memcpy(prebuffer + prebuffer_fill, b->data, b->nbytes);
            prebuffer_fill += b->nbytes;

            l = snd_pcm_bytes_to_frames(pcm_handle, prebuffer_fill);

            while (l >= m_chunk_size)
            {
                snd_pcm_wait(pcm_handle, 10);
                if ((m = alsa_write(prebuffer, m_chunk_size)) >= 0)
                {
                    l -= m;
                    m = snd_pcm_frames_to_bytes(pcm_handle, m); // convert frames to bytes
                    prebuffer_fill -= m;
                    memcpy(prebuffer, prebuffer + m, prebuffer_fill); //move data to begin
                    m_totalWritten += m;
                    status();
                    dispatchVisual(b, m_totalWritten, m_channels, m_precision);
                }
                else
                    break;
            }
        }
        //force buffer change
        recycler()->mutex()->lock ();
        recycler()->done();
        recycler()->mutex()->unlock();
        b = 0;
        mutex()->unlock();
    }

    mutex()->lock ();
    //write remaining data
    if (prebuffer_fill > 0 && recycler()->empty())
    {
        l = snd_pcm_bytes_to_frames(pcm_handle, prebuffer_fill);

        while (l > 0)
        {
            if ((m = alsa_write(prebuffer, l)) >= 0)
            {
                l -= m;
                m = snd_pcm_frames_to_bytes(pcm_handle, m); // convert frames to bytes
                prebuffer_fill -= m;
                memcpy(prebuffer, prebuffer + m, prebuffer_fill);
                m_totalWritten += m;
                status();
            }
            else
                break;
        }
    }
    m_play = FALSE;
    dispatch(Qmmp::Stopped);
    free(prebuffer);
    prebuffer = 0;
    mutex()->unlock();

}

long OutputALSA::alsa_write(unsigned char *data, long size)
{
    long m = 0;
    if (m_use_mmap)
        m = snd_pcm_mmap_writei (pcm_handle, data, size);
    else
        m = snd_pcm_writei (pcm_handle, data, size);

    if (m == -EAGAIN)
    {
        mutex()->unlock();
        snd_pcm_wait(pcm_handle, 500);
        mutex()->lock ();
        return 0;
    }
    else if (m >= 0)
    {
        if (m < size)
        {
            mutex()->unlock();
            snd_pcm_wait(pcm_handle, 500);
            mutex()->lock ();
        }
        return m;
    }
    else if (m == -EPIPE)
    {
        qDebug ("OutputALSA: buffer underrun!");
        if ((m = snd_pcm_prepare(pcm_handle)) < 0)
        {
            qDebug ("OutputALSA: Can't recover after underrun: %s",
                    snd_strerror(m));
            /* TODO: reopen the device */
            return -1;
        }
    }
    else if (m == -ESTRPIPE)
    {
        qDebug ("OutputALSA: Suspend, trying to resume");
        while ((m = snd_pcm_resume(pcm_handle))
                == -EAGAIN)
            sleep (1);
        if (m < 0)
        {
            qDebug ("OutputALSA: Failed, restarting");
            if ((m = snd_pcm_prepare(pcm_handle)) < 0)
            {
                qDebug ("OutputALSA: Failed to restart device: %s.",
                        snd_strerror(m));
                return -1;
            }
        }
    }
    return -1;
}

void OutputALSA::uninitialize()
{
    if (!m_inited)
        return;
    m_inited = FALSE;
    m_pause = FALSE;
    m_play = FALSE;
    m_userStop  = FALSE;
    m_totalWritten = 0;
    m_currentSeconds = -1;
    m_bps = -1;
    m_frequency = -1;
    m_channels = -1;
    m_precision = -1;
    if (pcm_handle)
    {
        qDebug("OutputALSA: closing pcm_handle");
        snd_pcm_close(pcm_handle);
        pcm_handle = 0;
    }
}
/* ****** MIXER ******* */

int OutputALSA::setupMixer(QString card, QString device)
{
    char *name;
    long int a, b;
    long alsa_min_vol = 0, alsa_max_vol = 100;
    int err, index;
    pcm_element = 0;

    qDebug("OutputALSA: setupMixer()");

    if ((err = getMixer(&mixer, card)) < 0)
        return err;

    parseMixerName(device.toAscii().data(), &name, &index);

    pcm_element = getMixerElem(mixer, name, index);

    free(name);

    if (!pcm_element)
    {
        qWarning("OutputALSA: Failed to find mixer element");
        return -1;
    }

    /* This hack was copied from xmms.
     * Work around a bug in alsa-lib up to 1.0.0rc2 where the
     * new range don't take effect until the volume is changed.
     * This hack should be removed once we depend on Alsa 1.0.0.
     */
    snd_mixer_selem_get_playback_volume(pcm_element,
                                        SND_MIXER_SCHN_FRONT_LEFT, &a);
    snd_mixer_selem_get_playback_volume(pcm_element,
                                        SND_MIXER_SCHN_FRONT_RIGHT, &b);

    snd_mixer_selem_get_playback_volume_range(pcm_element,
            &alsa_min_vol, &alsa_max_vol);
    snd_mixer_selem_set_playback_volume_range(pcm_element, 0, 100);

    if (alsa_max_vol == 0)
    {
        pcm_element = NULL;
        return -1;
    }

    setVolume(a * 100 / alsa_max_vol, b * 100 / alsa_max_vol);

    qDebug("OutputALSA: setupMixer() success");

    return 0;
}

void OutputALSA::parseMixerName(char *str, char **name, int *index)
{
    char *end;

    while (isspace(*str))
        str++;

    if ((end = strchr(str, ',')) != NULL)
    {
        *name = strndup(str, end - str);
        end++;
        *index = atoi(end);
    }
    else
    {
        *name = strdup(str);
        *index = 0;
    }
}

snd_mixer_elem_t* OutputALSA::getMixerElem(snd_mixer_t *mixer, char *name, int index)
{
    snd_mixer_selem_id_t* selem_id;
    snd_mixer_elem_t* elem;
    snd_mixer_selem_id_alloca(&selem_id);

    if (index != -1)
        snd_mixer_selem_id_set_index(selem_id, index);
    if (name != NULL)
        snd_mixer_selem_id_set_name(selem_id, name);

    elem = snd_mixer_find_selem(mixer, selem_id);

    return elem;
}

void OutputALSA::setVolume(int l, int r)
{

    if (!pcm_element)
        return;

    snd_mixer_selem_set_playback_volume(pcm_element,
                                        SND_MIXER_SCHN_FRONT_LEFT, l);
    snd_mixer_selem_set_playback_volume(pcm_element,
                                        SND_MIXER_SCHN_FRONT_RIGHT, r);
}

void OutputALSA::volume(int *l, int *r)
{
    if (!pcm_element)
        return;

    long ll = *l, lr = *r;
    snd_mixer_handle_events(mixer);
    snd_mixer_selem_get_playback_volume(pcm_element,
                                        SND_MIXER_SCHN_FRONT_LEFT, &ll);
    snd_mixer_selem_get_playback_volume(pcm_element,
                                        SND_MIXER_SCHN_FRONT_RIGHT, &lr);
    *l = ll;
    *r = lr;
}

int OutputALSA::getMixer(snd_mixer_t **mixer, QString card)
{
    char *dev;
    int err;

    dev = strdup(card.toAscii().data());

    if ((err = snd_mixer_open(mixer, 0)) < 0)
    {
        qWarning("OutputALSA: Failed to open empty mixer: %s",
                 snd_strerror(-err));
        mixer = NULL;
        return -1;
    }
    if ((err = snd_mixer_attach(*mixer, dev)) < 0)
    {
        qWarning("OutputALSA: Attaching to mixer %s failed: %s",
                 dev, snd_strerror(-err));
        return -1;
    }
    if ((err = snd_mixer_selem_register(*mixer, NULL, NULL)) < 0)
    {
        qWarning("OutputALSA: Failed to register mixer: %s",
                 snd_strerror(-err));
        return -1;
    }
    if ((err = snd_mixer_load(*mixer)) < 0)
    {
        qWarning("OutputALSA: Failed to load mixer: %s",
                 snd_strerror(-err));
        return -1;
    }

    free(dev);

    return (*mixer != NULL);
}
