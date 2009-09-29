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
#include <QIODevice>
#include <QtGlobal>

#include <qmmp/buffer.h>
#include <qmmp/output.h>
#include <qmmp/recycler.h>

#include "decoder_aac.h"
#include "aacfile.h"

#define AAC_BUFFER_SIZE 4096

// Decoder class

DecoderAAC::DecoderAAC(QIODevice *i)
        : Decoder(i)
{
    m_bitrate = 0;
    m_totalTime = 0;
    m_data = 0;
    m_input_buf = 0;
    m_input_at = 0;
}


DecoderAAC::~DecoderAAC()
{
    if (data())
    {
        if (data()->handle)
            NeAACDecClose (data()->handle);
        delete data();
        m_data = 0;
    }
    if (m_input_buf)
        delete [] m_input_buf;
    m_input_buf = 0;
    m_bitrate = 0;
}

bool DecoderAAC::initialize()
{
    m_bitrate = 0;
    m_totalTime = 0;

    if (!input())
    {
        qWarning("DecoderAAC: cannot initialize.  No input.");
        return FALSE;
    }
    if (!m_input_buf)
        m_input_buf = new char[AAC_BUFFER_SIZE];
    m_input_at = 0;

    if (!input()->isOpen())
    {
        if (!input()->open(QIODevice::ReadOnly))
        {
            qWarning("DecoderAAC: %s", qPrintable(input()->errorString ()));
            return FALSE;
        }
    }
    AACFile aac_file(input());
    if (!aac_file.isValid())
    {
        qWarning("DecoderAAC: unsupported AAC file");
        return FALSE;
    }
    m_totalTime = aac_file.length() * 1000;
    m_bitrate = aac_file.bitrate();

    if (!m_data)
        m_data = new aac_data;

    data()->handle = NeAACDecOpen();

    NeAACDecConfigurationPtr conf;
    conf = NeAACDecGetCurrentConfiguration(data()->handle);
    conf->downMatrix = 1;
    conf->defSampleRate = 44100;
    conf->dontUpSampleImplicitSBR = 0;
    conf->defObjectType = LC;
    conf->outputFormat = FAAD_FMT_16BIT;
    NeAACDecSetConfiguration(data()->handle, conf);

    m_input_at = input()->read((char *)m_input_buf, AAC_BUFFER_SIZE);

    //skip id3 tag
    int tag_size = 0;
    if (!memcmp(m_input_buf, "ID3", 3))
    {
        /* high bit is not used */
        tag_size = (m_input_buf[6] << 21) | (m_input_buf[7] << 14) |
                   (m_input_buf[8] <<  7) | (m_input_buf[9] <<  0);

        tag_size += 10;
        memmove (m_input_buf, m_input_buf + tag_size, m_input_at - tag_size);
        m_input_at -= tag_size;
        m_input_at += input()->read((char *)(m_input_buf + m_input_at), AAC_BUFFER_SIZE - m_input_at);
    }
    #ifdef FAAD_MODIFIED
    uint32_t freq = 0;
    uint8_t chan = 0;
    #else
    unsigned long freq = 0;
    unsigned char chan = 0;
    #endif
    int res = NeAACDecInit (data()->handle, (unsigned char*) m_input_buf, m_input_at, &freq, &chan);

    if (res < 0)
    {
        qWarning("DecoderAAC: NeAACDecInit() failed");
        return FALSE;
    }
    if (!freq || !chan)
    {
        qWarning("DecoderAAC: invalid sound parameters");
        return FALSE;
    }

    memmove(m_input_buf, m_input_buf + res, m_input_at - res);
    m_input_at -= res;
    configure(freq, chan, 16);
    qDebug("DecoderAAC: initialize succes");
    return TRUE;
}

qint64 DecoderAAC::read(char *audio, qint64 maxSize)
{
    NeAACDecFrameInfo frame_info;
    qint64 size = 0, to_read, read;
    void *out = 0;
    bool eof = FALSE;

    while (size <= 0 && !eof)
    {
        if (m_input_at < AAC_BUFFER_SIZE)
        {
            to_read = AAC_BUFFER_SIZE - m_input_at;
            read = input()->read((char *) (m_input_buf + m_input_at),  to_read);
            eof = (read != to_read);
            m_input_at += read;
        }

        out = NeAACDecDecode(data()->handle, &frame_info, (uchar *)m_input_buf, m_input_at);
        memmove(m_input_buf, m_input_buf + frame_info.bytesconsumed, m_input_at - frame_info.bytesconsumed);
        m_input_at -= frame_info.bytesconsumed;

        if ((size = frame_info.samples * 2) > 0 && size <= maxSize)
            memcpy((void *) (audio), out, size);
        if (frame_info.error > 0)
        {
            m_input_at = 0;
            qDebug("DecoderAAC: %s", NeAACDecGetErrorMessage(frame_info.error));
        }
    }
    return size;
}

qint64 DecoderAAC::totalTime()
{
    return m_totalTime;
}

int DecoderAAC::bitrate()
{
    return m_bitrate;
}

void DecoderAAC::seek(qint64 pos)
{
    input()->seek(pos * input()->size() / m_totalTime);
    NeAACDecPostSeekReset (data()->handle, 0);
    m_input_at = 0;
}
