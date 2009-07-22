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

DecoderFFmpeg::DecoderFFmpeg(QObject *parent, DecoderFactory *d, Output *o, const QString &path)
        : Decoder(parent, d, o)
{
    m_bitrate = 0;
    seekTime = -1;
    m_totalTime = 0;
    ic = 0;
    m_path = path;
    m_size = 0;
}


DecoderFFmpeg::~DecoderFFmpeg()
{
    m_bitrate = 0;
    if (ic)
        av_close_input_file(ic);
    if(m_pkt.data)
        av_free_packet(&m_pkt);
}

bool DecoderFFmpeg::initialize()
{
    m_bitrate = 0;
    seekTime = -1;
    m_totalTime = 0.0;

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

qint64 DecoderFFmpeg::readAudio(char *audio, qint64 maxSize)
{
    if(seekTime == 1)
    {
        if(!m_size)
            fillBuffer();
        if(!m_size)
            return 0;
        while(m_size > 0)
            ffmpeg_decode(audio, maxSize);
        seekTime = -1;
    }

    if(!m_size)
        fillBuffer();
    if(!m_size)
        return 0;
    return ffmpeg_decode(audio, maxSize);
}

qint64 DecoderFFmpeg::ffmpeg_decode(char *audio, qint64 maxSize)
{
    int out_size = maxSize;
    if((m_pkt.stream_index == wma_idx))
    {
        int l = avcodec_decode_audio2(c, (int16_t *)(audio), &out_size, m_inbuf_ptr, m_size);
        if(l < 0)
            return 0;
        m_inbuf_ptr += l;
        m_size -= l;

    }
    if (!m_size && m_pkt.data)
        av_free_packet(&m_pkt);

    return out_size;
}

void DecoderFFmpeg::seekAudio(qint64 pos)
{
    int64_t timestamp = int64_t(pos)*AV_TIME_BASE/1000;
    if (ic->start_time != AV_NOPTS_VALUE)
        timestamp += ic->start_time;
    av_seek_frame(ic, -1, timestamp, AVSEEK_FLAG_BACKWARD);
    avcodec_flush_buffers(c);
    seekTime = 1;
}

void DecoderFFmpeg::fillBuffer()
{
    if (av_read_frame(ic, &m_pkt) < 0)
    {
        m_size = 0;
        m_inbuf_ptr = 0;
    }
    m_size = m_pkt.size;
    m_inbuf_ptr = m_pkt.data;
}
