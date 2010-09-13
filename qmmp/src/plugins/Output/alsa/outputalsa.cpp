/***************************************************************************
 *   Copyright (C) 2006-2010 by Ilya Kotov                                 *
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

#include <qmmp/buffer.h>
#include <qmmp/visual.h>
#include <qmmp/statehandler.h>
#include "outputalsa.h"

OutputALSA::OutputALSA(QObject * parent)
        : Output(parent), m_inited(false)
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    QString dev_name = settings.value("ALSA/device","default").toString();
    m_use_mmap = settings.value("ALSA/use_mmap", false).toBool();
    pcm_name = strdup(dev_name.toAscii().data());
    pcm_handle = 0;
    m_prebuf = 0;
    m_prebuf_size = 0;
    m_prebuf_fill = 0;
    m_can_pause = false;
}

OutputALSA::~OutputALSA()
{
    uninitialize();
    free (pcm_name);
}

void OutputALSA::configure(quint32 freq, int chan, Qmmp::AudioFormat format)
{
    // we need to configure

    uint rate = freq; /* Sample rate */
    uint exact_rate = freq;   /* Sample rate returned by */

    /* load settings from config */
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("ALSA");
    uint buffer_time = settings.value("buffer_time",500).toUInt()*1000;
    uint period_time = settings.value("period_time",100).toUInt()*1000;
    bool use_pause =  settings.value("use_snd_pcm_pause", false).toBool();
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
            m_use_mmap = false;
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
    snd_pcm_format_t alsa_format = SND_PCM_FORMAT_UNKNOWN;
    switch (format)
    {
    case Qmmp::PCM_S8:
        alsa_format = SND_PCM_FORMAT_S8;
        break;
    case Qmmp::PCM_S16LE:
        alsa_format = SND_PCM_FORMAT_S16_LE;
        break;
    case Qmmp::PCM_S24LE:
        alsa_format = SND_PCM_FORMAT_S24_LE;
        break;
    case Qmmp::PCM_S32LE:
        alsa_format = SND_PCM_FORMAT_S32_LE;
        break;
    default:
        qWarning("OutputALSA: unsupported format detected");
        return;
    }
    if ((err = snd_pcm_hw_params_set_format(pcm_handle, hwparams, alsa_format)) < 0)
    {
        qDebug("OutputALSA: Error setting format: %s", snd_strerror(err));
        return;
    }
    exact_rate = rate;

    if ((err = snd_pcm_hw_params_set_rate_near(pcm_handle, hwparams, &exact_rate, 0)) < 0)
    {
        qWarning("OutputALSA: Error setting rate: %s", snd_strerror(err));
        return;
    }
    if (rate != exact_rate)
    {
        qWarning("OutputALSA: The rate %d Hz is not supported by your hardware.\n==> Using %d Hz instead.", rate, exact_rate);
    }
    uint c = chan;
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
    m_bits_per_frame = snd_pcm_format_physical_width(alsa_format) * chan;
    m_chunk_size = period_size;
    m_can_pause = snd_pcm_hw_params_can_pause(hwparams) && use_pause;
    qDebug("OutputALSA: can pause: %d", m_can_pause);
    Output::configure(freq, chan, format); //apply configuration
    //create alsa prebuffer;
    m_prebuf_size = QMMP_BUFFER_SIZE + m_bits_per_frame * m_chunk_size / 8;
    m_prebuf = (uchar *)malloc(m_prebuf_size);
}

bool OutputALSA::initialize()
{
    m_inited = false;

    if (pcm_handle)
        return false;

    if (snd_pcm_open(&pcm_handle, pcm_name, SND_PCM_STREAM_PLAYBACK, SND_PCM_NONBLOCK) < 0)
    {
        qWarning ("OutputALSA: Error opening PCM device %s", pcm_name);
        return false;
    }

    m_inited = true;
    return true;
}


qint64 OutputALSA::latency()
{
    return m_prebuf_fill * 1000 / sampleRate() / channels() / sampleSize();
}

void OutputALSA::drain()
{
    long m = 0;
    snd_pcm_uframes_t l = snd_pcm_bytes_to_frames(pcm_handle, m_prebuf_fill);
    while (l > 0)
    {
        if ((m = alsa_write(m_prebuf, l)) >= 0)
        {
            l -= m;
            m = snd_pcm_frames_to_bytes(pcm_handle, m); // convert frames to bytes
            m_prebuf_fill -= m;
            memcpy(m_prebuf, m_prebuf + m, m_prebuf_fill);
        }
        else
            break;
    }
    snd_pcm_nonblock(pcm_handle, 0);
    snd_pcm_drain(pcm_handle);
    snd_pcm_nonblock(pcm_handle, 1);
}

