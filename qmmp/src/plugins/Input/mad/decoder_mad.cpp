// Copyright (c) 2000-2001 Brad Hughes <bhughes@trolltech.com>
//
// Use, modification and distribution is allowed without limitation,
// warranty, or liability of any kind.
//


#include <QtGui>

#include "decoder_mad.h"
#include "tagextractor.h"
#include <qmmp/constants.h>
#include <qmmp/buffer.h>
#include <qmmp/output.h>

#include <math.h>
#include <stdio.h>

# define XING_MAGIC (('X' << 24) | ('i' << 16) | ('n' << 8) | 'g')


DecoderMAD::DecoderMAD(QObject *parent, DecoderFactory *d, QIODevice *i, Output *o)
        : Decoder(parent, d, i, o)
{
    inited = false;
    user_stop = false;
    done = false;
    finish = false;
    derror = false;
    eof = false;
    useeq = false;
    totalTime = 0.;
    seekTime = -1.;
    channels = 0;
    bks = 0;
    bitrate = 0;
    freq = 0;
    len = 0;
    input_buf = 0;
    input_bytes = 0;
    output_buf = 0;
    output_bytes = 0;
    output_at = 0;
    output_size = 0;
}

DecoderMAD::~DecoderMAD()
{
    wait();
    deinit();
    mutex()->lock();
    if (input_buf)
    {
        qDebug("DecoderMAD: deleting input_buf");
        delete [] input_buf;
    }
    input_buf = 0;

    if (output_buf)
    {
        qDebug("DecoderMAD: deleting output_buf");
        delete [] output_buf;
    }
    output_buf = 0;
    mutex()->unlock();
}

bool DecoderMAD::initialize()
{
    bks = blockSize();

    inited = false;
    user_stop = false;
    done = false;
    finish = false;
    derror = false;
    eof = false;
    totalTime = 0.;
    seekTime = -1.;
    channels = 0;
    bitrate = 0;
    freq = 0;
    len = 0;
    input_bytes = 0;
    output_bytes = 0;
    output_at = 0;
    output_size = 0;

    if (! input())
    {
        error("DecoderMAD: cannot initialize.  No input.");
        return FALSE;
    }

    if (! input_buf)
        input_buf = new char[globalBufferSize];

    if (! output_buf)
        output_buf = new char[globalBufferSize];

    if (! input()->isOpen())
    {
        if (! input()->open(QIODevice::ReadOnly))
        {
            error("DecoderMAD: Failed to open input.  Error " +
                  QString::number(input()->isOpen()) + ".");
            return FALSE;
        }
    }

    if (input()->isSequential ()) //for streams only
    {
        TagExtractor extractor(input());
        FileTag tag = extractor.id3v2tag();
        if (!tag.isEmpty())
            dispatch(extractor.id3v2tag());
    }

    mad_stream_init(&stream);
    mad_frame_init(&frame);
    mad_synth_init(&synth);

    if (! findHeader())
    {
        qDebug("DecoderMAD: Can't find a valid MPEG header.");
        return FALSE;
    }
    mad_stream_buffer(&stream, (unsigned char *) input_buf, input_bytes);
    stream.error = MAD_ERROR_NONE;
    stream.error = MAD_ERROR_BUFLEN;
    mad_frame_mute (&frame);
    stream.next_frame = NULL;
    stream.sync = 0;
    configure(freq, channels, 16, bitrate);

    inited = TRUE;
    return TRUE;
}


