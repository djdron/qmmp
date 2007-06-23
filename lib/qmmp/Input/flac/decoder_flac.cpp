/***************************************************************************
 *   Copyright (C) 2006 by Ilya Kotov                                      *
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


#include "constants.h"
#include "buffer.h"
#include "output.h"
#include "recycler.h"

#include <qobject.h>
#include <qiodevice.h>
#include <FLAC/all.h>
#include "decoder_flac.h"



static size_t pack_pcm_signed (FLAC__byte *data,
                               const FLAC__int32 * const input[],
                               unsigned wide_samples,
                               unsigned channels, unsigned bps)
{
    FLAC__byte * const start = data;
    FLAC__int32 sample;
    const FLAC__int32 *input_;
    unsigned samples, channel;
    unsigned bytes_per_sample;
    unsigned incr;

    if (bps == 24)
        bps = 32; /* we encode to 32-bit words */
    bytes_per_sample = bps / 8;
    incr = bytes_per_sample * channels;

    for (channel = 0; channel < channels; channel++)
    {
        samples = wide_samples;
        data = start + bytes_per_sample * channel;
        input_ = input[channel];

        while (samples--)
        {
            sample = *input_++;

            switch (bps)
            {
            case 8:
                data[0] = sample;
                break;
            case 16:
                data[1] = (FLAC__byte)(sample >> 8);
                data[0] = (FLAC__byte)sample;
                break;
            case 32:
                data[3] = (FLAC__byte)(sample >> 16);
                data[2] = (FLAC__byte)(sample >> 8);
                data[1] = (FLAC__byte)sample;
                data[0] = 0;
                break;
            }

            data += incr;
        }
    }

    return wide_samples * channels * bytes_per_sample;
}

static int flac_decode (void *void_data, char *buf, int buf_len) /*,
      struct sound_params *sound_params)*/
{
    //struct flac_data *data = (struct flac_data *)void_data;
    DecoderFLAC *dflac = (DecoderFLAC *) void_data;
    unsigned to_copy;
    int bytes_per_sample;
    FLAC__uint64 decode_position;

    bytes_per_sample = dflac->data()->bits_per_sample / 8;

    /*switch (bytes_per_sample) {
      case 1:
         sound_params->fmt = SFMT_S8;
         break;
      case 2:
         sound_params->fmt = SFMT_S16 | SFMT_LE;
         break;
      case 3:
         sound_params->fmt = SFMT_S32 | SFMT_LE;
         break;
    }

    sound_params->rate = data->sample_rate;
    sound_params->channels = data->channels;*/

    //decoder_error_clear (&data->error);

    if (!dflac->data()->sample_buffer_fill)
    {

        if (FLAC__stream_decoder_get_state(dflac->data()->decoder)
                == FLAC__STREAM_DECODER_END_OF_STREAM)
        {
            return 0;
        }

        if (!FLAC__stream_decoder_process_single(
                    dflac->data()->decoder))
        {
            return 0;
        }

        /* Count the bitrate */
        if (!FLAC__stream_decoder_get_decode_position(
                    dflac->data()->decoder, &decode_position))
            decode_position = 0;
        if (decode_position > dflac->data()->last_decode_position)
        {
            int bytes_per_sec = bytes_per_sample * dflac->data()->sample_rate
                                * dflac->data()->channels;

            dflac->data()->bitrate = int(((float)decode_position -
                                          dflac->data()->last_decode_position) * 8.0 *
                                          bytes_per_sec /
                                          dflac->data()->sample_buffer_fill / 1000);
        }

        dflac->data()->last_decode_position = decode_position;
    }

    to_copy = qMin((unsigned)buf_len, dflac->data()->sample_buffer_fill);
    memcpy (buf, dflac->data()->sample_buffer, to_copy);
    memmove (dflac->data()->sample_buffer,
             dflac->data()->sample_buffer + to_copy,
             dflac->data()->sample_buffer_fill - to_copy);
    dflac->data()->sample_buffer_fill -= to_copy;
    return to_copy;
}


