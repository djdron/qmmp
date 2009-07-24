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

#include <QDir>
#include <QApplication>
#include <QSettings>
#include <QLocale>
#include <QByteArray>

#ifndef QMMP_STR_VERSION
#define QMMP_STR_VERSION "0.4.0"
#endif

#ifndef LIB_DIR
#define LIB_DIR "/lib"
#endif

#include "buffer.h"
#include "qmmp.h"

QString Qmmp::m_configFile;

const QString Qmmp::configFile()
{
    return m_configFile.isEmpty() ? QDir::homePath() +"/.qmmp/qmmprc" : m_configFile;
}

void Qmmp::setConfigFile(const QString &path)
{
    m_configFile = path;
}

const QString Qmmp::strVersion()
{
#ifdef SVN_REVISION
    return QString("%1-%2").arg(QMMP_STR_VERSION).arg(SVN_REVISION);
#else
    return QMMP_STR_VERSION;
#endif
}

bool Qmmp::useProxy()
{
    QSettings settings(configFile(), QSettings::IniFormat);
    return settings.value("Proxy/use_proxy", FALSE).toBool();
}

bool Qmmp::useProxyAuth()
{
    QSettings settings(configFile(), QSettings::IniFormat);
    return settings.value("Proxy/authentication", FALSE).toBool();
}

const QUrl Qmmp::proxy()
{
    QSettings settings(configFile(), QSettings::IniFormat);
    return settings.value("Proxy/url").toUrl();
}

void Qmmp::setProxyEnabled(bool yes)
{
    QSettings settings(configFile(), QSettings::IniFormat);
    settings.setValue("Proxy/use_proxy", yes);
}

void Qmmp::setProxyAuthEnabled(bool yes)
{
    QSettings settings(configFile(), QSettings::IniFormat);
    settings.setValue("Proxy/authentication", yes);
}

void Qmmp::setProxy (const QUrl &proxy)
{
    QSettings settings(configFile(), QSettings::IniFormat);
    settings.setValue("Proxy/url", proxy);
}

const QString Qmmp::pluginsPath()
{
    const char *path = qgetenv("QMMP_PLUGINS");
    if (path)
       return path;
#ifdef QMMP_INSTALL_PREFIX
    QDir dir(QMMP_INSTALL_PREFIX"/"LIB_DIR"/qmmp");
    //qDebug(QMMP_INSTALL_PREFIX"/"LIB_DIR"/qmmp");
#else
#ifndef Q_OS_WIN32
    QDir dir(qApp->applicationDirPath() + "/../"LIB_DIR"/qmmp");
#else
    QDir dir(qApp->applicationDirPath() + "/plugins");
#endif
#endif
    return dir.canonicalPath();
}

QString Qmmp::systemLanguageID()
{
#ifdef Q_OS_UNIX
    QByteArray v = qgetenv ("LC_MESSAGES");
    if (v.isEmpty())
        v = qgetenv ("LC_ALL");
    if (v.isEmpty())
        v = qgetenv ("LANG");
    if (!v.isEmpty())
        return QLocale (v).name();
#endif
    return  QLocale::system().name();
}

unsigned int Qmmp::globalBufferSize()
{
    return Buffer::size() * 128;
}

