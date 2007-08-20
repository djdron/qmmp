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
#include "filetag.h"

FileTag::FileTag()
{}

FileTag::FileTag(const FileTag &other)
{
    *this = other;
}

FileTag::~FileTag()
{}

void FileTag::operator=(const FileTag &tag)
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

void FileTag::setValue(uint name, const QString &value)
{
    if (!value.isEmpty())
        m_strValues.insert (name, value);
}

void FileTag::setValue(uint name, const uint &value)
{
    if (value > 0)
        m_numValues.insert (name, value);
}

const QString FileTag::title () const
{
    return m_strValues[TITLE];
}

const QString FileTag::artist () const
{
    return m_strValues[ARTIST];
}

const QString FileTag::album () const
{
    return m_strValues[ALBUM];
}

const QString FileTag::comment () const
{
    return m_strValues[COMMENT];
}

const QString FileTag::genre () const
{
    return m_strValues[GENRE];
}

const uint FileTag::year () const
{
    return m_numValues[YEAR];
}

const uint FileTag::track () const
{
    return m_numValues[TRACK];
}

const uint FileTag::length () const
{
    return m_numValues[LENGTH];
}

const bool FileTag::isEmpty () const
{
    return m_strValues.isEmpty();
}