static FLAC__StreamDecoderReadStatus
flac_callback_read (const FLAC__StreamDecoder *, FLAC__byte buffer[],
                    size_t *bytes, void *client_data)
{
    DecoderFLAC *dflac = (DecoderFLAC *) client_data;
    qint64 res;

    res = dflac->input()->read((char *)buffer, *bytes);

    if (res > 0)
    {
        *bytes = res;
        return FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;
    }
    if (res == 0)
    {
        *bytes = res;
        return FLAC__STREAM_DECODER_READ_STATUS_END_OF_STREAM;
    }

    return FLAC__STREAM_DECODER_READ_STATUS_ABORT;

}

static FLAC__StreamDecoderWriteStatus
flac_callback_write (const FLAC__StreamDecoder *, const FLAC__Frame *frame, const FLAC__int32 * const buffer[], void *client_data)
{
    DecoderFLAC *dflac = (DecoderFLAC *) client_data;
    const unsigned wide_samples = frame->header.blocksize;

    if (dflac->data()->abort)
        return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;


    dflac->data()->sample_buffer_fill = pack_pcm_signed (
                                            dflac->data()->sample_buffer,
                                            buffer, wide_samples,
                                            dflac->data()->channels,
                                            dflac->data()->bits_per_sample);

    return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}

static FLAC__StreamDecoderTellStatus
flac_callback_tell (const FLAC__StreamDecoder *, FLAC__uint64 *offset, void *client_data)
{
    DecoderFLAC *dflac = (DecoderFLAC *) client_data;
    *offset = dflac->input()->pos ();
    return FLAC__STREAM_DECODER_TELL_STATUS_OK;
}

static FLAC__StreamDecoderSeekStatus
flac_callback_seek (const FLAC__StreamDecoder *, FLAC__uint64 offset, void *client_data)
{
    DecoderFLAC *dflac = (DecoderFLAC *) client_data;

    return dflac->input()->seek(offset)
           ? FLAC__STREAM_DECODER_SEEK_STATUS_OK
           : FLAC__STREAM_DECODER_SEEK_STATUS_ERROR;
}

static FLAC__StreamDecoderLengthStatus
flac_callback_length (const FLAC__StreamDecoder *, FLAC__uint64 *stream_length, void *client_data)
{
    DecoderFLAC *dflac = (DecoderFLAC *) client_data;
    *stream_length = dflac->input()->size();
    return FLAC__STREAM_DECODER_LENGTH_STATUS_OK;
}

static void
flac_callback_metadata (const FLAC__StreamDecoder *, const FLAC__StreamMetadata *metadata, void *client_data)
{
    DecoderFLAC *dflac = (DecoderFLAC *) client_data;

    if (metadata->type == FLAC__METADATA_TYPE_STREAMINFO)
    {
        qDebug ("DecoderFLAC: getting metadata info");

        dflac->data()->total_samples =
            (unsigned)(metadata->data.stream_info.total_samples
                       & 0xffffffff);
        dflac->data()->bits_per_sample =
            metadata->data.stream_info.bits_per_sample;
        dflac->data()->channels = metadata->data.stream_info.channels;
        dflac->data()->sample_rate = metadata->data.stream_info.sample_rate;
        dflac->data()->length = dflac->data()->total_samples / dflac->data()->sample_rate;
    }
}

static FLAC__bool
flac_callback_eof (const FLAC__StreamDecoder *, void *)
{
    return FALSE;
}

static void
flac_callback_error (const FLAC__StreamDecoder *, FLAC__StreamDecoderErrorStatus status, void *client_data)
{}

// Decoder class

DecoderFLAC::DecoderFLAC(QObject *parent, DecoderFactory *d, QIODevice *i, Output *o)
        : Decoder(parent, d, i, o)
{
    inited = FALSE;
    user_stop = FALSE;
    stat = 0;
    output_buf = 0;
    output_bytes = 0;
    output_at = 0;
    bks = 0;
    done = FALSE;
    finish = FALSE;
    len = 0;
    freq = 0;
    bitrate = 0;
    seekTime = -1.0;
    totalTime = 0.0;
    chan = 0;
    output_size = 0;
    m_data = 0;




}


DecoderFLAC::~DecoderFLAC()
{
    deinit();
    if (data())
    {
        if (data()->decoder)
            FLAC__stream_decoder_delete (data()->decoder);
        delete data();
        m_data = 0;
    }

    if (output_buf)
        delete [] output_buf;
    output_buf = 0;
}


void DecoderFLAC::stop()
{
    user_stop = TRUE;
}