void DecoderMAD::deinit()
{
    if (!inited)
        return;

    mad_synth_finish(&synth);
    mad_frame_finish(&frame);
    mad_stream_finish(&stream);

    inited = false;
    user_stop = false;
    done = false;
    finish = false;
    derror = false;
    eof = false;
    useeq = false;
    totalTime = 0.;
    seekTime = -1.;
    channels = 0;
    bks = 0;
    bitrate = 0;
    freq = 0;
    len = 0;
    input_bytes = 0;
    output_bytes = 0;
    output_at = 0;
    output_size = 0;
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
    bool result = FALSE;
    int count = 0;
    bool has_xing = FALSE;
    bool is_vbr = FALSE;
    mad_timer_t duration = mad_timer_zero;
    struct mad_header header;
    mad_header_init (&header);

    while (TRUE)
    {
        input_bytes = 0;
        if (stream.error == MAD_ERROR_BUFLEN || !stream.buffer)
        {
            size_t remaining = 0;

            if (!stream.next_frame)
            {
                remaining = stream.bufend - stream.next_frame;
                memmove (input_buf, stream.next_frame, remaining);
            }

            input_bytes = input()->read(input_buf + remaining, globalBufferSize - remaining);

            if (input_bytes <= 0)
                break;

            mad_stream_buffer(&stream, (unsigned char *) input_buf + remaining, input_bytes);
            stream.error = MAD_ERROR_NONE;
        }

        if (mad_header_decode(&header, &stream) == -1)
        {
            if (stream.error == MAD_ERROR_BUFLEN)
                continue;
            else if (MAD_RECOVERABLE(stream.error))
                continue;
            else
            {
                qDebug ("DecoderMAD: Can't decode header: %s", mad_stream_errorstr(&stream));
                break;
            }
        }
        result = TRUE;

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
                is_vbr = TRUE;

                qDebug ("DecoderMAD: Xing header detected");

                if (xing.flags & XING_FRAMES)
                {
                    has_xing = TRUE;
                    count = xing.frames;
                    break;
                }
            }
        }
        //try to detect VBR
        if (!is_vbr && !(count > 15))
        {
            if (bitrate && header.bitrate != bitrate)
            {
                qDebug ("DecoderMAD: VBR detected");
                is_vbr = TRUE;
            }
            else
                bitrate = header.bitrate;
        }
        else if (!is_vbr)
        {
            qDebug ("DecoderMAD: Fixed rate detected");
            break;
        }
        mad_timer_add (&duration, header.duration);
    }

    if (!result)
        return FALSE;

    if (!is_vbr)
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

    totalTime = mad_timer_count(duration, MAD_UNITS_SECONDS);
    qDebug ("DecoderMAD: Total time: %ld", long(totalTime));
    freq = header.samplerate;
    channels = MAD_NCHANNELS(&header);
    bitrate = header.bitrate / 1000;
    mad_header_finish(&header);
    input()->seek(0);
    input_bytes = 0;
    return TRUE;
}

double DecoderMAD::lengthInSeconds()
{
    if (! inited)
        return 0.;
    return totalTime;
}

void DecoderMAD::seek(double pos)
{
    seekTime = pos;
}

void DecoderMAD::stop()
{
    user_stop = TRUE;
}

