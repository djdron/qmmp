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
#include <QSettings>
#include <QDir>

#include <decoder.h>

#include "mediafile.h"

MediaFile::MediaFile(QString path)
{
    m_selected = FALSE;
    m_current = FALSE;
    m_path = path;
    m_tag = Decoder::createTag(path);
    //format
    QSettings settings ( QDir::homePath() +"/.qmmp/qmmprc", QSettings::IniFormat );
    QString format = settings.value("PlayList/title_format", "%p - %t").toString();
    bool use_meta = settings.value ("PlayList/load_metadata", TRUE).toBool();
    if (use_meta && m_tag && !m_tag->isEmpty())
    {
        m_year = m_tag->year();
        //m_title = m_tag->artist()+" - "+m_tag->title();
        m_title = format;
        m_title.replace("%p",m_tag->artist());
        m_title.replace("%a",m_tag->album());
        m_title.replace("%t",m_tag->title());
        m_title.replace("%n",QString("%1").arg(m_tag->track()));
        m_title.replace("%g",m_tag->genre ());
        m_title.replace("%f",m_path.section('/',-1));
        m_title.replace("%F",m_path);
        //m_title.replace("%d",);
        m_title.replace("%y",QString("%1").arg(m_tag->year ()));
        //m_title.replace("%c",);
    }
    else
        m_title = m_path.section('/',-1);
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

const QString MediaFile::title()const
{
    return m_title;
}

int MediaFile::length()const
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

bool MediaFile::isSelected()const
{
    return m_selected;
}

uint MediaFile::year()const
{
    return m_year;
}

bool MediaFile::isCurrent()
{
    return m_current;
}

void MediaFile::setCurrent(bool cur)
{
    m_current = cur;
}
