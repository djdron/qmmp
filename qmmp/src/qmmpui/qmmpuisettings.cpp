/***************************************************************************
 *   Copyright (C) 2012-2013 by Ilya Kotov                                 *
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

#include <QSettings>
#include <QApplication>
#include <qmmp/qmmp.h>
#include "playlistmanager.h"
#include "qmmpuisettings.h"

QmmpUiSettings *QmmpUiSettings::m_instance = 0;

QmmpUiSettings::QmmpUiSettings(QObject *parent) : QObject(parent)
{
    m_instance = this;
    QSettings s (Qmmp::configFile(), QSettings::IniFormat);
    m_title_format = s.value("PlayList/title_format", "%p%if(%p&%t, - ,)%t").toString();
    m_group_format = s.value("PlayList/group_format", "%if(%p,%p -,)%if(%y, [%y],)%if(%a, %a,)").toString();
    m_convertUnderscore = s.value ("PlayList/convert_underscore", true).toBool();
    m_convertTwenty = s.value ("PlayList/convert_twenty", true).toBool();
    m_useMetadata = s.value ("PlayList/load_metadata", true).toBool();
    m_autosave_playlist = s.value("PlayList/autosave", true).toBool();
    m_resume_on_startup = s.value("General/resume_on_startup", false).toBool();
    m_restrict_filters = s.value("General/restrict_filters").toStringList();
    m_exclude_filters = s.value("General/exclude_filters", QStringList() << "*.cue").toStringList();
    m_use_default_pl = s.value("General/use_default_pl", false).toBool();
    m_default_pl_name = s.value("General/default_pl_name", tr("Playlist")).toString();
    m_use_clipboard = s.value("URLDialog/use_clipboard", false).toBool();
}

QmmpUiSettings::~QmmpUiSettings()
{
    m_instance = 0;
    sync();
}

const QString QmmpUiSettings::titleFormat() const
{
    return m_title_format;
}

const QString QmmpUiSettings::groupFormat() const
{
    return m_group_format;
}

bool QmmpUiSettings::convertUnderscore() const
{
    return m_convertUnderscore;
}

bool QmmpUiSettings::convertTwenty() const
{
    return m_convertTwenty;
}

bool QmmpUiSettings::useMetadata() const
{
    return m_useMetadata;
}

void QmmpUiSettings::setConvertUnderscore(bool yes)
{
    m_convertUnderscore = yes;
}

void  QmmpUiSettings::setConvertTwenty(bool yes)
{
    m_convertTwenty = yes;
}

void QmmpUiSettings::setTitleFormat(const QString &titleFormat)
{
    if(titleFormat != m_title_format)
    {
        m_title_format = titleFormat;
        foreach(PlayListModel *model, PlayListManager::instance()->playLists())
        {
            model->doCurrentVisibleRequest();
        }
    }
}

void QmmpUiSettings::setGroupFormat(const QString &groupFormat)
{
    if(groupFormat != m_group_format)
    {
        m_group_format = groupFormat;
        if(!PlayListManager::instance()->isGroupsEnabled())
            return;
        foreach(PlayListModel *model, PlayListManager::instance()->playLists())
        {
            model->prepareGroups(true);
        }
    }
}

void QmmpUiSettings::setUseMetadata(bool yes)
{
    m_useMetadata = yes;
}

bool QmmpUiSettings::resumeOnStartup() const
{
    return m_resume_on_startup;
}

void QmmpUiSettings::setResumeOnStartup(bool enabled)
{
    m_resume_on_startup = enabled;
}

void QmmpUiSettings::setUseClipboard(bool enabled)
{
    m_use_clipboard = enabled;
}

bool QmmpUiSettings::useClipboard() const
{
    return m_use_clipboard;
}

void QmmpUiSettings::sync()
{
    QSettings s(Qmmp::configFile(), QSettings::IniFormat);
    s.setValue("PlayList/title_format", m_title_format);
    s.setValue("PlayList/group_format", m_group_format);
    s.setValue("PlayList/convert_underscore", m_convertUnderscore);
    s.setValue("PlayList/convert_twenty", m_convertTwenty);
    s.setValue("PlayList/load_metadata", m_useMetadata);
    s.setValue("PlayList/autosave", m_autosave_playlist);
    s.setValue("General/resume_on_startup", m_resume_on_startup);
    s.setValue("General/restrict_filters", m_restrict_filters);
    s.setValue("General/exclude_filters", m_exclude_filters);
    s.setValue("General/use_default_pl", m_use_default_pl);
    s.setValue("General/default_pl_name", m_default_pl_name);
    s.setValue("URLDialog/use_clipboard", m_use_clipboard);
}

QStringList QmmpUiSettings::restrictFilters() const
{
    return m_restrict_filters;
}

void QmmpUiSettings::setRestrictFilters(const QString &filters)
{
    m_restrict_filters = filters.trimmed().split(";", QString::SkipEmptyParts);
}

QStringList QmmpUiSettings::excludeFilters() const
{
    return m_exclude_filters;
}

void QmmpUiSettings::setExcludeFilters(const QString &filters)
{
    m_exclude_filters = filters.trimmed().split(";", QString::SkipEmptyParts);
}

bool QmmpUiSettings::useDefaultPlayList() const
{
    return m_use_default_pl;
}

QString QmmpUiSettings::defaultPlayListName() const
{
    return m_default_pl_name;
}

QmmpUiSettings * QmmpUiSettings::instance()
{
    if(!m_instance)
        return new QmmpUiSettings(qApp);
    return m_instance;
}

void QmmpUiSettings::setDefaultPlayList(const QString &name, bool enabled)
{
    m_use_default_pl = enabled;
    m_default_pl_name = name;
}

void QmmpUiSettings::setAutoSavePlayList(bool enabled)
{
    m_autosave_playlist = enabled;
    PlayListManager::instance()->readSettings();
}

bool QmmpUiSettings::autoSavePlayList() const
{
    return m_autosave_playlist;
}

