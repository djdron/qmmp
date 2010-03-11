// Copyright (c) 2000-2001 Brad Hughes <bhughes@trolltech.com>
//
// Use, modification and distribution is allowed without limitation,
// warranty, or liability of any kind.
//


#include <QtGui>
#include <taglib/id3v2header.h>
#include <taglib/tbytevector.h>
#include <qmmp/buffer.h>
#include <qmmp/output.h>
#include <math.h>
#include <stdio.h>
#include "tagextractor.h"
#include "decoder_mad.h"

#define XING_MAGIC (('X' << 24) | ('i' << 16) | ('n' << 8) | 'g')
#define INPUT_BUFFER_SIZE (32*1024)

DecoderMAD::DecoderMAD(QIODevice *i) : Decoder(i)
{
    m_inited = false;
    m_totalTime = 0;
    m_channels = 0;
    m_bitrate = 0;
    m_freq = 0;
    m_len = 0;
    m_input_buf = 0;
    m_input_bytes = 0;
    m_output_bytes = 0;
    m_output_at = 0;
    m_skip_frames = 0;
    m_eof = false;
}

DecoderMAD::~DecoderMAD()
{
    deinit();
    if (m_input_buf)
    {
        qDebug("DecoderMAD: deleting input_buf");
        delete [] m_input_buf;
        m_input_buf = 0;
    }    
}

bool DecoderMAD::initialize()
{
    m_inited = false;
    m_totalTime = 0;
    m_channels = 0;
    m_bitrate = 0;
    m_freq = 0;
    m_len = 0;
    m_input_bytes = 0;
    m_output_bytes = 0;
    m_output_at = 0;

    if (!input())
    {
        qWarning("DecoderMAD: cannot initialize.  No input.");
        return false;
    }

    if (!m_input_buf)
        m_input_buf = new char[INPUT_BUFFER_SIZE];

    if (!input()->isOpen())
    {
        if (!input()->open(QIODevice::ReadOnly))
        {
            qWarning("DecoderMAD: %s", qPrintable(input()->errorString ()));
            return false;
        }
    }

    if (input()->isSequential ()) //for streams only
    {
        TagExtractor extractor(input());
        if(!extractor.id3v2tag().isEmpty())
            StateHandler::instance()->dispatch(extractor.id3v2tag());
    }

    mad_stream_init(&stream);
    mad_frame_init(&frame);
    mad_synth_init(&synth);

    if (!findHeader())
    {
        qDebug("DecoderMAD: Can't find a valid MPEG header.");
        return false;
    }
    mad_stream_buffer(&stream, (unsigned char *) m_input_buf, m_input_bytes);
    stream.error = MAD_ERROR_BUFLEN;
    mad_frame_mute (&frame);
    stream.next_frame = 0;
    stream.sync = 0;
    configure(m_freq, m_channels, Qmmp::PCM_S16LE);
    m_inited = true;
    return true;
}


void DecoderMAD::deinit()
{
    if (!m_inited)
        return;

    mad_synth_finish(&synth);
    mad_frame_finish(&frame);
    mad_stream_finish(&stream);

    m_inited = false;
    m_totalTime = 0;
    m_channels = 0;
    m_bitrate = 0;
    m_freq = 0;
    m_len = 0;
    m_input_bytes = 0;
    m_output_bytes = 0;
    m_output_at = 0;
    m_skip_frames = 0;
    m_eof = false;
}

bool DecoderMAD::findXingHeader(struct mad_bitptr ptr, unsigned int bitlen)
{
    if (bitlen < 64 || mad_bit_read(&ptr, 32) != XING_MAGIC)
        goto fail;

    xing.flags = mad_bit_read(&ptr, 32);
    bitlen -= 64;

    if (xing.flags & XING_FRAMES)
    {
        if (bitlen < 32)
            goto fail;

        xing.frames = mad_bit_read(&ptr, 32);
        bitlen -= 32;
    }

    if (xing.flags & XING_BYTES)
    {
        if (bitlen < 32)
            goto fail;

        xing.bytes = mad_bit_read(&ptr, 32);
        bitlen -= 32;
    }

    if (xing.flags & XING_TOC)
    {
        int i;

        if (bitlen < 800)
            goto fail;

        for (i = 0; i < 100; ++i)
            xing.toc[i] = mad_bit_read(&ptr, 8);

        bitlen -= 800;
    }

    if (xing.flags & XING_SCALE)
    {
        if (bitlen < 32)
            goto fail;

        xing.scale = mad_bit_read(&ptr, 32);
        bitlen -= 32;
    }

    return true;

fail:
    xing.flags = 0;
    xing.frames = 0;
    xing.bytes = 0;
    xing.scale = 0;
    return false;
}

