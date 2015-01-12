/***************************************************************************
 *   Copyright (C) 2006-2015 by Ilya Kotov                                 *
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

#include <qmmp/metadatamanager.h>
#include <QRegExp>
#include "fileloader_p.h"
#include "qmmpuisettings.h"
#include "playlistitem.h"
#include "playlisttrack.h"

FileLoader::FileLoader(QObject *parent) : QThread(parent)
{
    m_settings = QmmpUiSettings::instance();
    m_finished = false;
}

FileLoader::~FileLoader()
{}

void FileLoader::addFile(const QString &path, PlayListItem *before)
{
    bool use_meta = m_settings->useMetadata();
    QList <FileInfo *> playList = MetaDataManager::instance()->createPlayList(path, use_meta);
    if(before)
    {
        foreach(FileInfo *info, playList)
            emit newTrackToInsert(before, new PlayListTrack(info));
    }
    else
    {
        foreach(FileInfo *info, playList)
            emit newTrackToAdd(new PlayListTrack(info));
    }
    qDeleteAll(playList);
}

void FileLoader::addDirectory(const QString& s, PlayListItem *before)
{
    QDir dir(s);
    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    dir.setSorting(QDir::Name);
    QFileInfoList l = dir.entryInfoList(m_filters);
    foreach(QFileInfo info, l)
    {
        if(checkRestrictFilters(info) && checkExcludeFilters(info))
            addFile(info.absoluteFilePath (), before);
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
            addDirectory(fileInfo.absoluteFilePath (), before);
            if (m_finished) return;
        }
}

void FileLoader::run()
{
    m_finished = false;
    while(!m_tasks.isEmpty() && !m_finished)
    {
        LoaderTask i = m_tasks.dequeue();
        PlayListItem *before = i.before;
        QString path = i.path;

        QFileInfo info(path);

        if(info.isDir())
        {
            addDirectory(path, before);
            continue;
        }
        else if(info.isFile() || path.contains("://"))
        {
            addFile(path, before);
            continue;
        }
    }
}

void FileLoader::add(const QString &path)
{
    add(QStringList() << path);
}

void FileLoader::add(const QStringList &paths)
{
    foreach (QString path, paths)
    {
        LoaderTask task;
        task.before = 0;
        task.path = path;
        m_tasks.append(task);
    }
    MetaDataManager::instance()->prepareForAnotherThread();
    m_filters = MetaDataManager::instance()->nameFilters();
    start(QThread::IdlePriority);
}

void FileLoader::insert(PlayListItem *before, const QString &path)
{
    insert(before, QStringList() << path);
}

void FileLoader::insert(PlayListItem *before, const QStringList &paths)
{
    foreach (QString path, paths)
    {
        LoaderTask task;
        task.before = before;
        task.path = path;
        m_tasks.append(task);
    }
    MetaDataManager::instance()->prepareForAnotherThread();
    m_filters = MetaDataManager::instance()->nameFilters();
    start(QThread::IdlePriority);
}

void FileLoader::finish()
{
    m_finished = true;
    wait();
    m_tasks.clear();
}

bool FileLoader::checkRestrictFilters(const QFileInfo &info)
{
    if(m_settings->restrictFilters().isEmpty())
        return true;

    foreach(QString filter, m_settings->restrictFilters())
    {
        QRegExp regexp (filter, Qt::CaseInsensitive, QRegExp::Wildcard);
        if(regexp.exactMatch(info.absoluteFilePath()))
            return true;
    }
    return false;
}

bool FileLoader::checkExcludeFilters(const QFileInfo &info)
{
    if(m_settings->excludeFilters().isEmpty())
        return true;

    foreach(QString filter, m_settings->excludeFilters())
    {
        QRegExp regexp (filter, Qt::CaseInsensitive, QRegExp::Wildcard);
        if(regexp.exactMatch(info.absoluteFilePath()))
            return false;
    }
    return true;
}