void DecoderFLAC::flush(bool final)
{
    //qDebug("DecoderFLAC: flush()");
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


bool DecoderFLAC::initialize()
{
    bks = blockSize();
    inited = user_stop = done = finish = FALSE;
    len = freq = bitrate = 0;
    stat = chan = 0;
    output_size = 0;
    seekTime = -1.0;
    totalTime = 0.0;


    if (! input())
    {
        error("DecoderFLAC: cannot initialize.  No input.");

        return FALSE;
    }

    if (! output_buf)
        output_buf = new char[globalBufferSize];
    output_at = 0;
    output_bytes = 0;

    if (! input()->isOpen())
    {
        if (! input()->open(QIODevice::ReadOnly))
        {

            return FALSE;
        }
    }


    if (! input())
    {
        error("DecoderFLAC: cannot initialize.  No input.");

        return FALSE;
    }

    if (! output_buf)
        output_buf = new char[globalBufferSize];
    output_at = 0;
    output_bytes = 0;

    if (! input()->isOpen())
    {
        if (! input()->open(QIODevice::ReadOnly))
        {
            return FALSE;
        }
    }
    if (!m_data)
    {
        m_data = new flac_data;
        m_data->decoder = NULL;
    }

    m_data->bitrate = -1;
    m_data->abort = 0;
    m_data->sample_buffer_fill = 0;
    m_data->last_decode_position = 0;
    if (!m_data->decoder)
    {
        qDebug("DecoderFLAC: creating FLAC__StreamDecoder");
        m_data->decoder = FLAC__stream_decoder_new ();
    }
    qDebug("DecoderFLAC: setting callbacks");
    if (FLAC__stream_decoder_init_stream(
                m_data->decoder,
                flac_callback_read,
                flac_callback_seek,
                flac_callback_tell,
                flac_callback_length,
                flac_callback_eof,
                flac_callback_write,
                flac_callback_metadata,
                flac_callback_error,
                this) != FLAC__STREAM_DECODER_INIT_STATUS_OK)
    {
        data()->ok = 0;
        return FALSE;
    }

    if (!FLAC__stream_decoder_process_until_end_of_metadata(
                data()->decoder))
    {
        data()->ok = 0;
        return FALSE;
    }
    chan = data()->channels;
    if (output())
        output()->configure(data()->sample_rate, data()->channels, 16, bitrate);
    totalTime = data()->length;

    inited = TRUE;
    qDebug("DecoderFLAC: initialize succes");
    return TRUE;
}


double DecoderFLAC::lengthInSeconds()
{
    if (! inited)
        return 0;

    return totalTime;
}


void DecoderFLAC::seek(double pos)
{
    seekTime = pos;
}


void DecoderFLAC::deinit()
{
    FLAC__stream_decoder_finish (data()->decoder);
    inited = user_stop = done = finish = FALSE;
    len = freq = bitrate = 0;
    stat = chan = 0;
    output_size = 0;
}

void DecoderFLAC::run()
{
    mutex()->lock ();

    if (! inited)
    {
        mutex()->unlock();

        return;
    }
    stat = DecoderState::Decoding;
    mutex()->unlock();
    {
        dispatch(DecoderState ((DecoderState::Type) stat));
    }

    while (! done && ! finish)
    {
        mutex()->lock ();
        // decode

        if (seekTime >= 0.0)
        {
            FLAC__uint64 target_sample;

            target_sample = (FLAC__uint64)((seekTime/(double)data()->length) *
                                           (double)data()->total_samples);

            FLAC__stream_decoder_seek_absolute(data()->decoder,
                                               target_sample);
            seekTime = -1.0;
        }
        len = flac_decode (this, (char *) (output_buf + output_at), bks);

        if (len > 0)
        {
            bitrate = data()->bitrate;
            output_at += len;
            output_bytes += len;

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
                finish = TRUE;
            }
        }
        else
        {
            // error in read
            error("DecoderFLAC: Error while decoding stream, File appears to be "
                  "corrupted");

            finish = TRUE;
        }

        mutex()->unlock();
    }

    mutex()->lock ();

    if (finish)
        stat = DecoderState::Finished;
    else if (user_stop)
        stat = DecoderState::Stopped;

    mutex()->unlock();

    {
        dispatch(DecoderState ((DecoderState::Type) stat));
    }

    deinit();
}