void OutputALSA::reset()
{
    m_prebuf_fill = 0;
    snd_pcm_drop(pcm_handle);
    snd_pcm_prepare(pcm_handle);
}

void OutputALSA::suspend()
{
    if (m_can_pause)
        snd_pcm_pause(pcm_handle, 1);
    snd_pcm_prepare(pcm_handle);
}

void OutputALSA::resume()
{
    if (m_can_pause)
        snd_pcm_pause(pcm_handle, 0);
    snd_pcm_prepare(pcm_handle);
}

qint64 OutputALSA::writeAudio(unsigned char *data, qint64 maxSize)
{
    if((maxSize = qMin(maxSize, m_prebuf_size - m_prebuf_fill)) > 0)
    {
        memcpy(m_prebuf + m_prebuf_fill, data, maxSize);
        m_prebuf_fill += maxSize;
    }

    snd_pcm_uframes_t l = snd_pcm_bytes_to_frames(pcm_handle, m_prebuf_fill);

    while (l >= m_chunk_size)
    {
        snd_pcm_wait(pcm_handle, 10);
        long m;
        if ((m = alsa_write(m_prebuf, m_chunk_size)) >= 0)
        {
            l -= m;
            m = snd_pcm_frames_to_bytes(pcm_handle, m); // convert frames to bytes
            m_prebuf_fill -= m;
            memcpy(m_prebuf, m_prebuf + m, m_prebuf_fill); //move data to begin
        }
        else
            return -1;
    }
    return maxSize;
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
        snd_pcm_wait(pcm_handle, 500);
        return 0;
    }
    else if (m >= 0)
    {
        if (m < size)
        { 
            snd_pcm_wait(pcm_handle, 500);  
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
        return 0;
    }
#ifdef ESTRPIPE
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
        return 0;
    }
#endif
    qDebug ("OutputALSA: error: %s", snd_strerror(m));
    return snd_pcm_prepare (pcm_handle);
}

void OutputALSA::uninitialize()
{
    if (!m_inited)
        return;
    m_inited = false;
    if (pcm_handle)
    {
        snd_pcm_drop(pcm_handle);
        qDebug("OutputALSA: closing pcm_handle");
        snd_pcm_close(pcm_handle);
        pcm_handle = 0;
    }
    if (m_prebuf)
        free(m_prebuf);
    m_prebuf = 0;
}
/* ****** MIXER ******* */

VolumeControlALSA::VolumeControlALSA(QObject *parent) : VolumeControl(parent)
{
    //alsa mixer
    mixer = 0;
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    QString card = settings.value("ALSA/mixer_card","hw:0").toString();
    QString dev = settings.value("ALSA/mixer_device", "PCM").toString();
    setupMixer(card, dev);
}


VolumeControlALSA::~VolumeControlALSA()
{
    if (mixer)
        snd_mixer_close(mixer);
}

void VolumeControlALSA::setVolume(int l, int r)
{

    if (!pcm_element)
        return;

    snd_mixer_selem_set_playback_volume(pcm_element,
                                        SND_MIXER_SCHN_FRONT_LEFT, l);
    snd_mixer_selem_set_playback_volume(pcm_element,
                                        SND_MIXER_SCHN_FRONT_RIGHT, r);
}

void VolumeControlALSA::volume(int *l, int *r)
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



int VolumeControlALSA::setupMixer(QString card, QString device)
{
    char *name;
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

    if((err = snd_mixer_selem_set_playback_volume_range(pcm_element, 0, 100)) < 0)
    {
        qWarning("OutputALSA: Unable to set volume range: %s", snd_strerror(-err));
        pcm_element = NULL;
        return -1;
    }

    qDebug("OutputALSA: setupMixer() success");
    return 0;
}

void VolumeControlALSA::parseMixerName(char *str, char **name, int *index)
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

snd_mixer_elem_t* VolumeControlALSA::getMixerElem(snd_mixer_t *mixer, char *name, int index)
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

int VolumeControlALSA::getMixer(snd_mixer_t **mixer, QString card)
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
