/***************************************************************************
 *   Copyright (C) 2008-2014 by Ilya Kotov                                 *
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
#include <QApplication>
#include "metadataformatter.h"
#include "qmmpuisettings.h"
#include "playlisttrack.h"

PlayListTrack::PlayListTrack() : QMap<Qmmp::MetaData, QString>(), PlayListItem(), m_flag(FREE)
{
    m_settings = QmmpUiSettings::instance();
    m_length = 0;
}

PlayListTrack::PlayListTrack(const PlayListTrack &other) : QMap<Qmmp::MetaData, QString>(other),
    PlayListItem(),m_flag(FREE)
{
    m_settings = QmmpUiSettings::instance();
    m_formattedTitle = other.m_formattedTitle;
    m_group = other.m_group;
    m_formattedLength = other.m_formattedLength;
    m_titleFormat = other.m_titleFormat;
    m_groupFormat = other.m_groupFormat;
    setSelected(other.isSelected());
    setFlag(other.flag());
    m_length = other.m_length;
    m_formattedLength = other.m_formattedLength;
}

PlayListTrack::PlayListTrack(FileInfo *info) :  QMap<Qmmp::MetaData, QString>(info->metaData()),
    PlayListItem(), m_flag(FREE)
{
    m_settings = QmmpUiSettings::instance();
    setLength(m_length = info->length());
    insert(Qmmp::URL, info->path());
}

PlayListTrack::~PlayListTrack()
{}

void PlayListTrack::updateMetaData(const QMap <Qmmp::MetaData, QString> &metaData)
{
    QMap <Qmmp::MetaData, QString>::operator =(metaData);
    formatTitle();
    formatGroup();
}

void PlayListTrack::updateMetaData()
{
    QList <FileInfo *> list =  MetaDataManager::instance()->createPlayList(value(Qmmp::URL));
    if(!list.isEmpty() && !list.at(0)->path().contains("://"))
    {
        FileInfo *info = list.at(0);
        m_length = info->length();
        QMap <Qmmp::MetaData, QString>::operator =(info->metaData());
        insert(Qmmp::URL, info->path());
        formatTitle();
        formatGroup();
    }
    qDeleteAll(list);
}

const QString PlayListTrack::groupName()
{
    if(m_group.isEmpty() || m_groupFormat != m_settings->groupFormat())
    {
        m_groupFormat = m_settings->groupFormat();
        formatGroup();
    }
    return m_group;
}

bool PlayListTrack::isGroup() const
{
    return false;
}

const QString PlayListTrack::formattedTitle()
{
    if(m_formattedTitle.isEmpty() || m_titleFormat != m_settings->titleFormat())
    {
        m_titleFormat = m_settings->titleFormat();
        formatTitle();
    }
    return m_formattedTitle;
}

const QString PlayListTrack::formattedLength()
{
    if(m_length != 0 && m_formattedLength.isEmpty())
    {
        MetaDataFormatter f;
        m_formattedLength = f.formatLength(m_length);
    }
    else if(m_length == 0)
        m_formattedLength.clear();
    return m_formattedLength;
}

qint64 PlayListTrack::length() const
{
    return m_length;
}

void PlayListTrack::setLength(qint64 length)
{
    m_length = length;
    m_formattedLength.clear();
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

void PlayListTrack::formatTitle()
{
    MetaDataFormatter f(m_settings->titleFormat());
    m_formattedTitle = f.parse(this);
    if (m_formattedTitle.isEmpty())
        m_formattedTitle = value(Qmmp::URL).section('/',-1);
    if (m_formattedTitle.isEmpty())
        m_formattedTitle = value(Qmmp::URL);
    if (m_settings->convertUnderscore())
        m_formattedTitle.replace("_", " ");
    if (m_settings->convertTwenty())
        m_formattedTitle.replace("%20", " ");
}

void PlayListTrack::formatGroup()
{
    if(length() == 0 && url().contains("://"))
    {
        m_group = qApp->translate("PlayListTrack", "Streams");
        return;
    }
    MetaDataFormatter f(m_settings->groupFormat());
    m_group = f.parse(this);
    if (m_group.isEmpty())
        m_group = qApp->translate("PlayListTrack", "Empty group");
    if (m_settings->convertUnderscore())
        m_group.replace("_", " ");
    if (m_settings->convertTwenty())
        m_group.replace("%20", " ");
}
