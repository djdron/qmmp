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
#include <QFile>

#include <qmmp/buffer.h>
#include <qmmp/output.h>
#include <qmmp/recycler.h>

#include "decoder_ffmpeg.h"

// Decoder class

DecoderFFmpeg::DecoderFFmpeg(const QString &path)
        : Decoder()
{
    m_bitrate = 0;
    m_skip = FALSE;
    m_totalTime = 0;
    ic = 0;
    m_path = path;
    m_temp_pkt.size = 0;
    m_pkt.size = 0;
    m_output_buf = 0;
    m_output_at = 0;
}


DecoderFFmpeg::~DecoderFFmpeg()
{
    m_bitrate = 0;
    m_temp_pkt.size = 0;
    if (ic)
        av_close_input_file(ic);
    if(m_pkt.data)
        av_free_packet(&m_pkt);
    if(m_output_buf)
        delete [] m_output_buf;
}

bool DecoderFFmpeg::initialize()
{
    m_bitrate = 0;
    m_skip = FALSE;
    m_totalTime = 0;

    avcodec_init();
    avcodec_register_all();
    av_register_all();

    AVCodec *codec;
    if (av_open_input_file(&ic, m_path.toLocal8Bit(), NULL,0, NULL) < 0)
    {
        qDebug("DecoderFFmpeg: cannot open input file");
        return FALSE;
    }

    av_find_stream_info(ic);
    av_read_play(ic);

    for (wma_idx = 0; wma_idx < (int)ic->nb_streams; wma_idx++)
    {
        c = ic->streams[wma_idx]->codec;
        if (c->codec_type == CODEC_TYPE_AUDIO) break;
    }

    if (c->channels > 0)
        c->channels = qMin(2, c->channels);
    else
        c->channels = 2;

    dump_format(ic,0,0,0);
    //dump_stream_info(ic);

    codec = avcodec_find_decoder(c->codec_id);

    if (!codec) return FALSE;
    if (avcodec_open(c, codec) < 0)
        return FALSE;

    m_totalTime = ic->duration * 1000 / AV_TIME_BASE;
    m_output_buf = new uint8_t[AVCODEC_MAX_AUDIO_FRAME_SIZE*sizeof(int16_t) + Qmmp::globalBufferSize()];
    configure(c->sample_rate, c->channels, 16);
    m_bitrate = c->bit_rate;
    qDebug("DecoderFFmpeg: initialize succes");
    return TRUE;
}


qint64 DecoderFFmpeg::totalTime()
{
    return m_totalTime;
}


int DecoderFFmpeg::bitrate()
{
    return m_bitrate;
}

qint64 DecoderFFmpeg::read(char *audio, qint64 maxSize)
{
    if (m_skip)
    {
        while(m_temp_pkt.size)
            ffmpeg_decode(m_output_buf);
        m_output_at = 0;
        m_skip = FALSE;
    }
    if(!m_output_at)
        fillBuffer();
    if(!m_output_at)
        return 0;
    qint64 len = qMin(m_output_at, maxSize);
    memcpy(audio, m_output_buf, len);
    m_output_at -= len;
    memmove(m_output_buf, m_output_buf + len, m_output_at);
    return len;
}

qint64 DecoderFFmpeg::ffmpeg_decode(uint8_t *audio)
{
    int out_size = AVCODEC_MAX_AUDIO_FRAME_SIZE * sizeof(int16_t);
    if((m_pkt.stream_index == wma_idx))
    {
#if (LIBAVCODEC_VERSION_INT >= ((52<<16)+(23<<8)+0))
        int l = avcodec_decode_audio3(c, (int16_t *)(audio), &out_size, &m_temp_pkt);
#else
        int l = avcodec_decode_audio2(c, (int16_t *)(audio), &out_size, m_temp_pkt.data, m_temp_pkt.size);
#endif
        m_bitrate = c->bit_rate/1024;
        if(l < 0)
            return l;
        m_temp_pkt.data += l;
        m_temp_pkt.size -= l;
    }
    if (!m_temp_pkt.size && m_pkt.data)
        av_free_packet(&m_pkt);

    if(!out_size)
        out_size = 4;
    return out_size;
}

void DecoderFFmpeg::seek(qint64 pos)
{
    int64_t timestamp = int64_t(pos)*AV_TIME_BASE/1000;
    if (ic->start_time != (qint64)AV_NOPTS_VALUE)
        timestamp += ic->start_time;
    av_seek_frame(ic, -1, timestamp, AVSEEK_FLAG_BACKWARD);
    avcodec_flush_buffers(c);
    if(m_pkt.size)
        m_skip = TRUE;
}

void DecoderFFmpeg::fillBuffer()
{
    while(!m_output_at)
    {
        if(!m_temp_pkt.size)
        {
            if (av_read_frame(ic, &m_pkt) < 0)
            {
                m_temp_pkt.size = 0;
                break;
            }
            m_temp_pkt.size = m_pkt.size;
            m_temp_pkt.data = m_pkt.data;
            if(m_pkt.stream_index != wma_idx)
            {
                if(m_pkt.data)
                    av_free_packet(&m_pkt);
                m_temp_pkt.size = 0;
                continue;
            }
        }
        m_output_at = ffmpeg_decode(m_output_buf);
        if(m_output_at < 0)
        {
            m_output_at = 0;
            break;
        }
        else if(m_output_at == 0)
        {
            if(m_pkt.data)
                av_free_packet(&m_pkt);
            m_temp_pkt.size = 0;
        }
    }
}
