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

#include "playlistitem.h"

PlayListItem::PlayListItem() : SongInfo(), m_flag(FREE)
{
    m_tag = 0;
}

PlayListItem::PlayListItem(const QString& path) : SongInfo(), m_flag(FREE)
{
    m_selected = FALSE;
    m_current = FALSE;
    m_tag = 0;
    setValue(SongInfo::PATH, path);
    setValue(SongInfo::STREAM, path.startsWith("http://")); //TODO do this inside SongInfo
    QSettings settings ( QDir::homePath() +"/.qmmp/qmmprc", QSettings::IniFormat );
    m_use_meta = settings.value ("PlayList/load_metadata", TRUE).toBool();
    //format
    m_format = settings.value("PlayList/title_format", "%p - %t").toString();
    if (m_use_meta && !path.startsWith("http://"))
    {
        m_tag = Decoder::createTag(path);
        readMetadata();
    }
    else
        m_title = path.startsWith("http://") ? path: path.section('/',-1);

}

PlayListItem::~PlayListItem()
{
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

void PlayListItem::updateTags(const FileTag *tag)
{
    if (m_tag)
    {
        delete m_tag;
        m_tag = 0;
    }
    if (!tag->isEmpty())
        m_tag = new FileTag(*tag);
    readMetadata();
}

void PlayListItem::updateTags()
{
    if (path().startsWith("http://"))
        return;
    if (m_tag)
    {
        delete m_tag;
        m_tag = 0;
    }
    m_tag = Decoder::createTag(path());
    readMetadata();
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
    //clear();
    m_title.clear();
    if(m_tag) //read length first
        setValue(SongInfo::LENGTH, m_tag->length());
    if (m_use_meta && m_tag && !m_tag->isEmpty())
    {
        //fill SongInfo
        setValue(SongInfo::TITLE,  m_tag->title());
        setValue(SongInfo::ARTIST, m_tag->artist());
        setValue(SongInfo::ALBUM, m_tag->album());
        setValue(SongInfo::COMMENT, m_tag->comment());
        setValue(SongInfo::GENRE, m_tag->genre());
        setValue(SongInfo::YEAR, m_tag->year());
        setValue(SongInfo::TRACK, m_tag->track());
        //generate playlist string
        m_title = m_format;
        m_title = printTag(m_title, "%p", artist());
        m_title = printTag(m_title, "%a", album());
        m_title = printTag(m_title, "%t", title());
        m_title = printTag(m_title, "%n", QString("%1").arg(track()));
        m_title = printTag(m_title, "%g", genre());
        m_title = printTag(m_title, "%f", path().section('/',-1));
        m_title = printTag(m_title, "%F", path());
        m_title = printTag(m_title, "%y", QString("%1").arg(year ()));
    }
    if (m_title.isEmpty())
        m_title = path().startsWith("http://") ? path(): path().section('/',-1);
    if(m_tag)
        delete m_tag;
    m_tag = 0;
}

QString PlayListItem::printTag(QString str, QString regExp, QString tagStr)
{
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
            regExpPos = m_format.lastIndexOf(regExp);
            nextPos = m_format.lastIndexOf("%", regExpPos - 1);
            QString lastSep = m_format.right (m_format.size() - nextPos - 2);
            str.remove(lastSep);
            str.remove(regExp);
        }
        else
            str.remove ( regExpPos, nextPos - regExpPos);
    }
    return str;
}

