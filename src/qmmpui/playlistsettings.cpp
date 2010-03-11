/***************************************************************************
 *   Copyright (C) 2009 by Ilya Kotov                                      *
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
#include <qmmp/qmmp.h>
#include "playlistsettings.h"

PlaylistSettings *PlaylistSettings::m_instance = 0;

PlaylistSettings::PlaylistSettings()
{
    QSettings s (Qmmp::configFile(), QSettings::IniFormat);
    m_format = s.value("PlayList/title_format", "%p%if(%p&%t, - ,)%t").toString();
    m_convertUnderscore = s.value ("PlayList/convert_underscore", true).toBool();
    m_convertTwenty = s.value ("PlayList/convert_twenty", true).toBool();
    m_useMetadata = s.value ("PlayList/load_metadata", true).toBool();
}

PlaylistSettings::~PlaylistSettings()
{
    m_instance = 0;
    QSettings s(Qmmp::configFile(), QSettings::IniFormat);
    s.setValue("PlayList/title_format", m_format);
    s.setValue("PlayList/convert_underscore", m_convertUnderscore);
    s.setValue("PlayList/convert_twenty", m_convertTwenty);
    s.setValue("PlayList/load_metadata", m_useMetadata);
}

PlaylistSettings *PlaylistSettings::instance()
{
    if (!m_instance)
        m_instance = new PlaylistSettings();
    return m_instance;
}

const QString PlaylistSettings::format() const
{
    return m_format;
}

bool PlaylistSettings::convertUnderscore()
{
    return m_convertUnderscore;
}

bool PlaylistSettings::convertTwenty()
{
    return m_convertTwenty;
}

bool PlaylistSettings::useMetadata()
{
    return m_useMetadata;
}

void PlaylistSettings::setConvertUnderscore(bool yes)
{
    m_convertUnderscore = yes;
}

void  PlaylistSettings::setConvertTwenty(bool yes)
{
    m_convertTwenty = yes;
}

void PlaylistSettings::setFormat(const QString &format)
{
    m_format = format;
}

void PlaylistSettings::setUseMetadata(bool yes)
{
    m_useMetadata = yes;
}
