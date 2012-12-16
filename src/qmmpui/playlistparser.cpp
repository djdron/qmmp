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
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#include <QtGui>
#include <QObject>
#include <QList>
#include <QApplication>
#include <qmmp/qmmp.h>
#include "playlistformat.h"
#include "playlistparser.h"

PlayListParser *PlayListParser::m_instance = 0;

PlayListParser::PlayListParser(QObject *parent) : QObject (parent)
{
    m_instance = this;
}

PlayListParser::~PlayListParser()
{
    m_instance = 0;
}

QStringList PlayListParser::getExtensions()
{
    loadExternalPlaylistFormats();
    QStringList extensions;
    foreach(PlayListFormat *format, m_formats)
    extensions << format->getExtensions();
    return extensions;
}

bool PlayListParser::supports(const QString &filePath)
{
    return findByPath(filePath) != 0;
}

QList<PlayListFormat*> PlayListParser::formats()
{
    loadExternalPlaylistFormats();
    return m_formats;
}

PlayListFormat *PlayListParser::findByPath(const QString &filePath)
{
    loadExternalPlaylistFormats();
    QString ext;
    if(filePath.contains("://")) //is it url?
    {
        QString p = QUrl(filePath).encodedPath();
        ext = QFileInfo(p).suffix().toLower();
    }
    else
        ext = QFileInfo(filePath).suffix().toLower();

    foreach(PlayListFormat* format, m_formats)
    {
        if (format->hasFormat(ext))
            return format;
    }
    return 0;
}

PlayListParser* PlayListParser::instance()
{
    if(!m_instance)
        qFatal("PlaylistParser: object is not created");
    return m_instance;
}

void PlayListParser::loadExternalPlaylistFormats()
{
    if (!m_formats.isEmpty())
        return;
    QDir pluginsDir (Qmmp::pluginsPath());
    pluginsDir.cd("PlaylistFormats");
    foreach (QString fileName, pluginsDir.entryList(QDir::Files))
    {
        QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = loader.instance();
        if (loader.isLoaded())
            qDebug("PlaylistParser: loaded plugin %s", qPrintable(fileName));
        else
            qWarning("PlaylistParser: %s", qPrintable(loader.errorString ()));

        PlayListFormat *fmt = 0;
        if (plugin)
            fmt = qobject_cast<PlayListFormat *>(plugin);

        if (fmt)
            m_formats << fmt;
    }
}
