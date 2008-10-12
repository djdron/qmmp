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
#include <QFile>

#include <qmmp/constants.h>
#include <qmmp/buffer.h>
#include <qmmp/output.h>
#include <qmmp/recycler.h>

#include "decoder_ffmpeg.h"

// Decoder class

DecoderFFmpeg::DecoderFFmpeg(QObject *parent, DecoderFactory *d, Output *o, const QString &path)
        : Decoder(parent, d, o)
{
    inited = FALSE;
    user_stop = FALSE;
    output_buf = 0;
    output_bytes = 0;
    output_at = 0;
    bks = 0;
    done = FALSE;
    m_finish = FALSE;
    freq = 0;
    bitrate = 0;
    seekTime = -1.0;
    totalTime = 0.0;
    chan = 0;
    output_size = 0;
    ic = 0;
    wma_outbuf = 0;
    m_path = path;
}


DecoderFFmpeg::~DecoderFFmpeg()
{
    deinit();
    if (wma_outbuf)
    {
        delete [] wma_outbuf;
        wma_outbuf = 0;
    }
    if (output_buf)
        delete [] output_buf;
    output_buf = 0;

    if (ic)
        av_close_input_file(ic);
}


void DecoderFFmpeg::stop()
{
    user_stop = TRUE;
}


void DecoderFFmpeg::flush(bool final)
{
    ulong min = final ? 0 : bks;

    while ((! done && ! m_finish) && output_bytes > min)
    {
        output()->recycler()->mutex()->lock ();

        while ((! done && ! m_finish) && output()->recycler()->full())
        {
            mutex()->unlock();

            output()->recycler()->cond()->wait(output()->recycler()->mutex());

            mutex()->lock ();
            done = user_stop;
        }

        if (user_stop || m_finish)
        {
            inited = FALSE;
            done = TRUE;
        }
        else
        {
            output_bytes -= produceSound(output_buf, output_bytes, bitrate, chan);
            output_size += bks;
            output_at = output_bytes;
        }

        if (output()->recycler()->full())
        {
            output()->recycler()->cond()->wakeOne();
        }

        output()->recycler()->mutex()->unlock();
    }
}


bool DecoderFFmpeg::initialize()
{
    bks = Buffer::size();
    inited = user_stop = done = m_finish = FALSE;
    freq = bitrate = 0;
    chan = 0;
    output_size = 0;
    seekTime = -1.0;
    totalTime = 0.0;

    if (!output_buf)
        output_buf = new char[globalBufferSize];
    output_at = 0;
    output_bytes = 0;

    avcodec_init();
    avcodec_register_all();
    av_register_all();

    AVCodec *codec;
    if (av_open_input_file(&ic, m_path.toLocal8Bit(), NULL,0, NULL) < 0)
    {
        qDebug("DecoderFFmpeg: cannot open input file");
        return FALSE;
    }
    for (wma_idx = 0; wma_idx < ic->nb_streams; wma_idx++)
    {
        c = ic->streams[wma_idx]->codec;
        if (c->codec_type == CODEC_TYPE_AUDIO) break;
    }

    av_find_stream_info(ic);

    codec = avcodec_find_decoder(c->codec_id);

    if (!codec) return FALSE;
    if (avcodec_open(c, codec) < 0)
        return FALSE;

    totalTime = ic->duration/AV_TIME_BASE;

    configure(c->sample_rate, c->channels, 16);

    bitrate = c->bit_rate;
    chan = c->channels;
    wma_outbuf = new uint8_t[AVCODEC_MAX_AUDIO_FRAME_SIZE*sizeof(int16_t)];
    inited = TRUE;
    qDebug("DecoderFFmpeg: initialize succes");
    return TRUE;
}


qint64 DecoderFFmpeg::lengthInSeconds()
{
    if (! inited)
        return 0;

    return totalTime;
}


void DecoderFFmpeg::seek(qint64 pos)
{
    seekTime = pos;
}


void DecoderFFmpeg::deinit()
{
    inited = user_stop = done = m_finish = FALSE;
    freq = bitrate = 0;
    chan = 0;
    output_size = 0;
}

void DecoderFFmpeg::run()
{
//     mpc_uint32_t vbrAcc = 0;
//     mpc_uint32_t vbrUpd = 0;
    uint8_t *inbuf_ptr;
    int out_size, size;
    AVPacket pkt;

    mutex()->lock ();

    if (! inited)
    {
        mutex()->unlock();

        return;
    }
    mutex()->unlock();

    while (!done && !m_finish)
    {
        mutex()->lock ();
        // decode

        if (seekTime >= 0.0)
        {
            int64_t timestamp;
            timestamp = int64_t(seekTime)*AV_TIME_BASE;
            if (ic->start_time != AV_NOPTS_VALUE)
                timestamp += ic->start_time;
            av_seek_frame(ic, -1, timestamp, AVSEEK_FLAG_BACKWARD);
            avcodec_flush_buffers(c);
            seekTime = -1.0;
        }

        int l = 0;
        if (av_read_frame(ic, &pkt) < 0)
        {
            m_finish = TRUE;
            goto end;
        }
        size = pkt.size;
        inbuf_ptr = pkt.data;

        out_size = 0;

        while (size > 0)
        {
            out_size = AVCODEC_MAX_AUDIO_FRAME_SIZE*sizeof(int16_t);
            l = avcodec_decode_audio2(c, (int16_t *)(wma_outbuf), &out_size, inbuf_ptr, size);

            if(l < 0)
                goto end;
            ffmpeg_out(out_size);
            size -= l;
            inbuf_ptr += l;
            if (pkt.data) 
                av_free_packet(&pkt);
        }
        bitrate = c->bit_rate/1024;
end:
        if (m_finish)
        {
            flush(TRUE);

            if (output())
            {
                output()->recycler()->mutex()->lock ();
                // end of stream
                while (! output()->recycler()->empty() && ! user_stop)
                {
                    output()->recycler()->cond()->wakeOne();
                    mutex()->unlock();
                    output()->recycler()->cond()->wait(output()->recycler()->mutex());
                    mutex()->lock ();
                }
                output()->recycler()->mutex()->unlock();
            }

            done = TRUE;
            if (! user_stop)
            {
                m_finish = TRUE;
            }
        }

        mutex()->unlock();

    }

    mutex()->lock ();
    if (m_finish)
        finish();
    mutex()->unlock();
    deinit();
}

void DecoderFFmpeg::ffmpeg_out(int size)
{
    if (size == 0)
        return;
    int at = 0;
    int to_copy = 0;
    while (size > 0 && !user_stop)
    {
        to_copy = qMin(int(globalBufferSize - output_at), int(size) );
        memmove ( (char *) (output_buf + output_at), wma_outbuf + at, to_copy);
        at += to_copy;
        size -= to_copy;
        output_at += to_copy;
        output_bytes += to_copy;
        if (output())
            flush();
    }
}
