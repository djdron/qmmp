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
#include <QSettings>
#include <QDir>

#include <qmmp/decoder.h>
#include "playlistsettings.h"
#include "playlistitem.h"

PlayListItem::PlayListItem() : AbstractPlaylistItem(), m_flag(FREE)
{
    m_info = 0;
}

PlayListItem::PlayListItem(FileInfo *info) : AbstractPlaylistItem(), m_flag(FREE)
{
    m_selected = FALSE;
    m_current = FALSE;
    m_info = info;

    setMetaData(info->metaData());
    setMetaData(Qmmp::URL, m_info->path());
    setLength(m_info->length());
    readMetadata();
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
    setMetaData(metaData);
    readMetadata();
}

void PlayListItem::updateTags()
{
    if (m_info)
    {
        delete m_info;
        m_info = 0;
    }
    QList <FileInfo *> list =  Decoder::createPlayList(url());
    if(!list.isEmpty())
    {
        m_info = list.at(0);
        setMetaData(m_info->metaData());
        setMetaData(Qmmp::URL, m_info->path());
        readMetadata();
    }
    while(list.size() > 1)
        delete list.takeLast();
}

const QString PlayListItem::text() const
{
    return m_title;
}

void PlayListItem::setText(const QString &title)
{
    m_title = title;
}

void PlayListItem::readMetadata()
{
    m_title = PlaylistSettings::instance()->format();
    m_title = printTag(m_title, "%p", artist());
    m_title = printTag(m_title, "%a", album());
    m_title = printTag(m_title, "%t", title());
    m_title = printTag(m_title, "%n", track());
    m_title = printTag(m_title, "%c", comment());
    m_title = printTag(m_title, "%C", composer());
    m_title = printTag(m_title, "%g", genre());
    m_title = printTag(m_title, "%D", discNumber());
    m_title = printTag(m_title, "%f", url().section('/',-1));
    m_title = printTag(m_title, "%F", url());
    m_title = printTag(m_title, "%y", year ());
    //TODO rewrite this
    if (m_title.isEmpty())
    {
        if (url().contains('/'))
            m_title = url().split('/',QString::SkipEmptyParts).takeLast ();
    }
    if (m_info)
        delete m_info;
    m_info = 0;
    if (PlaylistSettings::instance()->convertUnderscore())
        m_title.replace("_", " ");
    if (PlaylistSettings::instance()->convertTwenty())
        m_title.replace("%20", " ");
}

QString PlayListItem::printTag(QString str, QString regExp, QString tagStr)
{
    QString format = PlaylistSettings::instance()->format();
    if (!tagStr.isEmpty())
        str.replace(regExp, tagStr);
    else
    {
        //remove unused separators
        int regExpPos = str.indexOf(regExp);
        if (regExpPos < 0)
            return str;
        int nextPos = str.indexOf("%", regExpPos + 1);
        if (nextPos < 0)
        {
            //last separator
            regExpPos = format.lastIndexOf(regExp);
            nextPos = format.lastIndexOf("%", regExpPos - 1);
            QString lastSep = format.right (format.size() - nextPos - 2);
            str.remove(lastSep);
            str.remove(regExp);
        }
        else
            str.remove ( regExpPos, nextPos - regExpPos);
    }
    return str;
}
