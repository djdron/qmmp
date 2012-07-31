/***************************************************************************
 *   Copyright (C) 2012 by Ilya Kotov                                      *
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
    m_format = s.value("PlayList/title_format", "%p%if(%p&%t, - ,)%t").toString();
    m_convertUnderscore = s.value ("PlayList/convert_underscore", true).toBool();
    m_convertTwenty = s.value ("PlayList/convert_twenty", true).toBool();
    m_useMetadata = s.value ("PlayList/load_metadata", true).toBool();
    m_resume_on_startup = s.value("General/resume_on_startup", false).toBool();
    m_restrict_filters = s.value("General/restrict_filters").toStringList();
    m_exclude_filters = s.value("General/exclude_filters", QStringList() << "*.cue").toStringList();
    m_use_default_pl = s.value("General/use_default_pl", false).toBool();
    m_default_pl_name = s.value("General/default_pl_name", tr("Playlist")).toString();
}

QmmpUiSettings::~QmmpUiSettings()
{
    m_instance = 0;
    sync();
}

const QString QmmpUiSettings::format() const
{
    return m_format;
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

void QmmpUiSettings::setFormat(const QString &format)
{
    m_format = format;
    if(format != m_format)
    {
        m_format = format;
        //emit settingsChanged();
        foreach(PlayListModel *model, PlayListManager::instance()->playLists())
        {
            foreach(PlayListItem *item, model->items())
                item->setText(QString());
            model->doCurrentVisibleRequest();
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

void QmmpUiSettings::sync()
{
    QSettings s(Qmmp::configFile(), QSettings::IniFormat);
    s.setValue("PlayList/title_format", m_format);
    s.setValue("PlayList/convert_underscore", m_convertUnderscore);
    s.setValue("PlayList/convert_twenty", m_convertTwenty);
    s.setValue("PlayList/load_metadata", m_useMetadata);
    s.setValue("General/resume_on_startup", m_resume_on_startup);
    s.setValue("General/restrict_filters", m_restrict_filters);
    s.setValue("General/exclude_filters", m_exclude_filters);
    s.setValue("General/use_default_pl", m_use_default_pl);
    s.setValue("General/default_pl_name", m_default_pl_name);
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
