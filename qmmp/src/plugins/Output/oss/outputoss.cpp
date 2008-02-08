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

//extern Q_EXPORT QApplication* qApp;


void OutputOSS::stop()
{
    m_userStop = TRUE;
}

void OutputOSS::status()
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

long OutputOSS::written()
{
    return m_totalWritten;
}

void OutputOSS::seek(long pos)
{
    recycler()->mutex()->lock();
    recycler()->clear();
    recycler()->mutex()->unlock();

    m_totalWritten = (pos * m_bps);
    m_currentSeconds = -1;
}


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


OutputOSS::OutputOSS(QObject * parent)
    : Output(parent), m_inited(FALSE), m_pause(FALSE), m_play(FALSE),
      m_userStop(FALSE),
      m_totalWritten(0), m_currentSeconds(-1),
      m_bps(1), m_frequency(-1), m_channels(-1), m_precision(-1),
      do_select(TRUE),
      m_audio_fd(-1), m_mixer_fd(-1)
{
QSettings settings(QDir::homePath()+"/.qmmp/qmmprc", QSettings::IniFormat);
m_master = true;
m_audio_device = settings.value("OSS/device","/dev/dsp").toString();
m_mixer_device = settings.value("OSS/mixer_device","/dev/mixer").toString();
openMixer();
}

OutputOSS::~OutputOSS()
{
    if (m_audio_fd > 0)
    {
	close(m_audio_fd);
	m_audio_fd = -1;
    }
    if (m_mixer_fd > 0)
    {
	close(m_mixer_fd);
	m_mixer_fd = -1;
    }
}

void OutputOSS::configure(long freq, int chan, int prec, int rate)
{
    // we need to configure
    if (freq != m_frequency || chan != m_channels || prec != m_precision) {
	// we have already configured, but are changing settings...
	// reset the device
	resetDSP();

	m_frequency = freq;
	m_channels = chan;
	m_precision = prec;

	m_bps = freq * chan * (prec / 8);

	int p;
	switch(prec) {
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

	ioctl(m_audio_fd, SNDCTL_DSP_SETFMT, &p);
	ioctl(m_audio_fd, SNDCTL_DSP_SAMPLESIZE, &prec);
	int stereo = (chan > 1) ? 1 : 0;
	ioctl(m_audio_fd, SNDCTL_DSP_STEREO, &stereo);
	ioctl(m_audio_fd, SNDCTL_DSP_SPEED, &freq);
    }

    m_rate = rate;
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
	error(QString("OSSOutput: failed to open output device '%1'").
	      arg(m_audio_device));
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

    if (m_audio_fd > 0)
    {
	close(m_mixer_fd);
	m_mixer_fd = -1;
    }
    openMixer();
}

void OutputOSS::openMixer()
{
    if (m_mixer_fd != -1)
        return;

    if ((m_mixer_fd = open(m_mixer_device.toAscii(), O_RDWR)) == -1)
    {
        return;
    }
    if (m_audio_fd < 0)
    {
	error(QString("OSSOutput: failed to open mixer device '%1'").
	      arg(m_mixer_device));
	return;
    }
}

void OutputOSS::pause()
{
    m_pause = (m_pause) ? FALSE : TRUE;
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
    m_inited = m_pause = m_play = m_userStop = FALSE;


    reset();
    if (m_audio_fd < 0)
	return FALSE;
    if (m_mixer_fd < 0)
	return FALSE;

    
    m_currentSeconds = -1;
    m_totalWritten = 0;
    stat = OutputState::Stopped;

    m_inited = TRUE;
    return TRUE;
}

void OutputOSS::uninitialize()
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
    resetDSP();
    if (m_audio_fd > 0)
    {
	close(m_audio_fd);
	m_audio_fd = -1;
    }
    if (m_audio_fd > 0)
    {
	close(m_mixer_fd);
	m_mixer_fd = -1;
    }

    qDebug("OutputOSS: uninitialize");
    dispatch(OutputState::Stopped);
}

long OutputOSS::latency()
{
    ulong used = 0;

    if (! m_pause)
    {
	if (ioctl(m_audio_fd, SNDCTL_DSP_GETODELAY, &used) == -1)
	    used = 0;
    }

    return used;
}