void DecoderMAD::flush(bool final)
{
    ulong min = final ? 0 : bks;

    while ((! done && ! finish) && output_bytes > min)
    {
        output()->recycler()->mutex()->lock();

        while ((! done && ! finish) && output()->recycler()->full())
        {
            mutex()->unlock();
            output()->recycler()->cond()->wait(output()->recycler()->mutex());

            mutex()->lock();
            done = user_stop;
        }

        if (user_stop || finish)
        {
            inited = FALSE;
            done = TRUE;
        }
        else
        {
            output_bytes -= produceSound(output_buf, output_bytes, bitrate, channels);
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

void DecoderMAD::run()
{
    int skip_frames = 1; //skip first frame
    mutex()->lock();

    if (! inited)
    {
        mutex()->unlock();
        return;
    }


    DecoderState::Type stat = DecoderState::Decoding;

    mutex()->unlock();

    dispatch(stat);

    while (! done && ! finish && ! derror)
    {
        mutex()->lock();

        if (seekTime >= 0.0)
        {
            long seek_pos = long(seekTime * input()->size() / totalTime);
            input()->seek(seek_pos);
            output_size = long(seekTime) * long(freq * channels * 16 / 2);
            mad_frame_mute(&frame);
            mad_synth_mute(&synth);
            stream.error = MAD_ERROR_BUFLEN;
            stream.sync = 0;
            input_bytes = 0;
            output_at = 0;
            output_bytes = 0;
            stream.next_frame = 0;
            skip_frames = 2;
            eof = false;
        }

        finish = eof;

        if (! eof)
        {
            if (stream.next_frame && seekTime == -1.)
            {
                input_bytes = &input_buf[input_bytes] - (char *) stream.next_frame;
                memmove(input_buf, stream.next_frame, input_bytes);
            }

            if (input_bytes < globalBufferSize && seekTime == -1.)
            {
                int len = input()->read((char *) input_buf + input_bytes,
                                        globalBufferSize - input_bytes);

                if (len == 0)
                {
                    eof = true;
                }
                else if (len < 0)
                {
                    derror = true;
                    break;
                }

                input_bytes += len;
            }

            mad_stream_buffer(&stream, (unsigned char *) input_buf, input_bytes);
        }

        seekTime = -1.;

        mutex()->unlock();

        // decode
        while (! done && ! finish && ! derror)
        {
            if (mad_frame_decode(&frame, &stream) == -1)
            {
                if (stream.error == MAD_ERROR_LOSTSYNC)
                    continue;

                if (stream.error == MAD_ERROR_BUFLEN)
                    break;

                // error in decoding
                if (! MAD_RECOVERABLE(stream.error))
                {
                    derror = true;
                    break;
                }
                continue;
            }

            mutex()->lock();

            if (seekTime >= 0.)
            {
                mutex()->unlock();
                break;
            }

            if (skip_frames)
            {
                skip_frames-- ;
                mutex()->unlock();
                continue;
            }
            mad_synth_frame(&synth, &frame);
            madOutput();
            mutex()->unlock();
        }
    }

    mutex()->lock();

    if (! user_stop && eof)
    {
        flush(TRUE);

        if (output())
        {
            output()->recycler()->mutex()->lock();
            // end of stream
            while (! output()->recycler()->empty() && ! user_stop)
            {
                output()->recycler()->cond()->wakeOne();
                mutex()->unlock();
                output()->recycler()->cond()->wait(output()->recycler()->mutex());
                mutex()->lock();
            }
            output()->recycler()->mutex()->unlock();
        }

        done = TRUE;
        if (! user_stop)
            finish = TRUE;
    }

    if (finish)
        stat = DecoderState::Finished;
    else if (user_stop)
        stat = DecoderState::Stopped;

    mutex()->unlock();

    dispatch(stat);

    if (input())
        input()->close();
    deinit();

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

enum mad_flow DecoderMAD::madOutput()
{
    unsigned int samples, channels;
    mad_fixed_t const *left, *right;

    samples = synth.pcm.length;
    channels = synth.pcm.channels;
    left = synth.pcm.samples[0];
    right = synth.pcm.samples[1];


    bitrate = frame.header.bitrate / 1000;
    done = user_stop;

    while (samples-- && !user_stop)
    {
        signed int sample;

        if (output_bytes + 4096 > globalBufferSize)
            flush();

        sample = fix_sample(16, *left++);
        *(output_buf + output_at++) = ((sample >> 0) & 0xff);
        *(output_buf + output_at++) = ((sample >> 8) & 0xff);
        output_bytes += 2;

        if (channels == 2)
        {
            sample = fix_sample(16, *right++);
            *(output_buf + output_at++) = ((sample >> 0) & 0xff);
            *(output_buf + output_at++) = ((sample >> 8) & 0xff);
            output_bytes += 2;
        }
    }

    if (done || finish)
    {
        return MAD_FLOW_STOP;
    }

    return MAD_FLOW_CONTINUE;
}

enum mad_flow DecoderMAD::madError(struct mad_stream *stream,
                                   struct mad_frame *)
{
    if (MAD_RECOVERABLE(stream->error))
        return MAD_FLOW_CONTINUE;
    qFatal("MADERROR!\n");
    return MAD_FLOW_STOP;
}
