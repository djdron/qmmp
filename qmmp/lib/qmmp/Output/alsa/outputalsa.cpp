/***************************************************************************
 *   Copyright (C) 2006 by Ilya Kotov                                      *
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
#include <string.h>
#include <iostream>

#include "outputalsa.h"
#include "constants.h"
#include "buffer.h"
#include "visual.h"

OutputALSA::OutputALSA(QObject * parent, bool useVolume)
        : Output(parent), m_inited(FALSE), m_pause(FALSE), m_play(FALSE),
        m_userStop(FALSE), m_totalWritten(0), m_currentSeconds(-1),
        m_bps(-1), m_frequency(-1), m_channels(-1), m_precision(-1)
{
    QSettings settings(QDir::homePath()+"/.qmmp/qmmprc", QSettings::IniFormat);
    QString dev_name = settings.value("ALSA/device","default").toString();
    pcm_name = strdup(dev_name.toAscii().data());
    stream = SND_PCM_STREAM_PLAYBACK;
    snd_pcm_hw_params_alloca(&hwparams);
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

long OutputALSA::written()
{
    return m_totalWritten;
}

void OutputALSA::seek(long pos)
{
    m_totalWritten = (pos * m_bps);
    m_currentSeconds = -1;
}

void OutputALSA::configure(long freq, int chan, int prec, int brate)
{
    // we need to configure
    if (freq != m_frequency || chan != m_channels || prec != m_precision)
    {
        m_frequency = freq;
        m_channels = chan;
        m_precision = prec;
        m_bps = freq * chan * (prec / 8);
        snd_pcm_hw_params_alloca(&hwparams);
        if (snd_pcm_hw_params_any(pcm_handle, hwparams) < 0)
        {
            qWarning("OutputALSA: Can not configure this PCM device.");
            return;
        }

        uint rate = m_frequency; /* Sample rate */
        uint exact_rate = m_frequency;   /* Sample rate returned by */

        /* load settings from config */
        QSettings settings(QDir::homePath()+"/.qmmp/qmmprc", QSettings::IniFormat);
        settings.beginGroup("ALSA");
        uint buffer_time = settings.value("buffer_time",500).toUInt()*1000;
        uint period_time = settings.value("period_time",100).toUInt()*1000;
        settings.endGroup();

        if (snd_pcm_hw_params_set_access(pcm_handle, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED) < 0)
        {
            qWarning("OutputALSA: Error setting access.");
            return;
        }


        if (snd_pcm_hw_params_set_format(pcm_handle, hwparams, SND_PCM_FORMAT_S16_LE) < 0)
        {
            qDebug("OutputALSA: Error setting format.");
            return;
        }


        exact_rate = rate;// = 11000;
        qDebug("OutputALSA: frequency=%d, channels=%d, bitrate=%d",
               rate, chan, brate);
        if (snd_pcm_hw_params_set_rate_near(pcm_handle, hwparams, &exact_rate, 0) < 0)
        {
            qWarning("OutputALSA: Error setting rate.\n");
            return;
        }
        if (rate != exact_rate)
        {
            qWarning("OutputALSA: The rate %d Hz is not supported by your hardware.\n==> Using %d Hz instead.", rate, exact_rate);
        }

        uint c = m_channels;
        if (snd_pcm_hw_params_set_channels_near(pcm_handle, hwparams, &c) < 0)
        {
            qWarning("OutputALSA: Error setting channels.");
            return;
        }

        if (snd_pcm_hw_params_set_period_time_near(pcm_handle, hwparams,
                &period_time ,0) < 0 )
        {
            qWarning("OutputALSA: Error setting HW buffer.");
            return;
        }
        if (snd_pcm_hw_params_set_buffer_time_near(pcm_handle, hwparams,
                &buffer_time ,0) < 0 )
        {
            qWarning("Error setting HW buffer.\n");
            return;
        }
        if (snd_pcm_hw_params(pcm_handle, hwparams) < 0)
        {
            qWarning("OutputALSA: Error setting HW params.");
            return;
        }
    }
}

void OutputALSA::reset()
{
    if (pcm_handle)
    {
        snd_pcm_close(pcm_handle);
        pcm_handle = 0;
    }
    if (snd_pcm_open(&pcm_handle, pcm_name, stream, SND_PCM_NONBLOCK) < 0)
    {
        qWarning ("OutputALSA: Error opening PCM device %s", pcm_name);
        return;
    }
}


void OutputALSA::pause()
{
    if (!m_play)
        return;
    m_pause = (m_pause) ? FALSE : TRUE;
    OutputState::Type state = m_pause ? OutputState::Paused: OutputState::Playing;
    dispatch(state);
}

