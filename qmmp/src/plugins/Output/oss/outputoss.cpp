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

extern "C"
{
#ifdef HAVE_SYS_SOUNDCARD_H
#include <sys/soundcard.h>
#else
#include <soundcard.h>
#endif
}

#include "outputoss.h"
#include <qmmp/constants.h>
#include <qmmp/buffer.h>
#include <qmmp/visual.h>

#include <stdio.h>
#include <string.h>
#include <QtGlobal>
#include <QSettings>
#include <QDir>

#include <iostream>

#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/time.h>

#if defined(__FreeBSD__)
#  include <sys/soundcard.h>
#elif defined(__linux__)
#  include <linux/soundcard.h>
#elif defined(__bsdi__)
#  include <sys/soundcard.h>
#endif

OutputOSS *OutputOSS::m_instance = 0;
VolumeControlOSS *VolumeControlOSS::m_instance = 0;

OutputOSS* OutputOSS::instance()
{
    return m_instance;
}

OutputOSS::OutputOSS(QObject * parent)
        : Output(parent), m_inited(FALSE),
         m_frequency(-1), m_channels(-1), m_precision(-1),
        do_select(TRUE),
        m_audio_fd(-1)
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    m_audio_device = settings.value("OSS/device","/dev/dsp").toString();
    m_instance = this;
}

int OutputOSS::audio_fd()
{
    return m_audio_fd;
}

OutputOSS::~OutputOSS()
{
     m_instance = 0;
    if (m_audio_fd > 0)
    {
        close(m_audio_fd);
        m_audio_fd = -1;
    }
}

void OutputOSS::configure(quint32 freq, int chan, int prec)
{
    // we need to configure
    if (freq != m_frequency || chan != m_channels || prec != m_precision)
    {
        qDebug("OutputOSS: frequency=%d, channels=%d, bits=%d", freq, chan, prec);
        // we have already configured, but are changing settings...
        // reset the device
        resetDSP();
#if SOUND_VERSION >= 0x040000
    if (VolumeControlOSS::instance())
    {
        long cmd;
        int v;
        cmd = SNDCTL_DSP_SETPLAYVOL;
        v = (VolumeControlOSS::instance()->right() << 8) | VolumeControlOSS::instance()->left();
        if(m_audio_fd > 1)
            ioctl(m_audio_fd, cmd, &v);
    }
#endif
        m_frequency = freq;
        m_channels = chan;
        m_precision = prec;

        //m_bps = freq * chan * (prec / 8);

        int p;
        switch (prec)
        {
        default:
        case 16:
#if defined(AFMT_S16_NE)
            p = AFMT_S16_NE;
#else
            p = AFMT_S16_LE;
#endif
            break;

        case 8:
            p = AFMT_S8;
            break;

        }

        if (ioctl(m_audio_fd, SNDCTL_DSP_SETFMT, &p) == -1)
            qWarning("OutputOSS: can't set audio format");
        if(ioctl(m_audio_fd, SNDCTL_DSP_SAMPLESIZE, &prec) == -1)
            qDebug("OutputOSS: can't set audio format");
        int stereo = (chan > 1) ? 1 : 0;
            ioctl(m_audio_fd, SNDCTL_DSP_STEREO, &stereo);
        /*if (ioctl(m_audio_fd, SNDCTL_DSP_SPEED, &m_channels) == -1)
            qWarning("OutputOSS: can't set number of channels");*/
        /*if (chan != m_channels)
            qWarning("OutputOSS: can't set number of channels, using %d instead", m_channels);*/
        if (ioctl(m_audio_fd, SNDCTL_DSP_SPEED, &freq) == -1)
            qWarning("OutputOSS: can't set audio format");
    }
    Output::configure(freq, chan, prec);
}

