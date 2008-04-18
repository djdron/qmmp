/***************************************************************************
 *   Copyright (C) 2006-2008 by Ilya Kotov                                 *
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

#include "mediafile.h"

MediaFile::MediaFile(const QString& path) : m_flag(FREE)
{
    m_selected = FALSE;
    m_current = FALSE;
    m_path = path;
    m_tag = 0;
    m_year = 0;
    m_track = 0;

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
        m_title = m_path.startsWith("http://") ? m_path: m_path.section('/',-1);
}


MediaFile::~MediaFile()
{
    if (m_tag)
        delete m_tag;
}

const QString MediaFile::path()const
{
    return m_path;
}
const QString MediaFile::fileName() const
{
    return m_path.section('/',-1);
}

const QString MediaFile::title() const
{
    return m_title;
}

int MediaFile::length() const
{
    if (m_tag)
        return m_tag->length();
    else
        return 0;
}

void MediaFile::setSelected(bool yes)
{
    m_selected = yes;
}

bool MediaFile::isSelected() const
{
    return m_selected;
}

uint MediaFile::year() const
{
    return m_year;
}

uint MediaFile::track() const
{
    return m_track;
}

bool MediaFile::isCurrent()
{
    return m_current;
}

void MediaFile::setCurrent(bool cur)
{
    m_current = cur;
}

void MediaFile::updateTags(const FileTag *tag)
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

void MediaFile::updateTags()
{
    if (m_path.startsWith("http://"))
        return;
    if (m_tag)
    {
        delete m_tag;
        m_tag = 0;
    }
    m_tag = Decoder::createTag(m_path);
    readMetadata();
}

void MediaFile::readMetadata()
{
    m_title.clear();
    if (m_use_meta && m_tag && !m_tag->isEmpty())
    {
        m_year = m_tag->year();
        m_track = m_tag->track();
        m_title = m_format;
        m_title = printTag(m_title, "%p", m_tag->artist());
        m_title = printTag(m_title, "%a", m_tag->album());
        m_title = printTag(m_title, "%t", m_tag->title());
        m_title = printTag(m_title, "%n", QString("%1").arg(m_tag->track()));
        m_title = printTag(m_title, "%g", m_tag->genre());
        m_title = printTag(m_title, "%f", m_path.section('/',-1));
        m_title = printTag(m_title, "%F", m_path);
        m_title = printTag(m_title, "%y", QString("%1").arg(m_tag->year ()));
    }
    if (m_title.isEmpty())
        m_title = m_path.startsWith("http://") ? m_path: m_path.section('/',-1);
}

void MediaFile::changeTitle(const QString &newtitle)
{
    m_title = newtitle;
}

void MediaFile::setFlag(FLAGS f)
{
    m_flag = f;
}


MediaFile::FLAGS MediaFile::flag() const
{
    return m_flag;
}

FileTag *MediaFile::tag()
{
    if (m_tag && m_tag->isEmpty())
        return 0;
    return m_tag;
}

QString MediaFile::printTag(QString str, QString regExp, QString tagStr)
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

