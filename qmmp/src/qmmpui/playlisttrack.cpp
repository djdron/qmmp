/***************************************************************************
 *   Copyright (C) 2008-2013 by Ilya Kotov                                 *
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
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/
#include <qmmp/metadatamanager.h>
#include "metadataformatter.h"
#include "qmmpuisettings.h"
#include "playlisttrack.h"

PlayListTrack::PlayListTrack() : QMap<Qmmp::MetaData, QString>(), PlayListItem(), m_flag(FREE)
{
    m_info = 0;
    m_length = 0;
}

PlayListTrack::PlayListTrack(const PlayListTrack &other) : QMap<Qmmp::MetaData, QString>(other),
    PlayListItem(),m_flag(FREE)
{
    m_formattedTitle = other.m_formattedTitle;
    if (other.m_info)
        m_info = new FileInfo(*(other.m_info));
    else
        m_info = 0;
    setSelected(other.isSelected());
    setFlag(other.flag());
    m_length = other.m_length;
    m_formattedLength = other.m_formattedLength;
}

PlayListTrack::PlayListTrack(FileInfo *info) :  QMap<Qmmp::MetaData, QString>(info->metaData()),
    PlayListItem(), m_flag(FREE)
{
    setLength(m_length = info->length());
    m_info = info;
    insert(Qmmp::URL, m_info->path());
}

PlayListTrack::~PlayListTrack()
{
    if (m_info)
        delete m_info;
}

void PlayListTrack::updateMetaData(const QMap <Qmmp::MetaData, QString> &metaData)
{
    QMap <Qmmp::MetaData, QString>::operator =(metaData);
    readMetadata();
}

void PlayListTrack::updateMetaData()
{
    if (m_info)
    {
        delete m_info;
        m_info = 0;
    }
    QList <FileInfo *> list =  MetaDataManager::instance()->createPlayList(value(Qmmp::URL));
    if(!list.isEmpty() && !list.at(0)->path().contains("://"))
    {
        m_info = list.at(0);
        m_length = m_info->length();
        QMap <Qmmp::MetaData, QString>::operator =(m_info->metaData());
        insert(Qmmp::URL, m_info->path());
        readMetadata();
    }
    while(list.size() > 1)
        delete list.takeLast();
}

const QString PlayListTrack::groupName() const
{
    MetaDataFormatter f("%p - %a");
    return f.parse(this);
}

bool PlayListTrack::isGroup() const
{
    return false;
}

const QString PlayListTrack::formattedTitle()
{
    if(m_formattedTitle.isEmpty())
        readMetadata();
    return m_formattedTitle;
}

const QString PlayListTrack::formattedLength()
{
    return m_formattedLength;
}

qint64 PlayListTrack::length() const
{
    return m_length;
}

void PlayListTrack::setLength(qint64 length)
{
    m_length = length;
    MetaDataFormatter f;
    m_formattedLength = f.formatLength(m_length);
}

const QString PlayListTrack::url() const
{
    return value(Qmmp::URL);
}

void PlayListTrack::setFlag(FLAGS f)
{
    m_flag = f;
}

PlayListTrack::FLAGS PlayListTrack::flag() const
{
    return m_flag;
}

void PlayListTrack::readMetadata()
{
    MetaDataFormatter f(QmmpUiSettings::instance()->format());
    m_formattedTitle = f.parse(this);
    if (m_formattedTitle.isEmpty())
        m_formattedTitle = value(Qmmp::URL).section('/',-1);
    if (m_info)
        delete m_info;
    m_info = 0;
    if (QmmpUiSettings::instance()->convertUnderscore())
        m_formattedTitle.replace("_", " ");
    if (QmmpUiSettings::instance()->convertTwenty())
        m_formattedTitle.replace("%20", " ");
}
