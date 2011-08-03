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

extern "C"{
#include <pulse/error.h>
}
#include "outputpulseaudio.h"

OutputPulseAudio::OutputPulseAudio(QObject * parent)
        : Output(parent)
{
    m_connection = 0;
}

OutputPulseAudio::~OutputPulseAudio()
{
    uninitialize();
}

void OutputPulseAudio::configure(quint32 freq, int chan, Qmmp::AudioFormat format)
{
    pa_sample_spec ss;

    switch (format)
    {
    case Qmmp::PCM_S8:
        ss.format = PA_SAMPLE_U8;
        break;
    case Qmmp::PCM_S16LE:
        ss.format = PA_SAMPLE_S16LE;
        break;
    case Qmmp::PCM_S24LE:
        ss.format = PA_SAMPLE_S24_32LE;
        break;
    case Qmmp::PCM_S32LE:
        ss.format = PA_SAMPLE_S32LE;
        break;
    default:
        ss.format = PA_SAMPLE_S16LE;
    }

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
        return;
    }
    Output::configure(freq, chan, format);
}

bool OutputPulseAudio::initialize()
{
    return true;
}

qint64 OutputPulseAudio::latency()
{
    if (!m_connection)
        return 0;
    int error = 0;
    qint64 delay =  pa_simple_get_latency(m_connection, &error)/1000;
    if (error)
    {
        qWarning("OutputPulseAudio: %s", pa_strerror (error));
        delay = 0;
    }
    return delay;
}

qint64 OutputPulseAudio::writeAudio(unsigned char *data, qint64 maxSize)
{
    int error;
    if (!m_connection)
        return -1;
    int i = 0;
    if ((i = pa_simple_write(m_connection, data, maxSize, &error)) < 0)
    {
        qWarning("OutputPulseAudio: pa_simple_write() failed: %s", pa_strerror(error));
        return -1;
    }
    return maxSize;
}

void OutputPulseAudio::drain()
{
    int error;
    if (m_connection)
        pa_simple_drain(m_connection, &error);
}

void OutputPulseAudio::reset()
{
    int error;
    if (m_connection)
        pa_simple_flush(m_connection, &error);
}

void OutputPulseAudio::uninitialize()
{
    if (m_connection)
    {
        qDebug("OutputPulseAudio: closing connection");
        pa_simple_free(m_connection);
        m_connection = 0;
    }
}
