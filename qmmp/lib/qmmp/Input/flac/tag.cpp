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
#include "tag.h"

Tag::Tag(const QString &source)
        : FileTag()
{
    m_length = 0;
    m_year = 0;
    m_track = 0;
    m_empty = TRUE;
    TagLib::FileRef fileRef(source.toLocal8Bit ());

    m_tag = fileRef.tag();
    if(m_tag && !m_tag->isEmpty())
    {
        m_album = QString::fromUtf8(m_tag->album().toCString(TRUE)).trimmed();
        m_artist = QString::fromUtf8(m_tag->artist().toCString(TRUE)).trimmed();
        m_comment = QString::fromUtf8(m_tag->comment().toCString(TRUE)).trimmed();
        m_genre = QString::fromUtf8(m_tag->genre().toCString(TRUE)).trimmed();
        m_title = QString::fromUtf8(m_tag->title().toCString(TRUE)).trimmed();
        m_year = 0;
        m_track = 0;
        m_empty = FALSE;
    }
    else
        m_tag = 0;
    if(fileRef.audioProperties())
            m_length = fileRef.audioProperties()->length();
}


Tag::~Tag()
{}


bool Tag::isEmpty()
{
    return m_empty;
}

QString Tag::album()
{
    return m_album;
}

QString Tag::artist()
{
    return m_artist;
}

QString Tag::comment()
{
    return m_comment;
}

QString Tag::genre()
{
    return m_genre;
}

QString Tag::title()
{
    return m_title;
}

uint Tag::length()
{
    return m_length;
}

uint Tag::track()
{
    return m_track; 
}

uint Tag::year()
{
    return m_year;
}
