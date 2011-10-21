/***************************************************************************
 *   Copyright (C) 2011 by Ilya Kotov                                      *
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

#include <taglib/id3v2header.h>
#include <taglib/tbytevector.h>
#include <qmmp/buffer.h>
#include <qmmp/output.h>
#include <math.h>
#include <stdio.h>
#include "tagextractor.h"
#include "decoder_mpg123.h"

DecoderMPG123::DecoderMPG123(const QString &url, QIODevice *i) : Decoder(i)
{
    Q_UNUSED(i); //TODO add stream-based input support
    m_url = url;
    m_totalTime = 0;
    m_rate = 0;
    m_frame_info.bitrate = 0;
}

DecoderMPG123::~DecoderMPG123()
{
    cleanup(m_handle);
}

bool DecoderMPG123::initialize()
{
    int err = mpg123_init();
    if(err != MPG123_OK)
    {
        qWarning("DecoderMPG123: basic setup goes wrong: %s", mpg123_plain_strerror(err));
        return false;
    }
    int channels = 0, encoding = 0;

    if(!(m_handle = mpg123_new(0, &err)))
    {
        qWarning("DecoderMPG123: basic setup goes wrong: %s", mpg123_plain_strerror(err));
        return false;
    }

    if((err = mpg123_open(m_handle, qPrintable(m_url))) != MPG123_OK)
    {
        qWarning("DecoderMPG123: mpg123 error: %s", mpg123_plain_strerror(err));
        cleanup(m_handle);
        m_handle = 0;
        return false;
    }

    if((err = mpg123_getformat(m_handle, &m_rate, &channels, &encoding)) != MPG123_OK)
    {
        qWarning("DecoderMPG123: mpg123 error: %s", mpg123_plain_strerror(err));
        cleanup(m_handle);
        m_handle = 0;
        return false;
    }
    //check format
    if(encoding != MPG123_ENC_SIGNED_16)
    {
        cleanup(m_handle);
        qWarning("DecoderMPG123: bad encoding: 0x%x!\n", encoding);
        m_handle = 0;
        return false;
    }

    /* Ensure that this output format will not change (it could, when we allow it). */
    mpg123_format_none(m_handle);
    mpg123_format(m_handle, m_rate, channels, encoding);
    //duration
    mpg123_scan(m_handle);
    m_totalTime = (qint64) mpg123_length(m_handle) * 1000 / m_rate;

    configure(m_rate, channels, Qmmp::PCM_S16LE);

    return true;
}

qint64 DecoderMPG123::totalTime()
{
    return m_totalTime;
}

int DecoderMPG123::bitrate()
{
    return m_frame_info.bitrate;
}

qint64 DecoderMPG123::read(char *data, qint64 size)
{
    size_t done;
    int err = mpg123_read(m_handle, (unsigned char *)data, size, &done);
    if(err != MPG123_DONE && err != MPG123_OK)
    {
        qWarning("DecoderMPG123: deocder error: %s", mpg123_plain_strerror(err));
        return -1;
    }
    mpg123_info(m_handle, &m_frame_info);
    return done;
}
void DecoderMPG123::seek(qint64 pos)
{
    if(m_totalTime > 0)
    {
        mpg123_seek(m_handle, pos * m_rate / 1000, SEEK_SET);
    }
}

void DecoderMPG123::cleanup(mpg123_handle *handle)
{
    mpg123_close(handle);
    mpg123_delete(handle);
    handle = 0;
}
