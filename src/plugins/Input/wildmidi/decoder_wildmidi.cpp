/***************************************************************************
 *   Copyright (C) 2008 by Ilya Kotov                                      *
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
#include <QIODevice>
#include <QFile>
#include <math.h>
#include <stdint.h>

#include <qmmp/constants.h>
#include <qmmp/buffer.h>
#include <qmmp/output.h>
#include <qmmp/recycler.h>

#include "decoder_wildmidi.h"

// Decoder class

DecoderWildMidi::DecoderWildMidi(QObject *parent, DecoderFactory *d, Output *o, const QString &path)
        : Decoder(parent, d, o)
{
    m_path = path;
    m_inited = FALSE;
    m_user_stop = FALSE;
    m_output_buf = 0;
    m_output_bytes = 0;
    m_output_at = 0;
    m_bks = 0;
    m_done = FALSE;
    m_finish = FALSE;
    m_freq = 0;
    m_bitrate = 0;
    m_seekTime = -1.0;
    m_totalTime = 0.0;
    m_chan = 0;
    m_output_size = 0;
    //m_context = 0;
    midi_ptr =  0;
}

DecoderWildMidi::~DecoderWildMidi()
{
    deinit();
    if (m_output_buf)
        delete [] m_output_buf;
    m_output_buf = 0;
}

void DecoderWildMidi::stop()
{
    m_user_stop = TRUE;
}

void DecoderWildMidi::flush(bool final)
{
    ulong min = final ? 0 : m_bks;

    while ((! m_done && ! m_finish) && m_output_bytes > min)
    {
        output()->recycler()->mutex()->lock ();

        while ((! m_done && ! m_finish) && output()->recycler()->full())
        {
            mutex()->unlock();

            output()->recycler()->cond()->wait(output()->recycler()->mutex());

            mutex()->lock ();
            m_done = m_user_stop;
        }

        if (m_user_stop || m_finish)
        {
            m_inited = FALSE;
            m_done = TRUE;
        }
        else
        {
            m_output_bytes -= produceSound(m_output_buf, m_output_bytes, m_bitrate, m_chan);
            m_output_size += m_bks;
            m_output_at = m_output_bytes;
        }

        if (output()->recycler()->full())
        {
            output()->recycler()->cond()->wakeOne();
        }

        output()->recycler()->mutex()->unlock();
    }
}

bool DecoderWildMidi::initialize()
{
    m_bks = Buffer::size();
    m_inited = m_user_stop = m_done = m_finish = FALSE;
    m_freq = m_bitrate = 0;
    m_chan = 0;
    m_output_size = 0;
    m_seekTime = -1.0;
    m_totalTime = 0.0;
    _WM_Info *wm_info = 0; //TODO fix memory leak

    if (! m_output_buf)
        m_output_buf = new char[globalBufferSize];
    m_output_at = 0;
    m_output_bytes = 0;

    wm_info = new _WM_Info;

    unsigned long int mixer_options = 0;

    if (WildMidi_Init ("/etc/timidity/timidity.cfg", 44100, mixer_options) == -1)
        return FALSE;

    midi_ptr = WildMidi_Open (m_path.toLocal8Bit());

    wm_info = WildMidi_GetInfo(midi_ptr);

    m_totalTime = wm_info->approx_total_samples / 44100;

    configure(44100, 2, 16);

    m_inited = TRUE;
    qDebug("DecoderWildMidi: initialize succes");
    return TRUE;
}

qint64 DecoderWildMidi::lengthInSeconds()
{
    if (!m_inited)
        return 0;

    return m_totalTime;
}


void DecoderWildMidi::seek(qint64 pos)
{
    m_seekTime = pos;
}

void DecoderWildMidi::deinit()
{
    m_inited = m_user_stop = m_done = m_finish = FALSE;
    m_freq = m_bitrate = 0;
    m_chan = 0;
    m_output_size = 0;
    WildMidi_Shutdown();
}

void DecoderWildMidi::run()
{
    mutex()->lock ();

    ulong len = 0;

    _WM_Info *wm_info = new _WM_Info;

    if (!m_inited)
    {
        mutex()->unlock();
        return;
    }
    mutex()->unlock();

    while (! m_done && ! m_finish)
    {
        mutex()->lock ();

        //seeking

        if (m_seekTime >= 0.0)
        {
            // WavpackSeekSample (m_context, m_seekTime * m_freq);
            qint64 i = m_seekTime *44100;
            long unsigned int *sample_pos = (long unsigned int *) &i;
            WildMidi_FastSeek(midi_ptr, sample_pos);
            m_seekTime = -1.0;
        }

        wm_info = WildMidi_GetInfo(midi_ptr);

        if (wm_info->approx_total_samples > wm_info->current_sample)
            len = WildMidi_GetOutput (midi_ptr, m_output_buf, globalBufferSize - m_output_at);
        else
            len = 0;

        if (len > 0)
        {
            m_bitrate = 0; //TODO calculate bitrate using file size and length
            m_output_at += len;
            m_output_bytes += len;

            if (output())
                flush();

        }
        else if (len == 0)
        {
            flush(TRUE);

            if (output())
            {
                output()->recycler()->mutex()->lock ();
                // end of stream
                while (! output()->recycler()->empty() && ! m_user_stop)
                {
                    output()->recycler()->cond()->wakeOne();
                    mutex()->unlock();
                    output()->recycler()->cond()->wait(output()->recycler()->mutex());
                    mutex()->lock ();
                }
                output()->recycler()->mutex()->unlock();
            }

            m_done = TRUE;
            if (! m_user_stop)
            {
                m_finish = TRUE;
            }
        }
        else
        {
            // error while reading
            qWarning("DecoderWildMidi: Error while decoding stream, file appears to be corrupted");
            m_finish = TRUE;
        }
        mutex()->unlock();
    }

    mutex()->lock ();

    if (m_finish)
        finish();

    mutex()->unlock();
    deinit();
}
