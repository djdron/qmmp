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
#include <malloc.h>
#include <qmmp/buffer.h>
#include <qmmp/output.h>
#include <qmmp/recycler.h>

#include "decoder_ffmpeg.h"

// callbacks

static int ffmpeg_read(void *data, uint8_t *buf, int size)
{
    DecoderFFmpeg *d = (DecoderFFmpeg*)data;
    return (int)d->input()->read((char*)buf, size);
}

static int64_t ffmpeg_seek(void *data, int64_t offset, int whence)
{
    DecoderFFmpeg *d = (DecoderFFmpeg*)data;
    int64_t absolute_pos = 0;
    /*if(d->input()->isSequential())
        return -1;*/
    switch( whence )
    {
    case AVSEEK_SIZE:
        return d->input()->size();
    case SEEK_SET:
        absolute_pos = offset;
        break;
    case SEEK_CUR:
        absolute_pos = d->input()->pos() + offset;
        break;
    case SEEK_END:
        absolute_pos = d->input()->size() - offset;
    default:
        return -1;
    }
    if(absolute_pos < 0 || absolute_pos > d->input()->size())
        return -1;
    return d->input()->seek(absolute_pos);
}


// Decoder class

DecoderFFmpeg::DecoderFFmpeg(const QString &path, QIODevice *i)
        : Decoder(i)
{
    m_bitrate = 0;
    m_skip = FALSE;
    m_totalTime = 0;
    ic = 0;
    m_path = path;
    m_temp_pkt.size = 0;
    m_pkt.size = 0;
    m_pkt.data = 0;
    m_output_buf = 0;
    m_output_at = 0;
    m_skipBytes = 0;
    av_init_packet(&m_pkt);
    av_init_packet(&m_temp_pkt);
}


DecoderFFmpeg::~DecoderFFmpeg()
{
    m_bitrate = 0;
    m_temp_pkt.size = 0;
    if (ic)
        av_close_input_stream(ic);
   if(m_pkt.data)
        av_free_packet(&m_pkt);
    if(m_output_buf)
        free(m_output_buf);
}

bool DecoderFFmpeg::initialize()
{
    m_bitrate = 0;
    m_skip = FALSE;
    m_totalTime = 0;
    m_seekTime = -1;
    av_register_all();

    AVProbeData  pd;
    uint8_t buf[8192 + AVPROBE_PADDING_SIZE];
    pd.filename = m_path.toLocal8Bit().constData();
    pd.buf_size = input()->peek((char*)buf, sizeof(buf) - AVPROBE_PADDING_SIZE);
    pd.buf = buf;
    if(pd.buf_size < 8192)
        return FALSE;
    AVInputFormat *fmt = av_probe_input_format(&pd, 1);
    if(!fmt)
    {
        qWarning("DecoderFFmpeg: usupported format");
        return FALSE;
    }
    qDebug("DecoderFFmpeg: detected format: %s", fmt->long_name);

    init_put_byte(&m_stream, m_input_buf, INPUT_BUFFER_SIZE,
                   0, this, ffmpeg_read, NULL, ffmpeg_seek);

    m_stream.is_streamed = input()->isSequential();
    m_stream.max_packet_size = INPUT_BUFFER_SIZE;


    AVFormatParameters ap;
    memset(&ap, 0, sizeof(ap));

    if(av_open_input_stream(&ic, &m_stream, m_path.toLocal8Bit(),
                              fmt, &ap) != 0)
    {
        qDebug("DecoderFFmpeg: av_open_input_stream() failed");
        return FALSE;
    }

    AVCodec *codec;

    av_find_stream_info(ic);
    if(ic->pb)
        ic->pb->eof_reached = 0;

    ic->flags |= AVFMT_FLAG_GENPTS;
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
    codec = avcodec_find_decoder(c->codec_id);

    if (!codec)
    {
        qWarning("DecoderFFmpeg: unsupported codec for output stream");
        return FALSE;
    }

    if (avcodec_open(c, codec) < 0)
    {
        qWarning("DecoderFFmpeg: error while opening codec for output stream");
        return FALSE;
    }

    m_totalTime = input()->isSequential() ? 0 : ic->duration * 1000 / AV_TIME_BASE;
    m_output_buf = (uint8_t *)memalign(16, AVCODEC_MAX_AUDIO_FRAME_SIZE * 3 / 2 + QMMP_BUFFER_SIZE);

    configure(c->sample_rate, c->channels, Qmmp::PCM_S16LE);
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
    m_skipBytes = 0;
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
    memmove(m_output_buf, m_output_buf + len, m_output_at-m_skipBytes);
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

    return out_size;
}

void DecoderFFmpeg::seek(qint64 pos)
{    
   int64_t timestamp = int64_t(pos)*AV_TIME_BASE/1000;
    if (ic->start_time != (qint64)AV_NOPTS_VALUE)
        timestamp += ic->start_time;
    m_seekTime = timestamp;
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
#if (LIBAVCODEC_VERSION_INT >= ((51<<16)+(44<<8)+0))
            if(m_seekTime && c->codec_id == CODEC_ID_APE)
            {
                int64_t rescaledPts = av_rescale(m_pkt.pts,
                                                 AV_TIME_BASE * (int64_t)
                                                 ic->streams[m_pkt.stream_index]->time_base.num,
                                                 ic->streams[m_pkt.stream_index]->time_base.den);
                m_skipBytes =  (m_seekTime - rescaledPts) * c->sample_rate * 4 / AV_TIME_BASE;
            }
            else
                m_skipBytes = 0;

#endif
            m_seekTime = 0;
        }
#if (LIBAVCODEC_VERSION_INT >= ((51<<16)+(44<<8)+0))
        if(m_skipBytes > 0 && c->codec_id == CODEC_ID_APE)
        {
            while (m_skipBytes > 0)
            {
                m_output_at = ffmpeg_decode(m_output_buf);
                m_skipBytes -= m_output_at;
            }

            if(m_skipBytes < 0)
            {
                m_output_at = - m_skipBytes;
                m_output_at = m_output_at/4*4;
                memmove(m_output_buf, m_output_buf + m_output_at, m_output_at);
                m_skipBytes = 0;
            }
        }
        else
            m_output_at = ffmpeg_decode(m_output_buf);
#else
        m_output_at = ffmpeg_decode(m_output_buf);
#endif

        if(m_output_at < 0)
        {
            m_output_at = 0;
            m_temp_pkt.size = 0;
            continue;
        }
        else if(m_output_at == 0)
        {
            if(m_pkt.data)
                av_free_packet(&m_pkt);
            m_temp_pkt.size = 0;
        }
    }
}
