/***************************************************************************
 *   Copyright (C) 2006-2010 by Ilya Kotov                                 *
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

#include <qmmp/metadatamanager.h>
#include "fileloader.h"
#include "playlistsettings.h"
#include "playlistitem.h"

FileLoader::FileLoader(QObject *parent) : QThread(parent)
{
    m_filters = MetaDataManager::instance()->nameFilters();
    m_finished = false;
}


FileLoader::~FileLoader()
{
    qWarning("FileLoader::~FileLoader()");
}


void FileLoader::addFile(const QString &path)
{
    bool use_meta = PlaylistSettings::instance()->useMetadata();
    QList <FileInfo *> playList = MetaDataManager::instance()->createPlayList(path, use_meta);
    foreach(FileInfo *info, playList)
        emit newPlayListItem(new PlayListItem(info));
}


void FileLoader::addDirectory(const QString& s)
{
    QList <FileInfo *> playList;
    QDir dir(s);
    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    dir.setSorting(QDir::Name);
    QFileInfoList l = dir.entryInfoList(m_filters);
    foreach(QFileInfo info, l)
    {
        addFile(info.absoluteFilePath ());
        if (m_finished) return;
    }
    dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    dir.setSorting(QDir::Name);
    l.clear();
    l = dir.entryInfoList();
    if (l.size() > 0)
        for (int i = 0; i < l.size(); ++i)
        {
            QFileInfo fileInfo = l.at(i);
            addDirectory(fileInfo.absoluteFilePath ());
            if (m_finished) return;
        }
}

void FileLoader::run()
{
    m_finished = false;
    while(!m_files.isEmpty() || !m_directories.isEmpty())
    {
        if(!m_files.isEmpty())
        {
            addFile(m_files.dequeue());
            continue;
        }
        if(!m_directories.isEmpty())
        {
            addDirectory(m_directories.dequeue());
            continue;
        }
    }
}

void FileLoader::loadFile(const QString &path)
{
    m_files.enqueue(path);
    start(QThread::IdlePriority);
}

void FileLoader::loadFiles(const QStringList &paths)
{
    m_files << paths;
    start(QThread::IdlePriority);
}

void FileLoader::loadDirectory(const QString &path)
{
    m_directories.enqueue(path);
    start(QThread::IdlePriority);
}

void FileLoader::finish()
{
    m_finished = true;
    m_files.clear();
    m_directories.clear();
    wait();
}
