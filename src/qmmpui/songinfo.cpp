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
#include "songinfo.h"

SongInfo::SongInfo()
{}

SongInfo::SongInfo(const SongInfo &other)
{
    *this = other;
}

SongInfo::~SongInfo()
{}

void SongInfo::operator=(const SongInfo &info)
{
    setValue(TITLE,info.title ());
    setValue(ARTIST,info.artist ());
    setValue(ALBUM,info.album ());
    setValue(COMMENT,info.comment ());
    setValue(GENRE,info.genre ());
    setValue(YEAR,info.year ());
    setValue(TRACK,info.track ());
    setValue(LENGTH,info.length ());
    setValue(STREAM,info.isStream());
    setValue(PATH,info.path());
}

bool SongInfo::operator==(const SongInfo &info)
{
    return title() == info.title() &&
           artist() == info.artist() &&
           album() == info.album() &&
           comment() == info.comment() &&
           genre() == info.genre() &&
           track() == info.track() &&
           year() == info.year() &&
           isStream() == info.isStream() &&
           path() == info.path();
}

bool SongInfo::operator!=(const SongInfo &info)
{
    return !operator==(info);
}

void SongInfo::setValue(uint key, const QString &value)
{
    if (!value.isEmpty())
    {
        if (key == PATH)
            m_path = value;
        else
            m_strValues.insert (key, value);
    }
}

void SongInfo::setValue(uint key, const uint &value)
{
    if (value > 0)
        m_numValues.insert (key, value);
}

void SongInfo::setValue(uint key, const bool &value)
{
    if(key == STREAM)
        m_stream = value;
}

const QString SongInfo::title () const
{
    return m_strValues[TITLE];
}

const QString SongInfo::artist () const
{
    return m_strValues[ARTIST];
}

const QString SongInfo::album () const
{
    return m_strValues[ALBUM];
}

const QString SongInfo::comment () const
{
    return m_strValues[COMMENT];
}

const QString SongInfo::genre () const
{
    return m_strValues[GENRE];
}

const QString SongInfo::path () const
{
    return m_path;
}

const uint SongInfo::year () const
{
    return m_numValues[YEAR];
}

const uint SongInfo::track () const
{
    return m_numValues[TRACK];
}

const uint SongInfo::length () const
{
    return m_numValues[LENGTH];
}

const bool SongInfo::isEmpty () const
{
    return m_strValues.isEmpty();
}

void SongInfo::clear ()
{
    m_strValues.clear();
    m_numValues.clear();
    m_stream = FALSE;
}

const bool SongInfo::isStream () const
{
    return m_stream;
}