bool OutputALSA::initialize()
{
    m_inited = m_pause = m_play = m_userStop = FALSE;

    if (!pcm_handle < 0)
        return FALSE;

    m_currentSeconds = -1;
    m_totalWritten = 0;
    if (snd_pcm_open(&pcm_handle, pcm_name, stream, SND_PCM_NONBLOCK) < 0)
    {
        qWarning ("OutputALSA: Error opening PCM device %s", pcm_name);
        return FALSE;
    }

    m_inited = TRUE;
    return TRUE;
}


long OutputALSA::latency()
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
    unsigned long n = 0;
    long m = 0;
    snd_pcm_uframes_t l;

    dispatch(OutputState::Playing);

    while (! done)
    {
        mutex()->lock ();
        recycler()->mutex()->lock ();

        done = m_userStop;

        while (! done && (recycler()->empty() || m_pause))
        {
            mutex()->unlock();
            recycler()->cond()->wakeOne();
            recycler()->cond()->wait(recycler()->mutex());
            mutex()->lock ();
            done = m_userStop;
            status();
        }

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
            l = snd_pcm_bytes_to_frames(pcm_handle, b->nbytes - n);
            while (l>0)
            {
                m = snd_pcm_writei (pcm_handle, b->data+n, l);

                if (m > 0)
                {
                    n += snd_pcm_frames_to_bytes(pcm_handle, m);
                    l -= m;
                    status();
                    dispatchVisual(b, m_totalWritten, m_channels, m_precision);
                }
                else if (m == -EAGAIN)
                {
                    mutex()->unlock();
                    snd_pcm_wait(pcm_handle, 500);
                    mutex()->lock ();
                }
                else if (m == -EPIPE)
                {
                    qDebug ("OutputALSA: underrun!");
                    if ((m = snd_pcm_prepare(pcm_handle)) < 0)
                    {
                        qDebug ("OutputALSA: Can't recover after underrun: %s",
                                snd_strerror(m));
                        /* TODO: reopen the device */
                        break;
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
                        if ((m = snd_pcm_prepare(pcm_handle))
                                < 0)
                        {
                            qDebug ("OutputALSA: Failed to restart device: %s.",
                                    snd_strerror(m));
                            break;
                        }
                    }
                }
                else if (m < 0)
                {
                    qDebug ("OutputALSA: Can't play: %s", snd_strerror(m));
                    break;
                }
            }
            status();
            // force buffer change
            m_totalWritten += n;
            n = b->nbytes;
            m = 0;
        }
        if (n == b->nbytes)
        {
            recycler()->mutex()->lock ();
            recycler()->done();
            recycler()->mutex()->unlock();
            b = 0;
            n = 0;
        }
        mutex()->unlock();
    }

    mutex()->lock ();

    m_play = FALSE;

    dispatch(OutputState::Stopped);

    mutex()->unlock();

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
    dispatch(OutputState::Stopped);
}
/* ****** MIXER ******* */

int OutputALSA::setupMixer(QString card, QString device)
{
    char *name;
    long int a, b;
    long alsa_min_vol = 0, alsa_max_vol = 100;
    int err, index;

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

    qDebug("OutputALSA: setupMixer() succes");

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

void OutputALSA::volume(int * l, int * r)
{
    if (!pcm_element)
        return;
    snd_mixer_selem_get_playback_volume(pcm_element,
                                        SND_MIXER_SCHN_FRONT_LEFT, (long int*)l);
    snd_mixer_selem_get_playback_volume(pcm_element,
                                        SND_MIXER_SCHN_FRONT_RIGHT, (long int*)r);
					
    *l = (*l > 100) ? 100 : *l;
    *r = (*r > 100) ? 100 : *r;
    *l = (*l < 0) ? 0 : *l;
    *r = (*r < 0) ? 0 : *r;
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

void OutputALSA::checkVolume()
{
    long ll = 0, lr = 0;

    if (!pcm_element)
        return;

    snd_mixer_handle_events(mixer);

    snd_mixer_selem_get_playback_volume(pcm_element,
                                        SND_MIXER_SCHN_FRONT_LEFT,
                                        &ll);
    snd_mixer_selem_get_playback_volume(pcm_element,
                                        SND_MIXER_SCHN_FRONT_RIGHT,
                                        &lr);
    //qDebug("%d, %d",ll, lr);

    ll = (ll > 100) ? 100 : ll;
    lr = (lr > 100) ? 100 : lr;
    ll = (ll < 0) ? 0 : ll;
    lr = (lr < 0) ? 0 : lr;
    if (bl!=ll || br!=lr)
    {
        bl = ll;
        br = lr;
        dispatchVolume(ll,lr);
    }
}