bool DecoderMAD::findHeader()
{
    bool result = false;
    int count = 0;
    bool has_xing = false;
    bool is_vbr = false;
    mad_timer_t duration = mad_timer_zero;
    struct mad_header header;
    mad_header_init (&header);

    forever
    {
        m_input_bytes = 0;
        if (stream.error == MAD_ERROR_BUFLEN || !stream.buffer)
        {
            size_t remaining = 0;

            if (!stream.next_frame)
            {
                remaining = stream.bufend - stream.next_frame;
                memmove (m_input_buf, stream.next_frame, remaining);
            }

            m_input_bytes = input()->read(m_input_buf + remaining, INPUT_BUFFER_SIZE - remaining);

            if (m_input_bytes <= 0)
                break;

            mad_stream_buffer(&stream, (unsigned char *) m_input_buf + remaining, m_input_bytes);
            stream.error = MAD_ERROR_NONE;
        }

        if (mad_header_decode(&header, &stream) < 0)
        {
            if(stream.error == MAD_ERROR_LOSTSYNC)
            {
                uint tagSize = findID3v2((uchar *)stream.this_frame,
                                         (ulong) (stream.bufend - stream.this_frame));
                if (tagSize > 0)
                    mad_stream_skip(&stream, tagSize);
                continue;
            }
            else if (stream.error == MAD_ERROR_BUFLEN || MAD_RECOVERABLE(stream.error))
                continue;
            else
            {
                qDebug ("DecoderMAD: Can't decode header: %s", mad_stream_errorstr(&stream));
                break;
            }
        }
        result = true;

        if (input()->isSequential())
            break;

        count ++;
        //try to detect xing header
        if (count == 1)
        {
            frame.header = header;
            if (mad_frame_decode(&frame, &stream) != -1 &&
                    findXingHeader(stream.anc_ptr, stream.anc_bitlen))
            {
                is_vbr = true;

                qDebug ("DecoderMAD: Xing header detected");

                if (xing.flags & XING_FRAMES)
                {
                    has_xing = true;
                    count = xing.frames;
                    break;
                }
            }
        }
        //try to detect VBR
        if (!is_vbr && !(count > 15))
        {
            if (m_bitrate && header.bitrate != m_bitrate)
            {
                qDebug ("DecoderMAD: VBR detected");
                is_vbr = true;
            }
            else
                m_bitrate = header.bitrate;
        }
        else if (!is_vbr)
        {
            qDebug ("DecoderMAD: Fixed rate detected");
            break;
        }
        mad_timer_add (&duration, header.duration);
    }

    if (!result)
        return false;

    if (!is_vbr && !input()->isSequential())
    {
        double time = (input()->size() * 8.0) / (header.bitrate);
        double timefrac = (double)time - ((long)(time));
        mad_timer_set(&duration, (long)time, (long)(timefrac*100), 100);
    }
    else if (has_xing)
    {
        mad_timer_multiply (&header.duration, count);
        duration = header.duration;
    }

    m_totalTime = mad_timer_count(duration, MAD_UNITS_MILLISECONDS);
    qDebug ("DecoderMAD: Total time: %ld", long(m_totalTime));
    m_freq = header.samplerate;
    m_channels = MAD_NCHANNELS(&header);
    m_bitrate = header.bitrate / 1000;
    mad_header_finish(&header);
    input()->seek(0);
    m_input_bytes = 0;
    return true;
}

qint64 DecoderMAD::totalTime()
{
    if (!m_inited)
        return 0;
    return m_totalTime;
}

int DecoderMAD::bitrate()
{
    return int(m_bitrate);
}