void OutputOSS::reset()
{
    if (m_audio_fd > 0)
    {
        close(m_audio_fd);
        m_audio_fd = -1;
    }

    m_audio_fd = open(m_audio_device.toAscii(), O_WRONLY, 0);

    if (m_audio_fd < 0)
    {
        qWarning("OSSOutput: failed to open output device '%s'", qPrintable(m_audio_device));
        return;
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
}

void OutputOSS::post()
{
    if (m_audio_fd < 1)
        return;

    int unused;
    ioctl(m_audio_fd, SNDCTL_DSP_POST, &unused);
}

void OutputOSS::sync()
{
    if (m_audio_fd < 1)
        return;

    int unused;
    ioctl(m_audio_fd, SNDCTL_DSP_SYNC, &unused);
}


void OutputOSS::resetDSP()
{
    if (m_audio_fd < 1)
        return;

    int unused;
    ioctl(m_audio_fd, SNDCTL_DSP_RESET, &unused);
}


bool OutputOSS::initialize()
{
    reset();
    if (m_audio_fd < 0)
        return FALSE;
    m_inited = TRUE;
    return TRUE;
}

void OutputOSS::uninitialize()
{
    if (!m_inited)
        return;
    m_inited = FALSE;
    m_frequency = -1;
    m_channels = -1;
    m_precision = -1;
    resetDSP();
    if (m_audio_fd > 0)
    {
        close(m_audio_fd);
        m_audio_fd = -1;
    }
    qDebug("OutputOSS: uninitialize");
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
    if ((! do_select || (select(m_audio_fd + 1, 0, &afd, 0, &tv) > 0 &&
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

/***** MIXER *****/

VolumeControlOSS *VolumeControlOSS::instance()
{
    return m_instance;
}

VolumeControlOSS::VolumeControlOSS(QObject *parent) : VolumeControl(parent)
{
    m_master = TRUE;
    m_mixer_fd = -1;
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
#if SOUND_VERSION < 0x040000
    m_mixer_device = settings.value("OSS/mixer_device","/dev/mixer").toString();
    openMixer();
#else
    m_mixer_device = settings.value("OSS/device","/dev/dsp").toString();
    int mixer_fd = -1;
    bool to_close = FALSE;
    if (OutputOSS::instance() && OutputOSS::instance()->audio_fd() > 0)
        mixer_fd = OutputOSS::instance()->audio_fd();
    else
    {
        mixer_fd = open(m_audio_device.toAscii(), O_WRONLY, 0);
        to_close = TRUE;
    }
    if(mixer_fd > 0)
    {
        int v;
        long cmd = SNDCTL_DSP_GETPLAYVOL;
        if (ioctl(mixer_fd, cmd, &v) == -1)
            v = 0;
        m_left2 = (v & 0xFF00) >> 8;
        m_right2 = (v & 0x00FF);
    }
    if(to_close)
    {
        close(mixer_fd);
            mixer_fd = -1;
    }
#endif
    m_instance = this;
}

VolumeControlOSS::~VolumeControlOSS()
{
#if SOUND_VERSION < 0x040000
    if (m_mixer_fd > 0)
    {
        close(m_mixer_fd);
        m_mixer_fd = -1;
    }
#endif
    if (m_mixer_fd > 0 && !OutputOSS::instance())
    {
        close(m_mixer_fd);
        m_mixer_fd = -1;
    }
    m_instance = 0;
}

void VolumeControlOSS::setVolume(int l, int r)
{
    int v;
    long cmd;

#if SOUND_VERSION < 0x040000
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
#else
    cmd = SNDCTL_DSP_SETPLAYVOL;
    v = (r << 8) | l;
    if (OutputOSS::instance() && OutputOSS::instance()->audio_fd() > 0)
        ioctl(OutputOSS::instance()->audio_fd(), cmd, &v);
    m_left2 = l;
    m_right2 = r;
#endif
}

void VolumeControlOSS::volume(int *ll,int *rr)
{
    *ll = 0;
    *rr = 0;
#if SOUND_VERSION < 0x040000
    int  cmd;
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
#else
    /*cmd = SNDCTL_DSP_GETPLAYVOL;
    if (ioctl(m_audio_fd, cmd, &v) == -1)
        v = 0;
    *rr = (v & 0xFF00) >> 8;
    *ll = (v & 0x00FF);*/
    *rr = m_left2;
    *ll = m_right2;
#endif
}

void VolumeControlOSS::openMixer()
{
#if SOUND_VERSION < 0x040000
    if (m_mixer_fd != -1)
        return;
    m_mixer_fd = open(m_mixer_device.toAscii(), O_RDWR);
    if (m_mixer_fd < 0)
    {
        qWarning("VolumeControlOSS: unable to open mixer device '%s'", qPrintable(m_mixer_device));
        return;
    }
#endif
}
