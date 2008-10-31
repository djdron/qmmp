/***************************************************************************
 *   Copyright (C) 2008 by Ilya Kotov                                      *
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
#ifndef AACFILE_H
#define AACFILE_H


#include <QMap>
#include <QString>

#include <taglib/tag.h>
#include <taglib/fileref.h>
#include <taglib/id3v1tag.h>
#include <taglib/id3v2tag.h>
#include <taglib/id3v2header.h>

#include <qmmp/qmmp.h>

class QIODevice;
class TagExtractor;
class QBuffer;

/**
    @author Ilya Kotov <forkotov02@hotmail.ru>
*/
class AACFile
{
public:
    AACFile(QIODevice *i, bool metaData = TRUE);

    ~AACFile();

    qint64 length();
    quint32 bitrate();
    bool isValid();
    const QMap<Qmmp::MetaData, QString> metaData();

private:
    void parseADTS();
    void parseID3v2();
    qint64 m_length;
    quint32 m_bitrate;
    QIODevice *m_input;
    bool m_isValid;
    QMap<Qmmp::MetaData, QString> m_metaData;
};

class ID3v2Tag : public TagLib::ID3v2::Tag
{
public:
    ID3v2Tag(QByteArray *array, long offset);

protected:
    void read ();

private:
    QBuffer *m_buf;
    long m_offset;
};

#endif
