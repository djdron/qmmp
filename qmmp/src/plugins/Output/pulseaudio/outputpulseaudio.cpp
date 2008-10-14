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

extern "C" {
#include <pulse/error.h>
}

#include <stdio.h>
#include <string.h>
#include <iostream>

#include <qmmp/constants.h>
#include <qmmp/buffer.h>
#include <qmmp/visual.h>
#include "outputpulseaudio.h"

OutputPulseAudio::OutputPulseAudio(QObject * parent)
        : Output(parent), m_inited(FALSE), m_pause(FALSE), m_play(FALSE),
        m_userStop(FALSE), m_totalWritten(0), m_currentSeconds(-1),
        m_bps(-1), m_frequency(-1), m_channels(-1), m_precision(-1)
{
    m_connection = 0;
}

OutputPulseAudio::~OutputPulseAudio()
{
    uninitialize();
}

void OutputPulseAudio::stop()
{
    m_userStop = TRUE;
}

void OutputPulseAudio::status()
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

qint64 OutputPulseAudio::written()
{
    return m_totalWritten;
}

void OutputPulseAudio::seek(qint64 pos)
{
    m_totalWritten = (pos * m_bps);
    m_currentSeconds = -1;
}

void OutputPulseAudio::configure(quint32 freq, int chan, int prec)
{
    m_frequency = freq;
    m_channels = chan;
    m_precision = prec;
    m_bps = freq * chan * (prec / 8);

    pa_sample_spec ss;
    ss.format = PA_SAMPLE_S16LE;
    ss.channels = chan;
    ss.rate = freq;
    int error;
    m_connection = pa_simple_new(NULL, // Use the default server.
                                 "Qmmp",             // Our application's name.
                                 PA_STREAM_PLAYBACK,
                                 NULL,               // Use the default device.
                                 "Music",            // Description of our stream.
                                 &ss,                // Our sample format.
                                 NULL,               // Use default channel map
                                 NULL,               // Use default buffering attributes.
                                 &error              // Error code.
                                );
    if (!m_connection)
    {
        qWarning("OutputPulseAudio: pa_simple_new() failed: %s", pa_strerror(error));
        m_inited = FALSE;
        return;
    }
    qDebug("OutputPulseAudio: frequency=%d, channels=%d",  uint(freq), chan);
}

void OutputPulseAudio::pause()
{
    if (!m_play)
        return;
    m_pause = (m_pause) ? FALSE : TRUE;
    Qmmp::State state = m_pause ? Qmmp::Paused: Qmmp::Playing;
    dispatch(state);
}

bool OutputPulseAudio::initialize()
{
    m_inited = m_pause = m_play = m_userStop = FALSE;
    m_currentSeconds = -1;
    m_inited = TRUE;
    return TRUE;
}


qint64 OutputPulseAudio::latency()
{
    long used = 0;

    return used;
}

void OutputPulseAudio::run()
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
    int error;

    dispatch(Qmmp::Playing);

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
            if(pa_simple_write(m_connection, b->data, b->nbytes, &error) < 0)
            {
                mutex()->unlock();
                qWarning("OutputPulseAudio: pa_simple_write() failed: %s", pa_strerror(error));
                break;
            }

            dispatchVisual(b, m_totalWritten, m_channels, m_precision);
            status();
            m_totalWritten += b->nbytes;
            mutex()->unlock();
        }
        // force buffer change
        recycler()->mutex()->lock ();
        recycler()->done();
        recycler()->mutex()->unlock();
        b = 0;
    }

    mutex()->lock ();
    m_play = FALSE;
    dispatch(Qmmp::Stopped);
    mutex()->unlock();

}

void OutputPulseAudio::uninitialize()
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
    if (!m_connection)
    {
        qDebug("OutputPulseAudio: closing connection");
        pa_simple_free(m_connection);
        m_connection = 0;
    }
}
