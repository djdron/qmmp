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

#include "decoder_wavpack.h"

// Decoder class

DecoderWavPack::DecoderWavPack(QObject *parent, DecoderFactory *d, QIODevice *i, Output *o)
        : Decoder(parent, d, i, o)
{
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
    m_context = 0;
}

DecoderWavPack::~DecoderWavPack()
{
    deinit();
    if (m_output_buf)
        delete [] m_output_buf;
    m_output_buf = 0;
}

void DecoderWavPack::stop()
{
    m_user_stop = TRUE;
}

void DecoderWavPack::flush(bool final)
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

bool DecoderWavPack::initialize()
{
    m_bks = blockSize();
    m_inited = m_user_stop = m_done = m_finish = FALSE;
    m_freq = m_bitrate = 0;
    m_chan = 0;
    m_output_size = 0;
    m_seekTime = -1.0;
    m_totalTime = 0.0;


    if (! input())
    {
        error("DecoderWavPack: cannot initialize.  No input.");

        return FALSE;
    }

    if (! m_output_buf)
        m_output_buf = new char[globalBufferSize];
    m_output_at = 0;
    m_output_bytes = 0;

    QString filename = qobject_cast<QFile*>(input())->fileName ();
    input()->close();

    char err [80];
    m_context = WavpackOpenFileInput (filename.toLocal8Bit(), err,
                                      OPEN_WVC | OPEN_TAGS, 0);
    if(!m_context)
    {
        error(QString("DecoderWavPack: error: %1").arg(err));
        return FALSE;
    }
    m_chan = WavpackGetNumChannels(m_context);
    m_freq = WavpackGetSampleRate (m_context);
    m_bps =  WavpackGetBitsPerSample (m_context);
    configure(m_freq, m_chan, m_bps, int(WavpackGetAverageBitrate(m_context, m_chan)/1000));
    m_totalTime = (int) WavpackGetNumSamples(m_context)/m_freq;
    m_inited = TRUE;
    qDebug("DecoderWavPack: initialize succes");
    return TRUE;
}

double DecoderWavPack::lengthInSeconds()
{
    if (! m_inited)
        return 0;

    return m_totalTime;
}


void DecoderWavPack::seek(double pos)
{
    m_seekTime = pos;
}

void DecoderWavPack::deinit()
{
    m_inited = m_user_stop = m_done = m_finish = FALSE;
    m_freq = m_bitrate = 0;
    m_chan = 0;
    m_output_size = 0;
    if(m_context)
    {
        WavpackCloseFile (m_context);
        m_context = 0;
    }
}

void DecoderWavPack::run()
{
    mutex()->lock ();

    ulong len = 0;
    int32_t *in = new int32_t[globalBufferSize * m_chan / m_chan / 4];
    int16_t *out = new int16_t[globalBufferSize * m_chan / m_chan / 4];
    uint32_t samples = 0;

    if (! m_inited)
    {
        mutex()->unlock();

        return;
    }
    mutex()->unlock();
    dispatch(DecoderState::Decoding);

    while (! m_done && ! m_finish)
    {
        mutex()->lock ();

        //seeking

        if (m_seekTime >= 0.0)
        {
            WavpackSeekSample (m_context, m_seekTime * m_freq);
            m_seekTime = -1.0;
        }

        // decode
        samples = (globalBufferSize-m_output_at)/m_chan/4;

        len = WavpackUnpackSamples (m_context, in, samples);
        for(ulong i = 0; i < len * m_chan; ++i)
            out[i] = in[i];

        len *= (m_chan * 2); //convert to number of bytes
        memcpy(m_output_buf + m_output_at, (char *) out, len);

        if (len > 0)
        {
            m_bitrate =int( WavpackGetInstantBitrate(m_context)/1000);
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
            // error in read
            error("DecoderWavPack: Error while decoding stream, File appears to be "
                  "corrupted");
            m_finish = TRUE;
        }
        mutex()->unlock();
    }

    mutex()->lock ();

    delete[] in;
    delete[] out;

    if (m_finish)
        dispatch(DecoderState::Finished);
    else if (m_user_stop)
        dispatch(DecoderState::Stopped);

    mutex()->unlock();

    deinit();
}
