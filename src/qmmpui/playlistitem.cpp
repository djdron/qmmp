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
#include <QSettings>
#include <QDir>
#include <qmmp/metadatamanager.h>
#include "metadataformatter.h"
#include "playlistsettings.h"
#include "playlistitem.h"

PlayListItem::PlayListItem() : QMap<Qmmp::MetaData, QString>(), m_flag(FREE)
{
    m_info = 0;
    m_length = 0;
    m_selected = false;
    m_current = false;
}

PlayListItem::PlayListItem(const PlayListItem &other) : QMap<Qmmp::MetaData, QString>(other),
    m_flag(other.m_flag)
{
    m_title = other.m_title;
    if (other.m_info)
        m_info = new FileInfo(*(other.m_info));
    else
        m_info = 0;
    m_selected = other.m_selected;
    m_current = other.m_current;
    m_length = other.m_length;
}

PlayListItem::PlayListItem(FileInfo *info) :  QMap<Qmmp::MetaData, QString>(info->metaData()), m_flag(FREE)
{
    m_length = info->length();
    m_selected = false;
    m_current = false;
    m_info = info;
    insert(Qmmp::URL, m_info->path());
}

PlayListItem::~PlayListItem()
{
    if (m_info)
        delete m_info;
}

void PlayListItem::setSelected(bool yes)
{
    m_selected = yes;
}

bool PlayListItem::isSelected() const
{
    return m_selected;
}

void PlayListItem::setCurrent(bool yes)
{
    m_current = yes;
}

bool PlayListItem::isCurrent() const
{
    return m_current;
}

void PlayListItem::setFlag(FLAGS f)
{
    m_flag = f;
}

PlayListItem::FLAGS PlayListItem::flag() const
{
    return m_flag;
}

void PlayListItem::updateMetaData(const QMap <Qmmp::MetaData, QString> &metaData)
{
    QMap <Qmmp::MetaData, QString>::operator =(metaData);
    readMetadata();
}

void PlayListItem::updateTags()
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

const QString PlayListItem::text()
{
    if(m_title.isEmpty())
        readMetadata();
    return m_title;
}

void PlayListItem::setText(const QString &title)
{
    m_title = title;
}

qint64 PlayListItem::length() const
{
    return m_length;
}

void PlayListItem::setLength(qint64 length)
{
    m_length = length;
}

const QString PlayListItem::url() const
{
    return value(Qmmp::URL);
}

void PlayListItem::readMetadata()
{
    MetaDataFormatter f(PlaylistSettings::instance()->format());
    m_title = f.parse(this);
    //TODO rewrite this
    if (m_title.isEmpty())
    {
        if (value(Qmmp::URL).contains('/'))
            m_title = value(Qmmp::URL).split('/',QString::SkipEmptyParts).takeLast ();
    }
    if (m_info)
        delete m_info;
    m_info = 0;
    if (PlaylistSettings::instance()->convertUnderscore())
        m_title.replace("_", " ");
    if (PlaylistSettings::instance()->convertTwenty())
        m_title.replace("%20", " ");
}
