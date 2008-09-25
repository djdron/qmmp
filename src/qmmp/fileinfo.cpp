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
#include "fileinfo.h"

FileInfo::FileInfo()
{
    m_length = 0;
}

/*FileInfo::FileInfo(const FileInfo &other)
{
    *this = other;
}*/

FileInfo::~FileInfo()
{}

/*void FileInfo::operator=(const FileInfo &tag)
{
    setValue(TITLE,tag.title ());
    setValue(ARTIST,tag.artist ());
    setValue(ALBUM,tag.album ());
    setValue(COMMENT,tag.comment ());
    setValue(GENRE,tag.genre ());
    setValue(YEAR,tag.year ());
    setValue(TRACK,tag.track ());
    setValue(LENGTH,tag.length ());
}

bool FileInfo::operator==(const FileInfo &tag)
{
    return title() == tag.title() &&
           artist() == tag.artist() &&
           album() == tag.album() &&
           comment() == tag.comment() &&
           genre() == tag.genre() &&
           year() == tag.year() &&
           track() == tag.track() &&
           length() == tag.length() &&
           isEmpty() == tag.isEmpty();
}

bool FileInfo::operator!=(const FileInfo &tag)
{
    return !operator==(tag);
}*/

const qint64 FileInfo::length () const
{
    return m_length;
}

const QString FileInfo::metaData (Qmmp::MetaData key) const
{
    return m_metaData[key];
}

bool FileInfo::isEmpty()
{
    return m_metaData.isEmpty(); //TODO add correct test
}

void FileInfo::setLength(qint64 length)
{
    m_length = length;
}

void FileInfo::setMetaData(Qmmp::MetaData key, const QString &value)
{
    m_metaData.insert(key, value);
}

void FileInfo::setMetaData(Qmmp::MetaData key, int value)
{
    m_metaData.insert(key, QString::number(value));
}
