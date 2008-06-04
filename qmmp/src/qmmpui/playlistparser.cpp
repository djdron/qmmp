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

#include <QtGui>
#include <QObject>
#include <QList>
#include <QApplication>

#include "playlistformat.h"
#include "songinfo.h"
#include "playlistparser.h"

PlaylistParser *PlaylistParser::m_instance = 0;

PlaylistParser::PlaylistParser(QObject *parent) : QObject (parent)
{
    m_instance = this;
}

PlaylistParser::~PlaylistParser()
{}

QStringList PlaylistParser::getExtensions()
{
    loadExternalPlaylistFormats();
    QStringList extensions;
    foreach(PlaylistFormat *format, m_formats)
    extensions << format->getExtensions();
    return extensions;
}

bool PlaylistParser::supports(const QString &filePath)
{
    return findByPath(filePath) != 0;
}

QList<PlaylistFormat*> PlaylistParser::formats()
{
    loadExternalPlaylistFormats();
    return m_formats;
}

PlaylistFormat *PlaylistParser::findByPath(const QString &filePath)
{
    loadExternalPlaylistFormats();
    foreach(PlaylistFormat* format, m_formats)
    {
        if (format->hasFormat(QFileInfo(filePath).suffix().toLower()))
            return format;
    }
    return 0;
}

PlaylistParser* PlaylistParser::instance()
{
    if(!m_instance)
        qFatal("PlaylistParser: object is not created");
    return m_instance;
}

void PlaylistParser::loadExternalPlaylistFormats()
{
    if (!m_formats.isEmpty())
        return;
    QDir pluginsDir (qApp->applicationDirPath());
    pluginsDir.cdUp();
    pluginsDir.cd("./"LIB_DIR"/qmmp/PlaylistFormats");
    foreach (QString fileName, pluginsDir.entryList(QDir::Files))
    {
        QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = loader.instance();
        if (loader.isLoaded())
            qDebug("PlaylistParser: plugin loaded - %s", qPrintable(fileName));
        else
            qWarning("PlaylistParser: %s", qPrintable(loader.errorString ()));

        PlaylistFormat *fmt = 0;
        if (plugin)
            fmt = qobject_cast<PlaylistFormat *>(plugin);

        if (fmt)
            m_formats << fmt;
    }
}