void OutputOSS::run()
{
    mutex()->lock();

    if (! m_inited)
    {
	mutex()->unlock();

	return;
    }

    m_play = TRUE;

    mutex()->unlock();

    fd_set afd;
    struct timeval tv;
    Buffer *b = 0;
    bool done = FALSE;
    unsigned long n = 0, m = 0, l = 0;

    dispatch(OutputState::Playing);

    FD_ZERO(&afd);

    while (! done) {
	mutex()->lock();

	recycler()->mutex()->lock();

	done = m_userStop;

	while (! done && (recycler()->empty() || m_pause)) {
	    post();

	    mutex()->unlock();

	    {
		stat = m_pause ? OutputState::Paused : OutputState::Buffering;
		OutputState e((OutputState::Type) stat);
		dispatch(e);
	    }

	    recycler()->cond()->wakeOne();
	    recycler()->cond()->wait(recycler()->mutex());

	    mutex()->lock();
	    done = m_userStop;
	    status();
	}

       	if (! b) {
	    b = recycler()->next();
	    if (b->rate)
	        m_rate = b->rate;
	}

	recycler()->cond()->wakeOne();
	recycler()->mutex()->unlock();

	FD_ZERO(&afd);
	FD_SET(m_audio_fd, &afd);
	// nice long poll timeout
	tv.tv_sec = 5l;
	tv.tv_usec = 0l;

	if (b &&
	    (! do_select || (select(m_audio_fd + 1, 0, &afd, 0, &tv) > 0 &&
			     FD_ISSET(m_audio_fd, &afd)))) {
	    l = qMin(int(2048), int(b->nbytes - n));
	    if (l > 0) {
		m = write(m_audio_fd, b->data + n, l);
		n += m;

		status();
		dispatchVisual(b, m_totalWritten, m_channels, m_precision);
	    } else {
		// force buffer change
		n = b->nbytes;
		m = 0;
	    }
	}

	m_totalWritten += m;

	if (n == b->nbytes) {
	    recycler()->mutex()->lock();
	    recycler()->done();
	    recycler()->mutex()->unlock();

	    b = 0;
	    n = 0;
	}

	mutex()->unlock();
    }

    mutex()->lock();

    if (! m_userStop)
	sync();
    resetDSP();

    m_play = FALSE;

    dispatch(OutputState::Stopped);
    mutex()->unlock();
}


void OutputOSS::setVolume(int l, int r)
{
    int v, devs;
    long cmd;

        ioctl(m_mixer_fd, SOUND_MIXER_READ_DEVMASK, &devs);
        if ((devs & SOUND_MASK_PCM) && (m_master == false))
            cmd = SOUND_MIXER_WRITE_PCM;
        else if ((devs & SOUND_MASK_VOLUME) && (m_master == true))
            cmd = SOUND_MIXER_WRITE_VOLUME;
        else
        {
            //close(mifd);
            return;
        }
        v = (r << 8) | l;
        ioctl(m_mixer_fd, cmd, &v);
}

void OutputOSS::volume(int *ll,int *rr)
{
    *ll = 0; 
    *rr = 0;
    int  cmd;
    int v, devs;

    ioctl(m_mixer_fd, SOUND_MIXER_READ_DEVMASK, &devs);
    if ((devs & SOUND_MASK_PCM) && (m_master == 0))
        cmd = SOUND_MIXER_READ_PCM;
    else if ((devs & SOUND_MASK_VOLUME) && (m_master == 1))
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


void OutputOSS::checkVolume()
{
    long ll = 0, lr = 0, cmd;
    int v, devs;

    /*
     * We dont show any errors if this fails, as this is called
     * rather often
     */
//    if (!open_mixer_device()) {
    ioctl(m_mixer_fd, SOUND_MIXER_READ_DEVMASK, &devs);
    if ((devs & SOUND_MASK_PCM) && (m_master == 0))
        cmd = SOUND_MIXER_READ_PCM;
    else if ((devs & SOUND_MASK_VOLUME) && (m_master == 1))
        cmd = SOUND_MIXER_READ_VOLUME;
    else
        return;

    ioctl(m_mixer_fd, cmd, &v);
    ll = (v & 0xFF00) >> 8;
    lr = (v & 0x00FF);

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
//    }
}