qint64 DecoderMAD::read(char *data, qint64 size)
{
    forever
    {
        if(((stream.error == MAD_ERROR_BUFLEN) || !stream.buffer) && !m_eof)
        {
            m_eof = !fillBuffer();
        }
        if(mad_frame_decode(&frame, &stream) < 0)
        {
            switch((int) stream.error)
            {
                case MAD_ERROR_LOSTSYNC:
                {
                    //skip ID3v2 tag
                    uint tagSize = findID3v2((uchar *)stream.this_frame,
                                             (ulong) (stream.bufend - stream.this_frame));
                    if (tagSize > 0)
                    {
                        mad_stream_skip(&stream, tagSize);
                        qDebug("DecoderMAD: %d bytes skipped", tagSize);
                    }
                    continue;
                }
                case MAD_ERROR_BUFLEN:
                    if(m_eof)
                        return 0;
                    continue;
                default:
                    if (!MAD_RECOVERABLE(stream.error))
                        return 0;
                    else
                        continue;
            }
        }
        if(m_skip_frames)
        {
            m_skip_frames--;
            continue;
        }
        mad_synth_frame(&synth, &frame);
        return madOutput(data, size);
    }
}
void DecoderMAD::seek(qint64 pos)
{
    if(m_totalTime > 0)
    {
        qint64 seek_pos = qint64(pos * input()->size() / m_totalTime);
        input()->seek(seek_pos);
        mad_frame_mute(&frame);
        mad_synth_mute(&synth);
        stream.error = MAD_ERROR_BUFLEN;
        stream.sync = 0;
        m_input_bytes = 0;
        stream.next_frame = 0;
        m_skip_frames = 2;
    }
}

bool DecoderMAD::fillBuffer()
{
    if (stream.next_frame)
    {
        m_input_bytes = &m_input_buf[m_input_bytes] - (char *) stream.next_frame;
        memmove(m_input_buf, stream.next_frame, m_input_bytes);
    }
    int len = input()->read((char *) m_input_buf + m_input_bytes, INPUT_BUFFER_SIZE - m_input_bytes);
    if (!len)
    {
        qDebug("DecoderMAD: end of file");
        return false;
    }
    else if(len < 0)
    {
        qWarning("DecoderMAD: error");
        return false;
    }
    m_input_bytes += len;
    mad_stream_buffer(&stream, (unsigned char *) m_input_buf, m_input_bytes);
    return true;
}

uint DecoderMAD::findID3v2(uchar *data, ulong size) //retuns ID3v2 tag size
{
    if (size < 10)
        return 0;

    if (((data[0] == 'I' && data[1] == 'D' && data[2] == '3') || //ID3v2 tag
            (data[0] == '3' && data[1] == 'D' && data[2] == 'I')) && //ID3v2 footer
            data[3] < 0xff && data[4] < 0xff && data[6] < 0x80 &&
            data[7] < 0x80 && data[8] < 0x80 && data[9] < 0x80)
    {
        TagLib::ByteVector byteVector((char *)data, size);
        TagLib::ID3v2::Header header(byteVector);
        return header.tagSize();
    }
    return 0;
}

static inline signed int scale(mad_fixed_t sample)
{
    /* round */
    sample += (1L << (MAD_F_FRACBITS - 16));

    /* clip */
    if (sample >= MAD_F_ONE)
        sample = MAD_F_ONE - 1;
    else if (sample < -MAD_F_ONE)
        sample = -MAD_F_ONE;

    /* quantize */
    return sample >> (MAD_F_FRACBITS + 1 - 16);
}

static inline signed long fix_sample(unsigned int bits, mad_fixed_t sample)
{
    mad_fixed_t quantized, check;
    // clip
    quantized = sample;
    check = (sample >> MAD_F_FRACBITS) + 1;
    if (check & ~1)
    {
        if (sample >= MAD_F_ONE)
            quantized = MAD_F_ONE - 1;
        else if (sample < -MAD_F_ONE)
            quantized = -MAD_F_ONE;
    }
    // quantize
    quantized &= ~((1L << (MAD_F_FRACBITS + 1 - bits)) - 1);
    // scale
    return quantized >> (MAD_F_FRACBITS + 1 - bits);
}

qint64 DecoderMAD::madOutput(char *data, qint64 size)
{
    unsigned int samples, channels;
    mad_fixed_t const *left, *right;

    samples = synth.pcm.length;
    channels = synth.pcm.channels;
    left = synth.pcm.samples[0];
    right = synth.pcm.samples[1];
    m_bitrate = frame.header.bitrate / 1000;
    m_output_at = 0;
    m_output_bytes = 0;

    if(samples * channels * 2 > size)
    {
        qWarning("DecoderMad: input buffer is too small");
        samples = size / channels / 2;
    }

    while (samples--)
    {
        signed int sample;

        sample = fix_sample(16, *left++);
        *(data + m_output_at++) = ((sample >> 0) & 0xff);
        *(data + m_output_at++) = ((sample >> 8) & 0xff);
        m_output_bytes += 2;

        if (channels == 2)
        {
            sample = fix_sample(16, *right++);
            *(data + m_output_at++) = ((sample >> 0) & 0xff);
            *(data + m_output_at++) = ((sample >> 8) & 0xff);
            m_output_bytes += 2;
        }
    }
    return m_output_bytes;
}
