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


#include <QString>
#include <QTextCodec>
#include <QSettings>
#include <QDir>
/*#include <filetag.h>
#include <taglib/tag.h>
#include <taglib/fileref.h>
#include <taglib/id3v1tag.h>
#include <taglib/id3v2tag.h>
#include <taglib/tfile.h>
#include <taglib/mpegfile.h>*/

#include "id3tag.h"

ID3Tag::ID3Tag(const QString &source): FileTag(), m_tag(0)
{
    m_length = 0;
    m_year = 0;
    m_track = 0;
    m_empty = TRUE;
    TagLib::MPEG::File fileRef(source.toLocal8Bit ());

    QSettings settings(QDir::homePath()+"/.qmmp/qmmprc", QSettings::IniFormat);
    settings.beginGroup("MAD");
    QTextCodec *codec_v1 =
        QTextCodec::codecForName(settings.value("ID3v1_encoding","UTF-8" )
                                 .toByteArray ());
    QTextCodec *codec_v2 =
        QTextCodec::codecForName(settings.value("ID3v2_encoding","UTF-8" )
                                 .toByteArray ());
    if(!codec_v1)
        codec_v1 = QTextCodec::codecForName ("UTF-8");
    if(!codec_v2)
        codec_v2 = QTextCodec::codecForName ("UTF-8");

    QTextCodec *codec = 0;

    int ver = settings.value("ID3_version", 2).toInt();
    if (ver == 1 && settings.value("ID3v1_enable", TRUE).toBool() &&
            fileRef.ID3v1Tag())
    {
        m_tag = fileRef.ID3v1Tag();
        codec = codec_v1;
        if(m_tag->isEmpty())
        {
            m_tag = 0;
            if (settings.value("ID3v2_enable", TRUE).toBool() &&
                    fileRef.ID3v2Tag())
            {
                if(!fileRef.ID3v2Tag()->isEmpty())
                {
                    m_tag = fileRef.ID3v2Tag();
                    codec = codec_v2;
                }
            }
        }
    }
    else
        ver = 2;
    if (ver == 2 && settings.value("ID3v2_enable", TRUE).toBool() &&
            fileRef.ID3v2Tag())
    {
        m_tag = fileRef.ID3v2Tag();
        codec = codec_v2;
        if(m_tag->isEmpty())
        {
            m_tag = 0;
            if (settings.value("ID3v1_enable", TRUE).toBool() &&
                    fileRef.ID3v1Tag())
            {
                if(fileRef.ID3v1Tag()->isEmpty())
                {
                    m_tag = fileRef.ID3v1Tag();
                    codec = codec_v1;
                }
            }
        }
    }
    settings.endGroup();

    if (m_tag && codec)
    {
        bool utf = codec->name ().contains("UTF");
        TagLib::String album = m_tag->album();
        TagLib::String artist = m_tag->artist();
        TagLib::String comment = m_tag->comment();
        TagLib::String genre = m_tag->genre();
        TagLib::String title = m_tag->title();

        m_album = codec->toUnicode(album.toCString(utf)).trimmed();
        m_artist = codec->toUnicode(artist.toCString(utf)).trimmed();
        m_comment = codec->toUnicode(comment.toCString(utf)).trimmed();
        m_genre = codec->toUnicode(genre.toCString(utf)).trimmed();
        m_title = codec->toUnicode(title.toCString(utf)).trimmed();
        m_year = m_tag->year();
        m_track = m_tag->track();
        m_empty = FALSE;
    }
    if(fileRef.audioProperties())
            m_length = fileRef.audioProperties()->length();
}


ID3Tag::~ID3Tag()
{}


uint ID3Tag::length()
{
    return m_length;
}

uint ID3Tag::track()
{
    return m_track;
}

uint ID3Tag::year()
{
    return m_year;
}

QString ID3Tag::album()
{
    return m_album;
}

QString ID3Tag::artist()
{
    return m_artist;
}

QString ID3Tag::comment()
{
    return m_comment;
}

QString ID3Tag::genre()
{
    return m_genre;
}

QString ID3Tag::title()
{
    return m_title;
}

bool ID3Tag::isEmpty ()
{
    return m_empty;
}
