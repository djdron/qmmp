/***************************************************************************
 *   Copyright (C) 2007 by Uriy Zhuravlev stalkerg@gmail.com               *
 *                                                                         *
 *   Copyright (c) 2000-2001 Brad Hughes bhughes@trolltech.com             *
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
#include "outputoss.h"

OutputOSS::OutputOSS(QObject * parent) : Output(parent), do_select(true), m_audio_fd(-1)
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    m_audio_device = settings.value("OSS/device","/dev/dsp").toString();
}

OutputOSS::~OutputOSS()
{
    if (m_audio_fd >= 0)
    {
        ioctl(m_audio_fd, SNDCTL_DSP_RESET, 0);
        close(m_audio_fd);
        m_audio_fd = -1;
    }
}

void OutputOSS::post()
{
    ioctl(m_audio_fd, SNDCTL_DSP_POST, 0);
}

void OutputOSS::sync()
{
    ioctl(m_audio_fd, SNDCTL_DSP_SYNC, 0);
}

bool OutputOSS::initialize(quint32 freq, int chan, Qmmp::AudioFormat format)
{
    m_audio_fd = open(m_audio_device.toAscii(), O_WRONLY, 0);

    if (m_audio_fd < 0)
    {
        qWarning("OSSOutput: failed to open output device '%s'", qPrintable(m_audio_device));
        return false;
    }

    int flags;
    if ((flags = fcntl(m_audio_fd, F_GETFL, 0)) > 0)
    {
        flags &= O_NDELAY;
        fcntl(m_audio_fd, F_SETFL, flags);
    }
    fd_set afd;
    FD_ZERO(&afd);
    FD_SET(m_audio_fd, &afd);
    struct timeval tv;
    tv.tv_sec = 0l;
    tv.tv_usec = 50000l;
    do_select = (select(m_audio_fd + 1, 0, &afd, 0, &tv) > 0);

    int p;
    switch (format)
    {
    case Qmmp::PCM_S16LE:
#ifdef AFMT_S16_NE
    p = AFMT_S16_NE;
#else
    p = AFMT_S16_LE;
#endif
        break;
    case Qmmp::PCM_S8:
        p = AFMT_S8;
        break;
    default:
        qWarning("OutputOSS: unsupported audio format");
        return false;
    }
    int param = p;
    if (ioctl(m_audio_fd, SNDCTL_DSP_SETFMT, &p) < 0)
    {
        qWarning("OutputOSS: ioctl SNDCTL_DSP_SETFMT failed: %s",strerror(errno));
        //return;
    }
    /*if(param != p)
    {
        qWarning("OutputOSS: unsupported audio format");
        return;
    }*/
    param = chan;
    if(ioctl(m_audio_fd, SNDCTL_DSP_CHANNELS, &chan) < 0)
    {
        qWarning("OutputOSS: ioctl SNDCTL_DSP_CHANNELS failed: %s", strerror(errno));
        return false;
    }
    if(param != chan)
    {
        qWarning("OutputOSS: unsupported %d-channel mode", param);
        return false;
    }
    uint param2 = freq;
    if (ioctl(m_audio_fd, SNDCTL_DSP_SPEED, &freq) < 0)
    {
        qWarning("OutputOSS: ioctl SNDCTL_DSP_SPEED failed: %s", strerror(errno));
        return false;
    }
    if(param2 != freq)
    {
        qWarning("OutputOSS: unsupported sample rate");
        return false;
    }
    ioctl(m_audio_fd, SNDCTL_DSP_RESET, 0);
    configure(freq, chan, format);
    return true;
}

qint64 OutputOSS::latency()
{
    //ulong used = 0;

    /*if (ioctl(m_audio_fd, SNDCTL_DSP_GETODELAY, &used) == -1)
        used = 0;*/
    return 0;
}

qint64 OutputOSS::writeAudio(unsigned char *data, qint64 maxSize)
{
    fd_set afd;
    struct timeval tv;
    qint64 m = -1, l;
    FD_ZERO(&afd);
    FD_SET(m_audio_fd, &afd);
    // nice long poll timeout
    tv.tv_sec = 5l;
    tv.tv_usec = 0l;
    if ((!do_select || (select(m_audio_fd + 1, 0, &afd, 0, &tv) > 0 &&
                                 FD_ISSET(m_audio_fd, &afd))))
    {
        l = qMin(int(2048), int(maxSize));
        if (l > 0)
        {
             m = write(m_audio_fd, data, l);
        }
    }
    post();
    return m;
}

void OutputOSS::drain()
{
    ioctl(m_audio_fd, SNDCTL_DSP_SYNC, 0);
}

void OutputOSS::reset()
{
    ioctl(m_audio_fd, SNDCTL_DSP_RESET, 0);
}

/***** MIXER *****/
VolumeControlOSS::VolumeControlOSS(QObject *parent) : VolumeControl(parent)
{
    m_master = true;
    m_mixer_fd = -1;
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    m_mixer_device = settings.value("OSS/mixer_device","/dev/mixer").toString();
    openMixer();

}

VolumeControlOSS::~VolumeControlOSS()
{
    if (m_mixer_fd >= 0)
    {
        close(m_mixer_fd);
        m_mixer_fd = -1;
    }
}

void VolumeControlOSS::setVolume(int l, int r)
{
    if (m_mixer_fd < 0)
        return;
    int v;
    long cmd;
    int devs = 0;
    ioctl(m_mixer_fd, SOUND_MIXER_READ_DEVMASK, &devs);
    if ((devs & SOUND_MASK_PCM) && !m_master)
        cmd = SOUND_MIXER_WRITE_PCM;
    else if ((devs & SOUND_MASK_VOLUME) && m_master)
        cmd = SOUND_MIXER_WRITE_VOLUME;
    else
    {
        //close(mifd);
        return;
    }
    v = (r << 8) | l;
    ioctl(m_mixer_fd, cmd, &v);
}

void VolumeControlOSS::volume(int *ll,int *rr)
{
    *ll = 0;
    *rr = 0;
    if(m_mixer_fd < 0)
        return;
    int cmd;
    int v, devs = 0;
    ioctl(m_mixer_fd, SOUND_MIXER_READ_DEVMASK, &devs);
    if ((devs & SOUND_MASK_PCM) && !m_master)
        cmd = SOUND_MIXER_READ_PCM;

    else if ((devs & SOUND_MASK_VOLUME) && m_master)
        cmd = SOUND_MIXER_READ_VOLUME;
    else
        return;

    ioctl(m_mixer_fd, cmd, &v);
    *ll = (v & 0xFF00) >> 8;
    *rr = (v & 0x00FF);

    *ll = (*ll > 100) ? 100 : *ll;
    *rr = (*rr > 100) ? 100 : *rr;
    *ll = (*ll < 0) ? 0 : *ll;
    *rr = (*rr < 0) ? 0 : *rr;
}

void VolumeControlOSS::openMixer()
{
    if (m_mixer_fd >= 0)
        return;
    m_mixer_fd = open(m_mixer_device.toAscii(), O_RDWR);
    if (m_mixer_fd < 0)
    {
        qWarning("VolumeControlOSS: unable to open mixer device '%s'", qPrintable(m_mixer_device));
        return;
    }
}
