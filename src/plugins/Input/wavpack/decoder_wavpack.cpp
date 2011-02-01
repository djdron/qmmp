/***************************************************************************
 *   Copyright (C) 2008-2010 by Ilya Kotov                                 *
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
#include <QFile>
#include <math.h>
#include <stdint.h>
#include <qmmp/buffer.h>
#include <qmmp/output.h>
#include <qmmp/recycler.h>
#include "decoder_wavpack.h"
#include "cueparser.h"

// Decoder class

DecoderWavPack::DecoderWavPack(const QString &path)
        : Decoder()
{
    m_path = path;
    m_totalTime = 0.0;
    m_chan = 0;
    m_context = 0;
    m_freq = 0;
    m_parser = 0;
    m_output_buf = 0;
    m_parser = 0;
    length_in_bytes = 0;
    m_totalBytes = 0;
    m_sz = 0;
    m_buf = 0;
    m_offset = 0;
    m_bps = 0;
}

DecoderWavPack::~DecoderWavPack()
{
    deinit();
    if (m_output_buf)
        delete [] m_output_buf;
    m_output_buf = 0;
}

bool DecoderWavPack::initialize()
{
    m_chan = 0;
    m_totalTime = 0;

    char err [80];
    if (m_path.startsWith("wvpack://")) //embeded cue track
    {
        QString p = QUrl(m_path).path();
        p.replace(QString(QUrl::toPercentEncoding("#")), "#");
        p.replace(QString(QUrl::toPercentEncoding("?")), "?");
        p.replace(QString(QUrl::toPercentEncoding("%")), "%");
        p.replace(QString(QUrl::toPercentEncoding(":")), ":");
        m_context = WavpackOpenFileInput (p.toLocal8Bit(), err, OPEN_WVC | OPEN_TAGS, 0);
        int cue_len = WavpackGetTagItem (m_context, "cuesheet", NULL, 0);
        char *value;
        if (cue_len)
        {
            value = (char*)malloc (cue_len * 2 + 1);
            WavpackGetTagItem (m_context, "cuesheet", value, cue_len + 1);
            m_parser = new CUEParser(value, p);
            m_track = m_path.section("#", -1).toInt();
            if(m_track > m_parser->count())
            {
                qWarning("DecoderWavPack: invalid cuesheet comment");
                return false;
            }
            m_path = p;
            //send metadata
            QMap<Qmmp::MetaData, QString> metaData = m_parser->info(m_track)->metaData();
            StateHandler::instance()->dispatch(metaData);
        }
    }
    else
        m_context = WavpackOpenFileInput (m_path.toLocal8Bit(), err, OPEN_WVC | OPEN_TAGS, 0);

    if (!m_context)
    {
        qWarning("DecoderWavPack: error: %s", err);
        return false;
    }

    m_chan = WavpackGetNumChannels(m_context);
    m_freq = WavpackGetSampleRate (m_context);
    m_bps = WavpackGetBitsPerSample (m_context);
    if (!m_output_buf)
        m_output_buf = new int32_t[QMMP_BLOCK_FRAMES * m_chan];
    switch(m_bps)
    {
    case 8:
        configure(m_freq, m_chan, Qmmp::PCM_S8);
        break;
    case 16:
        configure(m_freq, m_chan, Qmmp::PCM_S16LE);
        break;
    case 24:
    case 32:
        configure(m_freq, m_chan, Qmmp::PCM_S32LE);
    }
    if(!m_parser)
        m_totalTime = (qint64) WavpackGetNumSamples(m_context) * 1000 / m_freq;
    else
    {
        m_length = m_parser->length(m_track);
        m_offset = m_parser->offset(m_track);
        length_in_bytes = audioParameters().sampleRate() *
                          audioParameters().channels() *
                          audioParameters().sampleSize() * m_length/1000;
        seek(0);
    }
    m_totalBytes = 0;
    m_sz = audioParameters().sampleSize() * audioParameters().channels();
    qDebug("DecoderWavPack: initialize succes");
    return true;
}

int DecoderWavPack::bitrate()
{
    if(m_context)
        return int(WavpackGetInstantBitrate(m_context)/1000);
    return 0;
}

qint64 DecoderWavPack::totalTime()
{
    if(m_parser)
        return m_length;
    return m_totalTime;
}

void DecoderWavPack::deinit()
{
    m_chan = 0;
    m_freq = 0;
    if (m_context)
        WavpackCloseFile (m_context);
    m_context = 0;
    if(m_parser)
        delete m_parser;
    m_parser = 0;
    if(m_buf)
        delete m_buf;
    m_buf = 0;
}

void DecoderWavPack::seek(qint64 time)
{
    m_totalBytes = audioParameters().sampleRate() *
                   audioParameters().channels() *
                   audioParameters().sampleSize() * time/1000;
    if(m_parser)
        time += m_offset;
    WavpackSeekSample (m_context, time * m_freq / 1000);
}

qint64 DecoderWavPack::read(char *data, qint64 size)
{
    if(m_parser)
    {
        if(length_in_bytes - m_totalBytes < m_sz) //end of cue track
            return 0;

        qint64 len = 0;

        if(m_buf) //read remaining data first
        {
            len = qMin(m_buf_size, size);
            memmove(data, m_buf, len);
            if(size >= m_buf_size)
            {
                delete[] m_buf;
                m_buf = 0;
                m_buf_size = 0;
            }
            else
                memmove(m_buf, m_buf + len, size - len);
        }
        else
            len = wavpack_decode (data, size);

        if(len <= 0) //end of file
            return 0;

        if(len + m_totalBytes <= length_in_bytes)
        {
            m_totalBytes += len;
            return len;
        }

        qint64 len2 = qMax(qint64(0), length_in_bytes - m_totalBytes);
        len2 = (len2 / m_sz) * m_sz; //returned size must contain integer number of samples
        m_totalBytes += len2;
        //save data of the next track
        if(m_buf)
            delete[] m_buf;
        m_buf_size = len - len2;
        m_buf = new char[m_buf_size];
        memmove(m_buf, data + len2, m_buf_size);
        return len2;
    }
    return wavpack_decode(data, size);
}

const QString DecoderWavPack::nextURL()
{
    if(m_parser && m_track +1 <= m_parser->count())
        return m_parser->trackURL(m_track + 1);
    else
        return QString();
}

void DecoderWavPack::next()
{
    if(m_parser && m_track +1 <= m_parser->count())
    {
        m_track++;
        m_offset = m_parser->length(m_track);
        m_length = m_parser->length(m_track);
        length_in_bytes = audioParameters().sampleRate() *
                          audioParameters().channels() *
                          audioParameters().sampleSize() * m_length/1000;
        StateHandler::instance()->dispatch(m_parser->info(m_track)->metaData());
        m_totalBytes = 0;
    }
}

qint64 DecoderWavPack::wavpack_decode(char *data, qint64 size)
{
    ulong len = qMin(QMMP_BLOCK_FRAMES, (int)size / m_chan / 4);
    len = WavpackUnpackSamples (m_context, m_output_buf, len);
    //convert 32 to 16
    qint8 *data8 = (qint8 *)data;
    qint16 *data16 = (qint16 *)data;
    qint32 *data32 = (qint32 *)data;
    uint i = 0;
    switch (m_bps)
    {
    case 8:
        for (i = 0;  i < len * m_chan; ++i)
            data8[i] = m_output_buf[i];
         return len * m_chan;
    case 16:
        for (i = 0;  i < len * m_chan; ++i)
            data16[i] = m_output_buf[i];
         return len * m_chan * 2;
    case 24:
        for (i = 0;  i < len * m_chan; ++i)
            data32[i] = m_output_buf[i] << 8;
         return len * m_chan * 4;
    case 32:
        for (i = 0;  i < len * m_chan; ++i)
            data32[i] = m_output_buf[i];
         return len * m_chan * 4;
    }
    return 0;
}
