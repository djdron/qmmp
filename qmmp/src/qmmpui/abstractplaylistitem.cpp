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
#include "abstractplaylistitem.h"

AbstractPlaylistItem::AbstractPlaylistItem()
{
    m_length = 0;
}


AbstractPlaylistItem::~AbstractPlaylistItem()
{
}

const QString AbstractPlaylistItem::title () const
{
    return m_metaData.value(Qmmp::TITLE);
}

const QString AbstractPlaylistItem::artist () const
{
    return m_metaData.value(Qmmp::ARTIST);
}

const QString AbstractPlaylistItem::album () const
{
    return m_metaData.value(Qmmp::ALBUM);
}

const QString AbstractPlaylistItem::comment () const
{
    return m_metaData.value(Qmmp::COMMENT);
}

const QString AbstractPlaylistItem::genre () const
{
    return m_metaData.value(Qmmp::GENRE);
}

const QString AbstractPlaylistItem::composer() const
{
    return m_metaData.value(Qmmp::COMPOSER);
}

const QString AbstractPlaylistItem::track () const
{
    return m_metaData.value(Qmmp::TRACK);
}

const QString AbstractPlaylistItem::year () const
{
    return m_metaData.value(Qmmp::YEAR);
}

const QString AbstractPlaylistItem::discNumber () const
{
    return m_metaData.value(Qmmp::DISCNUMBER);
}

const QString AbstractPlaylistItem::url () const
{
    return m_metaData.value(Qmmp::URL);
}

qint64 AbstractPlaylistItem::length ()
{
    return m_length;
}

bool AbstractPlaylistItem::isEmpty()
{
    return m_metaData.isEmpty();
}

void AbstractPlaylistItem::clear()
{
    m_metaData.clear();
    m_length = 0;
}

void AbstractPlaylistItem::setMetaData(const QMap <Qmmp::MetaData, QString> &metaData)
{
    m_metaData = metaData;
}

void AbstractPlaylistItem::setMetaData(Qmmp::MetaData key, const QString &value)
{
    m_metaData.insert(key, value);
}

void AbstractPlaylistItem::setLength(qint64 length)
{
    m_length = length;
}

const QMap <Qmmp::MetaData, QString> AbstractPlaylistItem::metaData()
{
    return m_metaData;
}
