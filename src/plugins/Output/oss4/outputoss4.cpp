/***************************************************************************
 *   Copyright (C) 2010-2012 by Ilya Kotov                                 *
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
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#include <QApplication>
#include <QSettings>
#include <QDir>

extern "C"
{
#ifdef HAVE_SYS_SOUNDCARD_H
#include <sys/soundcard.h>
#else
#include <soundcard.h>
#endif
//#include </usr/lib/oss/include/sys/soundcard.h>
}

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <qmmp/buffer.h>
#include <qmmp/visual.h>
#include "outputoss4.h"

OutputOSS4 *OutputOSS4::m_instance = 0;
VolumeOSS4 *OutputOSS4::m_vc = 0;

OutputOSS4::OutputOSS4() : Output()
{
    m_audio_fd = -1;
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    m_audio_device = settings.value("OSS4/device", DEFAULT_DEV).toString();
    m_instance = this;
}

OutputOSS4::~OutputOSS4()
{
    if (m_audio_fd >= 0)
    {
        ioctl(m_audio_fd, SNDCTL_DSP_RESET, 0);
        close(m_audio_fd);
        m_audio_fd = -1;
    }
    m_instance = 0;
}

int OutputOSS4::fd()
{
    return m_audio_fd;
}

OutputOSS4 *OutputOSS4::instance()
{
    return m_instance;
}

void OutputOSS4::post()
{
    ioctl(m_audio_fd, SNDCTL_DSP_POST, 0);
}

void OutputOSS4::sync()
{
    ioctl(m_audio_fd, SNDCTL_DSP_SYNC, 0);
}

bool OutputOSS4::initialize(quint32 freq, int chan, Qmmp::AudioFormat format)
{
    m_audio_fd = open(m_audio_device.toAscii(), O_WRONLY);

    if (m_audio_fd < 0)
    {
        qWarning("OSS4Output: unable to open output device '%s'; error: %s",
                 qPrintable(m_audio_device), strerror(errno));
        return false;
    }

    ioctl(m_audio_fd, SNDCTL_DSP_RESET, 0);

    int p;
    switch (format)
    {
    case Qmmp::PCM_S32LE:
        p = AFMT_S32_LE;
        break;
    case Qmmp::PCM_S24LE:
        p = AFMT_S24_LE;
        break;
    case Qmmp::PCM_S16LE:
        p = AFMT_S16_LE;
        break;
    case Qmmp::PCM_S8:
        p = AFMT_S8;
        break;
    default:
        qWarning("OutputOSS4: unsupported audio format");
        return false;
    }

    if (ioctl(m_audio_fd, SNDCTL_DSP_SETFMT, &p) == -1)
        qWarning("OutputOSS4: ioctl SNDCTL_DSP_SETFMT failed: %s",strerror(errno));

    if(ioctl(m_audio_fd, SNDCTL_DSP_CHANNELS, &chan) == -1)
        qWarning("OutputOSS4: ioctl SNDCTL_DSP_CHANNELS failed: %s", strerror(errno));

    if (ioctl(m_audio_fd, SNDCTL_DSP_SPEED, &freq) < 0)
        qWarning("OutputOSS4: ioctl SNDCTL_DSP_SPEED failed: %s", strerror(errno));

    int enabled = 1;
    if(ioctl(m_audio_fd, SNDCTL_DSP_COOKEDMODE, &enabled) == -1)
        qWarning("OutputOSS4: ioctl SNDCTL_DSP_COOKEDMODE: %s", strerror(errno));

    ioctl(m_audio_fd, SNDCTL_DSP_RESET, 0);

    configure(freq, chan, format);

    if(m_vc)
        m_vc->restore();
    return true;
}

qint64 OutputOSS4::latency()
{
    return 0;
}

qint64 OutputOSS4::writeAudio(unsigned char *data, qint64 maxSize)
{
    qint64 m = write(m_audio_fd, data, maxSize);
    post();
    return m;
}

void OutputOSS4::drain()
{
    ioctl(m_audio_fd, SNDCTL_DSP_SYNC, 0);
}

void OutputOSS4::reset()
{
    ioctl(m_audio_fd, SNDCTL_DSP_SKIP, 0);
}

/***** MIXER *****/
VolumeOSS4::VolumeOSS4()
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    m_volume = settings.value("OSS4/volume", 0x3232).toInt();
    OutputOSS4::m_vc = this;
}

VolumeOSS4::~VolumeOSS4()
{
    VolumeSettings vol = volume();
    m_volume = (vol.right << 8) | vol.left;
    OutputOSS4::m_vc = 0;
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.setValue("OSS4/volume", m_volume);
}

void VolumeOSS4::setVolume(const VolumeSettings &vol)
{
    m_volume = (vol.right << 8) | vol.left;
    if(OutputOSS4::instance() && OutputOSS4::instance()->fd() >= 0)
    {
        ioctl(OutputOSS4::instance()->fd(), SNDCTL_DSP_SETPLAYVOL, &m_volume);
    }
}

VolumeSettings VolumeOSS4::volume() const
{
    VolumeSettings vol;
    if(OutputOSS4::instance() && OutputOSS4::instance()->fd() >= 0)
    {
        int v = 0;
        if (ioctl(OutputOSS4::instance()->fd(), SNDCTL_DSP_GETPLAYVOL, &v) < 0)
            v = 0;
        vol.left = v & 0x00FF;
        vol.right = (v & 0xFF00) >> 8;
    }
    else
    {
        vol.left = m_volume & 0x00FF;
        vol.right = (m_volume & 0xFF00) >> 8;
    }
    return vol;
}

void VolumeOSS4::restore()
{
    if(OutputOSS4::instance() && OutputOSS4::instance()->fd() >= 0)
    {
        ioctl(OutputOSS4::instance()->fd(), SNDCTL_DSP_SETPLAYVOL, &m_volume);
    }
}
