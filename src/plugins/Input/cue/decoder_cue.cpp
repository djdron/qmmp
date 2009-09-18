/***************************************************************************
 *   Copyright (C) 2008-2009 by Ilya Kotov                                 *
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

#include <qmmp/buffer.h>
#include <qmmp/output.h>
#include <qmmp/recycler.h>
#include <qmmp/fileinfo.h>
#include <qmmp/decoderfactory.h>
#include <qmmp/soundcore.h>

#include <QObject>
#include <QFile>

#include "cueparser.h"
#include "decoder_cue.h"


DecoderCUE::DecoderCUE(const QString &url)
        : Decoder()
{
    m_path = url;
    m_decoder = 0;
}

DecoderCUE::~DecoderCUE()
{
    if(m_decoder)
        delete m_decoder;
    m_decoder = 0;
}

bool DecoderCUE::initialize()
{
    QString p = QUrl(m_path).path();
    p.replace(QString(QUrl::toPercentEncoding("#")), "#");
    p.replace(QString(QUrl::toPercentEncoding("%")), "%");
    CUEParser parser(p);
    if (parser.count() == 0)
    {
        qWarning("DecoderCUE: invalid cue file");
        return FALSE;
    }
    int track = m_path.section("#", -1).toInt();
    m_path = parser.filePath(track);
    if (!QFile::exists(m_path))
    {
        qWarning("DecoderCUE: file \"%s\" doesn't exist", qPrintable(m_path));
        return FALSE;
    }
    DecoderFactory *df = Decoder::findByPath(m_path);
    if (!df)
    {
        qWarning("DecoderCUE: unsupported file format");
        return FALSE;
    }
    m_length = parser.length(track);
    m_offset = parser.offset(track);

    m_decoder = df->create(m_path, new QFile(m_path));
    if(!m_decoder->initialize())
    {
        qWarning("DecoderCUE: invalid audio file");
        return FALSE;
    }
    m_decoder->seek(m_offset);

    configure(m_decoder->audioParameters().sampleRate(),
              m_decoder->audioParameters().channels(),
              m_decoder->audioParameters().bits());
    offset_in_bytes = audioParameters().sampleRate() *
                      audioParameters().channels() *
                      audioParameters().bits() * m_length/8000;
    m_totalBytes = 0;

    StateHandler::instance()->dispatch(parser.info(track)->metaData());
    return TRUE;
}

qint64 DecoderCUE::totalTime()
{
    return m_decoder ? m_length : 0;
}

void DecoderCUE::seek(qint64 pos)
{
     m_decoder->seek(m_offset + pos);
     m_totalBytes = audioParameters().sampleRate() *
                    audioParameters().channels() *
                    audioParameters().bits() * pos/8000;
}

qint64 DecoderCUE::read(char *data, qint64 size)
{
    qint64 len = m_decoder->read(data, size);
    m_totalBytes += len;
    if(len > offset_in_bytes - m_totalBytes)
    {
        len = offset_in_bytes - m_totalBytes;
        int sample_size = audioParameters().bits() * audioParameters().channels()/8;
        len = (len / sample_size) * sample_size;
    }
    if(len < 0)
        len = 0;
    return len;
}

int DecoderCUE::bitrate()
{
    return m_decoder->bitrate();
}
