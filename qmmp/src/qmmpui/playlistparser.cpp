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

#include <QtGui>
#include <QObject>
#include <QList>
#include <QApplication>
#include <qmmp/qmmp.h>
#include "playlistformat.h"
#include "playlistparser.h"

QList<PlayListFormat*> *PlayListParser::m_formats = 0;

QStringList PlayListParser::filters()
{
    checkFormats();
    QStringList extensions;
    foreach(PlayListFormat *format, *m_formats)
        extensions << format->getExtensions();
    return extensions;
}

QList<PlayListFormat*> *PlayListParser::formats()
{
    checkFormats();
    return m_formats;
}

PlayListFormat *PlayListParser::findByPath(const QString &filePath)
{
    checkFormats();
    QString ext;
    if(filePath.contains("://")) //is it url?
    {
        QString p = QUrl(filePath).encodedPath();
        ext = QFileInfo(p).suffix().toLower();
    }
    else
        ext = QFileInfo(filePath).suffix().toLower();

    foreach(PlayListFormat* format, *m_formats)
    {
        if (format->hasFormat(ext))
            return format;
    }
    return 0;
}

void PlayListParser::checkFormats()
{
    if (m_formats)
        return;

    m_formats = new QList<PlayListFormat*>();
    QDir pluginsDir (Qmmp::pluginsPath());
    pluginsDir.cd("PlaylistFormats");
    foreach (QString fileName, pluginsDir.entryList(QDir::Files))
    {
        QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = loader.instance();
        if (loader.isLoaded())
            qDebug("PlayListParser: loaded plugin %s", qPrintable(fileName));
        else
            qWarning("PlayListParser: %s", qPrintable(loader.errorString ()));

        PlayListFormat *fmt = 0;
        if (plugin)
            fmt = qobject_cast<PlayListFormat *>(plugin);

        if (fmt)
            m_formats->append(fmt);
    }
}
