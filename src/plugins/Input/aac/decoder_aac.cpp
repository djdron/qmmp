/***************************************************************************
 *   Copyright (C) 2006-2009 by Ilya Kotov                                 *
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
#include <QtGlobal>

#include <qmmp/constants.h>
#include <qmmp/buffer.h>
#include <qmmp/output.h>
#include <qmmp/recycler.h>

#include "decoder_aac.h"
#include "aacfile.h"

#define AAC_BUFFER_SIZE 4096

// Decoder class

DecoderAAC::DecoderAAC(QObject *parent, DecoderFactory *d, QIODevice *i, Output *o)
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
    m_len = 0;
    m_freq = 0;
    m_bitrate = 0;
    m_seekTime = -1.0;
    m_totalTime = 0.0;
    m_chan = 0;
    m_output_size = 0;
    m_data = 0;
    m_input_buf = 0;
    m_input_at = 0;
}


DecoderAAC::~DecoderAAC()
{
    deinit();
    if (data())
    {
        if (data()->handle)
            NeAACDecClose (data()->handle);
        delete data();
        m_data = 0;
    }
    if (m_output_buf)
        delete [] m_output_buf;
    if (m_input_buf)
        delete [] m_input_buf;
    m_output_buf = 0;
    m_input_buf = 0;
}


void DecoderAAC::stop()
{
    m_user_stop = TRUE;
}


void DecoderAAC::flush(bool final)
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


bool DecoderAAC::initialize()
{
    m_bks = Buffer::size();
    m_inited = m_user_stop = m_done = m_finish = FALSE;
    m_len = m_freq = m_bitrate = 0;
    //chan = 0;
    m_output_size = 0;
    m_seekTime = -1.0;
    m_totalTime = 0.0;


    if (!input())
    {
        qWarning("DecoderAAC: cannot initialize.  No input.");
        return FALSE;
    }
    if (!m_input_buf)
        m_input_buf = new char[AAC_BUFFER_SIZE];

    if (!m_output_buf)
        m_output_buf = new char[globalBufferSize];
    m_output_at = 0;
    m_output_bytes = 0;
    m_input_at = 0;

    if (!input()->isOpen())
    {
        if (!input()->open(QIODevice::ReadOnly))
        {
            qWarning("DecoderAAC: %s", qPrintable(input()->errorString ()));
            return FALSE;
        }
    }
    AACFile aac_file(input());
    if (!aac_file.isValid())
    {
        qWarning("DecoderAAC: unsupported AAC file");
        return FALSE;
    }
    m_totalTime = aac_file.length();
    m_bitrate = aac_file.bitrate();

    if (!m_data)
        m_data = new aac_data;

    data()->handle = NeAACDecOpen();

    NeAACDecConfigurationPtr conf;
    conf = NeAACDecGetCurrentConfiguration(data()->handle);
    conf->downMatrix = 1;
    conf->defSampleRate = 44100;
    conf->dontUpSampleImplicitSBR = 0;
    conf->defObjectType = LC;
    conf->outputFormat = FAAD_FMT_16BIT;
    NeAACDecSetConfiguration(data()->handle, conf);

    m_input_at = input()->read((char *)m_input_buf, AAC_BUFFER_SIZE);

    //skip id3 tag
    int tag_size = 0;
    if (!memcmp(m_input_buf, "ID3", 3))
    {
        /* high bit is not used */
        tag_size = (m_input_buf[6] << 21) | (m_input_buf[7] << 14) |
                   (m_input_buf[8] <<  7) | (m_input_buf[9] <<  0);

        tag_size += 10;
        memmove (m_input_buf, m_input_buf + tag_size, m_input_at - tag_size);
        m_input_at -= tag_size;
        m_input_at += input()->read((char *)(m_input_buf + m_input_at), AAC_BUFFER_SIZE - m_input_at);
    }
    int res = NeAACDecInit (data()->handle, (unsigned char*) m_input_buf, m_input_at, &m_freq, &m_chan);

    if (res < 0)
    {
        qWarning("DecoderAAC: NeAACDecInit() failed");
        return FALSE;
    }
    if (!m_freq || !m_chan)
    {
        qWarning("DecoderAAC: invalid sound parameters");
        return FALSE;
    }

    memmove(m_input_buf, m_input_buf + res, m_input_at - res);
    m_input_at -= res;
    configure(m_freq, m_chan, 16);
    m_inited = TRUE;
    qDebug("DecoderAAC: initialize succes");
    return TRUE;
}

qint64 DecoderAAC::aac_decode(void *out)
{
    NeAACDecFrameInfo frame_info;
    qint64 size = 0, to_read, read;
    out = 0;
    bool eof = FALSE;

    while (size <= 0 && !eof)
    {
        if (m_input_at < AAC_BUFFER_SIZE)
        {
            to_read = AAC_BUFFER_SIZE - m_input_at;
            read = input()->read((char *) (m_input_buf + m_input_at),  to_read);
            eof = (read != to_read);
            m_input_at += read;
        }

        out = NeAACDecDecode(data()->handle, &frame_info, (uchar *)m_input_buf, m_input_at);
        memmove(m_input_buf, m_input_buf + frame_info.bytesconsumed, m_input_at - frame_info.bytesconsumed);
        m_input_at -= frame_info.bytesconsumed;

        if ((size = frame_info.samples * 2) > 0)
            memcpy((void *) (m_output_buf + m_output_at), out, size);
        if (frame_info.error > 0)
        {
            m_input_at = 0;
            qDebug("DecoderAAC: %s", NeAACDecGetErrorMessage(frame_info.error));
        }
    }
    return size;
}

qint64 DecoderAAC::lengthInSeconds()
{
    if (!m_inited)
        return 0;

    return m_totalTime;
}


void DecoderAAC::seek(qint64 pos)
{
    m_seekTime = pos;
}


void DecoderAAC::deinit()
{
    m_inited = m_user_stop = m_done = m_finish = FALSE;
    m_len = m_freq = m_bitrate = 0;
    m_chan = 0;
    m_output_size = 0;
}

void DecoderAAC::run()
{
    mutex()->lock ();
    if (!m_inited)
    {
        mutex()->unlock();
        return;
    }
    mutex()->unlock();

    while (!m_done && !m_finish)
    {
        mutex()->lock ();
        // decode

        if (m_seekTime >= 0 && m_totalTime)
        {
            input()->seek(m_seekTime * input()->size() / m_totalTime);
            NeAACDecPostSeekReset (data()->handle, 0);
            m_input_at = 0;
            m_seekTime = -1.0;
        }

        m_len = aac_decode(m_prebuf2);

        if (m_len > 0)
        {
            //qDebug("flush");
            //bitrate = vbrUpd * data()->info.sample_freq / 1152;
            m_output_at += m_len;
            m_output_bytes += m_len;

            if (output())
                flush();

        }
        else if (m_len == 0)
        {
            flush(TRUE);

            if (output())
            {
                output()->recycler()->mutex()->lock ();
                // end of stream
                while (!output()->recycler()->empty() && !m_user_stop)
                {
                    output()->recycler()->cond()->wakeOne();
                    mutex()->unlock();
                    output()->recycler()->cond()->wait(output()->recycler()->mutex());
                    mutex()->lock ();
                }
                output()->recycler()->mutex()->unlock();
            }

            m_done = TRUE;
            if (!m_user_stop)
            {
                m_finish = TRUE;
            }
        }
        else
        {
            // error in read
            qWarning("DecoderAAC: Error while decoding stream, file appears to be corrupted");
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
