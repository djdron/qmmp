/***************************************************************************
 *   Copyright (C) 2008-2012 by Ilya Kotov                                 *
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

#include <QDir>
#include <QApplication>
#include <QSettings>
#include <QLocale>
#include <QByteArray>

#ifndef LIB_DIR
#define LIB_DIR "/lib"
#endif

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
    QString ver = QString("%1.%2.%3")
            .arg(QMMP_VERSION_MAJOR)
            .arg(QMMP_VERSION_MINOR)
            .arg(QMMP_VERSION_PATCH);
#if !QMMP_VERSION_STABLE
#ifdef SVN_REVISION
    ver += "-"SVN_REVISION;
#else
    ver += "-svn";
#endif
#endif
    return ver;
}

const QString Qmmp::pluginsPath()
{
    QByteArray path = qgetenv("QMMP_PLUGINS");
    if (!path.isEmpty())
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
    QByteArray v = qgetenv ("LC_ALL");
    if (v.isEmpty())
        v = qgetenv ("LC_MESSAGES");
    if (v.isEmpty())
        v = qgetenv ("LANG");
    if (!v.isEmpty())
        return QLocale (v).name();
#endif
    return  QLocale::system().name();
}
