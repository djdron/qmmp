/***************************************************************************
 *   Copyright (C) 2008-2009 by Ilya Kotov                                 *
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
#include "QtDebug"

#include "decoder_wavpack.h"
#include "cueparser.h"

// Decoder class

DecoderWavPack::DecoderWavPack(QObject *parent, DecoderFactory *d, Output *o, const QString &path)
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
    m_context = 0;
    m_length = 0;
    m_offset = 0;
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
    m_bks = Buffer::size();
    m_inited = m_user_stop = m_done = m_finish = FALSE;
    m_freq = m_bitrate = 0;
    m_chan = 0;
    m_output_size = 0;
    m_seekTime = -1.0;
    m_totalTime = 0.0;

    if (! m_output_buf)
        m_output_buf = new char[globalBufferSize];
    m_output_at = 0;
    m_output_bytes = 0;

    char err [80];
    if (m_path.startsWith("wvpack://")) //embeded cue track
    {
        QString p = QUrl(m_path).path();
        p.replace(QString(QUrl::toPercentEncoding("#")), "#");
        p.replace(QString(QUrl::toPercentEncoding("%")), "%");
        m_context = WavpackOpenFileInput (p.toLocal8Bit(), err, OPEN_WVC | OPEN_TAGS, 0);
        int cue_len = WavpackGetTagItem (m_context, "cuesheet", NULL, 0);
        char *value;
        if (cue_len)
        {
            value = (char*)malloc (cue_len * 2 + 1);
            WavpackGetTagItem (m_context, "cuesheet", value, cue_len + 1);
            CUEParser parser(value, p);
            int track = m_path.section("#", -1).toInt();
            m_offset = parser.offset(track);
            m_length = parser.length(track);
            m_path = p;
            //send metadata
            QMap<Qmmp::MetaData, QString> metaData = parser.info(track)->metaData();
            StateHandler::instance()->dispatch(metaData);
        }
    }
    else
    {
        m_context = WavpackOpenFileInput (m_path.toLocal8Bit(), err, OPEN_WVC | OPEN_TAGS, 0);
    }

    if (!m_context)
    {
        qWarning("DecoderWavPack: error: %s", err);
        return FALSE;
    }

    m_chan = WavpackGetNumChannels(m_context);
    m_freq = WavpackGetSampleRate (m_context);
    m_bps = WavpackGetBitsPerSample (m_context);
    configure(m_freq, m_chan, m_bps);
    m_inited = TRUE;

    if (m_offset)
        m_seekTime = m_offset;
    if (m_length)
        m_totalTime = m_length;
    else
        m_totalTime = (qint64) WavpackGetNumSamples(m_context) * 1000 / m_freq;
    qDebug("DecoderWavPack: initialize succes");
    return TRUE;
}

qint64 DecoderWavPack::totalTime()
{
    if (!m_inited)
        return 0;

    return m_totalTime;
}


void DecoderWavPack::seek(qint64 pos)
{
    m_seekTime = pos + m_offset;
}

void DecoderWavPack::deinit()
{
    m_inited = m_user_stop = m_done = m_finish = FALSE;
    m_freq = m_bitrate = 0;
    m_chan = 0;
    m_output_size = 0;
    m_length = 0;
    m_offset = 0;
    if (m_context)
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
            WavpackSeekSample (m_context, m_seekTime * m_freq / 1000);
            m_seekTime = -1.0;
        }
        //stop if track ended
        if (WavpackGetSampleIndex(m_context)/m_freq * 1000 - m_offset >= m_totalTime)
        {
            m_finish = TRUE;
        }

        samples = (globalBufferSize-m_output_at)/m_chan/4;

        len = WavpackUnpackSamples (m_context, in, samples);
        for (ulong i = 0; i < len * m_chan; ++i)
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
            // error while reading
            qWarning("DecoderWavPack: Error while decoding stream, file appears to be corrupted");
            m_finish = TRUE;
        }
        mutex()->unlock();
    }

    mutex()->lock ();

    delete[] in;
    delete[] out;

    if (m_finish)
        finish();

    mutex()->unlock();
    deinit();
}
