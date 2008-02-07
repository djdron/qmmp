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


#include <QObject>
#include <QIODevice>

#include "constants.h"
#include "buffer.h"
#include "output.h"
#include "recycler.h"

#include "decoder_mpc.h"

// this function used from xmms
inline static void copyBuffer(MPC_SAMPLE_FORMAT* pInBuf, char* pOutBuf, unsigned pLength)
{
    unsigned pSize = 16;
    int clipMin    = -1 << (pSize - 1);
    int clipMax    = (1 << (pSize - 1)) - 1;
    int floatScale =  1 << (pSize - 1);
    for (unsigned n = 0; n < 2 * pLength; n++)
    {
        int val;
#ifdef MPC_FIXED_POINT
        val = shiftSigned(pInBuf[n], pSize - MPC_FIXED_POINT_SCALE_SHIFT);
#else
        val = (int) (pInBuf[n] * floatScale);
#endif
        if (val < clipMin)
            val = clipMin;
        else if (val > clipMax)
            val = clipMax;
        unsigned shift = 0;
        do
        {
            pOutBuf[n * 2 + (shift / 8)] = (unsigned char) ((val >> shift) & 0xFF);
            shift += 8;
        }
        while (shift < pSize);
    }
}

// mpc callbacks

static mpc_int32_t mpc_callback_read (void *data, void *buffer, mpc_int32_t size)
{
    DecoderMPC *dmpc = (DecoderMPC *) data;
    qint64 res;

    res = dmpc->input()->read((char *)buffer, size);

    return res;
}

static mpc_bool_t mpc_callback_seek (void *data, mpc_int32_t offset)
{
    DecoderMPC *dmpc = (DecoderMPC *) data;

    return dmpc->input()->seek(offset); // ? TRUE : FALSE;
}

static mpc_int32_t mpc_callback_tell (void *data)
{
    DecoderMPC *dmpc = (DecoderMPC *) data;
    return dmpc->input()->pos ();
}

static mpc_bool_t  mpc_callback_canseek (void *data)
{
    DecoderMPC *dmpc = (DecoderMPC *) data;
    return !dmpc->input()->isSequential () ;
}

static mpc_int32_t mpc_callback_get_size (void *data)
{
    DecoderMPC *dmpc = (DecoderMPC *) data;
    return dmpc->input()->size();
}

// Decoder class

DecoderMPC::DecoderMPC(QObject *parent, DecoderFactory *d, QIODevice *i, Output *o)
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


DecoderMPC::~DecoderMPC()
{
    deinit();
    if(data())
    {
        delete data();
        m_data = 0;
    }
    if (output_buf)
        delete [] output_buf;
    output_buf = 0;
}


void DecoderMPC::stop()
{
    user_stop = TRUE;
}


void DecoderMPC::flush(bool final)
{
    ulong min = final ? 0 : bks;

    while ((! done && ! finish) && output_bytes > min)
    {
        output()->recycler()->mutex()->lock ();

        while ((! done && ! finish) && output()->recycler()->full())
        {
            mutex()->unlock();

            output()->recycler()->cond()->wait(output()->recycler()->mutex());

            mutex()->lock ();
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


bool DecoderMPC::initialize()
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
        error("DecoderMPC: cannot initialize.  No input.");

        return FALSE;
    }

    if (! output_buf)
        output_buf = new char[globalBufferSize];
    output_at = 0;
    output_bytes = 0;

    if (! input())
    {
        error("DecoderMPC: cannot initialize.  No input.");

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
            error("DecoderMPC: cannot open input.");
            return FALSE;
        }
    }
    if (!m_data)
    {
        m_data = new mpc_data;
    }

    qDebug("DecoderMPC: setting callbacks");
    m_data->reader.read = mpc_callback_read;
    m_data->reader.seek = mpc_callback_seek;
    m_data->reader.tell = mpc_callback_tell;
    m_data->reader.canseek = mpc_callback_canseek;
    m_data->reader.get_size = mpc_callback_get_size;
    m_data->reader.data = this;

    mpc_streaminfo_init (&m_data->info);

    if (mpc_streaminfo_read (&m_data->info, &m_data->reader) != ERROR_CODE_OK)
        return FALSE;
    chan = data()->info.channels;
    configure(data()->info.sample_freq, chan, 16, data()->info.bitrate);

    mpc_decoder_setup (&data()->decoder, &data()->reader);

    //mpc_decoder_scale_output (&data()->decoder, 3.0);

    if (!mpc_decoder_initialize (&data()->decoder, &data()->info))
    {
        error("DecoderMPC: cannot get info.");
        return FALSE;
    }
    totalTime = mpc_streaminfo_get_length(&data()->info);
    inited = TRUE;
    qDebug("DecoderMPC: initialize succes");
    return TRUE;
}


double DecoderMPC::lengthInSeconds()
{
    if (! inited)
        return 0;

    return totalTime;
}


void DecoderMPC::seek(double pos)
{
    seekTime = pos;
}


void DecoderMPC::deinit()
{
    //FLAC__stream_decoder_finish (data()->decoder);
    inited = user_stop = done = finish = FALSE;
    len = freq = bitrate = 0;
    stat = chan = 0;
    output_size = 0;
}

void DecoderMPC::run()
{
    mpc_uint32_t vbrAcc = 0;
    mpc_uint32_t vbrUpd = 0;
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
            mpc_decoder_seek_seconds(&data()->decoder, seekTime);
            seekTime = -1.0;
        }
        MPC_SAMPLE_FORMAT buffer[MPC_DECODER_BUFFER_LENGTH];

        len = mpc_decoder_decode (&data()->decoder, buffer, &vbrAcc, &vbrUpd);

        copyBuffer(buffer, (char *) (output_buf + output_at), len);

        len = len * 4;

        if (len > 0)
        {
            bitrate = vbrUpd * data()->info.sample_freq / 1152;
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
            error("DecoderMPC: Error while decoding stream, File appears to be "
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
