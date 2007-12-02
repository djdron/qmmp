/***************************************************************************
 *   Copyright (C) 2007 by Ilya Kotov                                      *
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
#include <QFileInfo>



#include "constants.h"
#include "buffer.h"
#include "output.h"
#include "recycler.h"

#include "decoder_sndfile.h"

// Decoder class

DecoderSndFile::DecoderSndFile(QObject *parent, DecoderFactory *d, QIODevice *i, Output *o)
        : Decoder(parent, d, i, o)
{
    m_inited = FALSE;
    m_user_stop = FALSE;
    m_output_buf = 0;
    m_output_bytes = 0;
    m_output_at = 0;
    bks = 0;
    m_done = FALSE;
    m_finish = FALSE;
    m_freq = 0;
    m_bitrate = 0;
    m_seekTime = -1.0;
    m_totalTime = 0.0;
    m_chan = 0;
    m_output_size = 0;
    m_buf = 0;
    m_sndfile = 0;
}


DecoderSndFile::~DecoderSndFile()
{
    deinit();
}


void DecoderSndFile::stop()
{
    m_user_stop = TRUE;
}


void DecoderSndFile::flush(bool final)
{
    ulong min = final ? 0 : bks;

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
            m_output_size += bks;
            m_output_at = m_output_bytes;
        }

        if (output()->recycler()->full())
        {
            output()->recycler()->cond()->wakeOne();
        }

        output()->recycler()->mutex()->unlock();
    }
}


bool DecoderSndFile::initialize()
{
    bks = blockSize();
    m_inited = m_user_stop = m_done = m_finish = FALSE;
    m_freq = m_bitrate = 0;
    m_output_size = 0;
    m_seekTime = -1.0;
    m_totalTime = 0.0;
    SF_INFO snd_info;


    if (! input())
    {
        error("DecoderSndFile: cannot initialize.  No input.");

        return FALSE;
    }

    if (! m_output_buf)
        m_output_buf = new char[globalBufferSize];
    m_output_at = 0;
    m_output_bytes = 0;

    QString filename = qobject_cast<QFile*>(input())->fileName ();
    input()->close();

    memset (&snd_info, 0, sizeof(snd_info));
    snd_info.format=0;
    m_sndfile = sf_open(filename.toLocal8Bit(), SFM_READ, &snd_info);
    if (!m_sndfile)
    {
        qWarning("DecoderSndFile: failed to open: %s", qPrintable(filename));
        return FALSE;
    }

    m_freq = snd_info.samplerate;
    m_chan = snd_info.channels;

    m_totalTime = (double) snd_info.frames / m_freq;

    m_bitrate =  QFileInfo(filename).size () * 8.0 / m_totalTime / 1000.0 + 0.5;

    configure(m_freq, m_chan, 16, m_bitrate);
    m_buf = new short[blockSize() / sizeof(short)];
    m_inited = TRUE;
    qDebug("DecoderSndFile: detected format: %08X", snd_info.format);
    qDebug("DecoderSndFile: initialize succes");
    return TRUE;
}


double DecoderSndFile::lengthInSeconds()
{
    if (! m_inited)
        return 0;

    return m_totalTime;
}


void DecoderSndFile::seek(double pos)
{
    m_seekTime = pos;
}


void DecoderSndFile::deinit()
{
    m_inited = m_user_stop = m_done = m_finish = FALSE;
    m_freq = m_bitrate = m_chan = 0;
    m_output_size = 0;
    if (m_inited)
    {
       delete m_buf;
       m_buf = 0;
       sf_close(m_sndfile);
       m_sndfile = 0;
    }
}

void DecoderSndFile::run()
{

    long len = 0;
    int stat = 0;

    mutex()->lock ();

    if (! m_inited)
    {
        mutex()->unlock();

        return;
    }

    stat = DecoderState::Decoding;
    mutex()->unlock();

    {
        dispatch(DecoderState ((DecoderState::Type) stat));
    }

    while (! m_done && ! m_finish)
    {
        mutex()->lock ();
        // decode

        if (m_seekTime >= 0.0)
        {
            m_output_size = sf_seek(m_sndfile, m_freq*m_seekTime, SEEK_SET);
            m_seekTime = -1.0;
        }

        len = sizeof(short)* sf_read_short  (m_sndfile, m_buf, blockSize() / sizeof(short));

        if (len > 0)
        {
            memmove((char *)(m_output_buf + m_output_at), (char *) m_buf, len);
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
            // error in read
            error("DecoderSndFile: Error while decoding stream, File appears to be "
                  "corrupted");

            m_finish = TRUE;
        }

        mutex()->unlock();
    }

    mutex()->lock ();

    if (m_finish)
        stat = DecoderState::Finished;
    else if (m_user_stop)
        stat = DecoderState::Stopped;

    mutex()->unlock();

    dispatch(DecoderState ((DecoderState::Type) stat));
    deinit();
}

