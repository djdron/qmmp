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

#include <QByteArray>
#include <QFile>
#include <QBuffer>
#include <QSettings>
#include <QDir>
#include <qmmp/fileinfo.h>
#include "playlistsettings.h"
#include "playlistmanager.h"

PlayListManager::PlayListManager(QObject *parent) : QObject(parent)
{
    m_current = 0;
    m_selected = 0;
    m_repeatable = FALSE;
    m_shuffle = FALSE;
    readPlayLists();
}

PlayListManager::~PlayListManager()
{
    writePlayLists();
    delete PlaylistSettings::instance();
}

PlayListModel *PlayListManager::selectedPlayList() const
{
    return m_selected;
}

PlayListModel *PlayListManager::currentPlayList() const
{
    return m_current;
}

QList <PlayListModel *> PlayListManager::playLists() const
{
    return m_models;
}

QStringList PlayListManager::playListNames() const
{
    QStringList names;
    foreach(PlayListModel *model, m_models)
        names << model->name();
    return names;
}

void PlayListManager::selectPlayList(PlayListModel *model)
{
    if(model != m_selected && m_models.contains(model))
    {
        PlayListModel *prev = m_selected;
        m_selected = model;
        emit selectedPlayListChanged(model, prev);
        emit playListsChanged();
    }
}

void PlayListManager::selectPlayList(int i)
{
    if(i > m_models.count() - 1)
        return;
    selectPlayList(playListAt(i));
}

void PlayListManager::selectNextPlayList()
{
    int i = m_models.indexOf(m_selected);
    i++;
    if( i >= 0 && i < m_models.size())
        selectPlayList(i);
}

void PlayListManager::selectPreviousPlayList()
{
    int i = m_models.indexOf(m_selected);
    i--;
    if( i >= 0 && i < m_models.size())
        selectPlayList(i);
}

void PlayListManager::activatePlayList(PlayListModel *model)
{
    if(model != m_current && m_models.contains(model))
    {
        PlayListModel *prev = m_current;
        m_current = model;
        emit currentPlayListChanged(model, prev);
        emit playListsChanged();
    }
}

PlayListModel *PlayListManager::createPlayList(const QString &name)
{
    PlayListModel *model = new PlayListModel (name.isEmpty() ? tr("Playlist") : name, this);
    int i = m_models.indexOf(m_selected);
    m_models.insert(i, model);
    model->prepareForRepeatablePlaying(m_repeatable);
    model->prepareForShufflePlaying(m_shuffle);
    emit playListAdded(i);
    emit playListsChanged();
    return model;
}

void PlayListManager::removePlayList(PlayListModel *model)
{
     if(m_models.count() < 2 || !m_models.contains(model))
        return;

     int i = m_models.indexOf(model);

     if(m_current == model)
     {
         m_current = m_models.at((i > 0) ? (i - 1) : (i + 1));
         emit currentPlayListChanged(m_current, model);
     }
     if(m_selected == model)
     {
         m_selected = m_models.at((i > 0) ? (i - 1) : (i + 1));
         emit selectedPlayListChanged(m_selected, model);
     }
     m_models.removeAt(i);
     model->deleteLater();
     emit playListRemoved(i);
     emit playListsChanged();
}

void PlayListManager::setRepeatableList(bool r)
{
    if(m_repeatable == r)
        return;
    m_repeatable = r;
    foreach(PlayListModel *model, m_models)
        model->prepareForRepeatablePlaying(r);
    emit repeatableListChanged(r);
}

void PlayListManager::setShuffle(bool s)
{
    if(m_shuffle == s)
        return;
    m_shuffle = s;
    foreach(PlayListModel *model, m_models)
        model->prepareForShufflePlaying(s);
    emit shuffleChanged(s);
}

int PlayListManager::count()
{
    return m_models.count();
}

int PlayListManager::indexOf(PlayListModel *model)
{
    return m_models.indexOf(model);
}

PlayListModel *PlayListManager::playListAt(int i)
{
    if(i >= 0 || i < m_models.count())
        return m_models.at(i);
    return 0;
}

bool PlayListManager::convertUnderscore()
{
    return PlaylistSettings::instance()->convertUnderscore();
}

bool PlayListManager::convertTwenty()
{
    return PlaylistSettings::instance()->convertTwenty();
}

bool PlayListManager::useMetadata()
{
    return PlaylistSettings::instance()->useMetadata();
}

const QString PlayListManager::format() const
{
    return PlaylistSettings::instance()->format();
}

void PlayListManager::setConvertUnderscore(bool yes)
{
    PlaylistSettings::instance()->setConvertUnderscore(yes);
    emit settingsChanged();
}

void PlayListManager::setConvertTwenty(bool yes)
{
    PlaylistSettings::instance()->setConvertTwenty(yes);
    emit settingsChanged();
}

void PlayListManager::setUseMetadata(bool yes)
{
    PlaylistSettings::instance()->setUseMetadata(yes);
    emit settingsChanged();
}

void PlayListManager::setFormat(const QString &format)
{
    PlaylistSettings::instance()->setFormat(format);
    emit settingsChanged();
}

bool PlayListManager::isRepeatableList() const
{
    return m_repeatable;
}

bool PlayListManager::isShuffle() const
{
    return m_shuffle;
}

void PlayListManager::readPlayLists()
{
    QString line, param, value;
    int s;
    QList <FileInfo *> infoList;
    QFile file(QDir::homePath() +"/.qmmp/playlist.txt");
    file.open(QIODevice::ReadOnly);
    QByteArray array = file.readAll();
    file.close();
    QBuffer buffer(&array);
    buffer.open(QIODevice::ReadOnly);

    while (!buffer.atEnd())
    {
        line = QString::fromUtf8(buffer.readLine()).trimmed();
        if ((s = line.indexOf("=")) < 0)
            continue;

        param = line.left(s);
        value = line.right(line.size() - s - 1);

        if(param == "playlist")
        {
            if(!m_models.isEmpty())
            {
                foreach(FileInfo *info, infoList)
                    m_models.last()->add(new PlayListItem(info));
            }
            infoList.clear();
            m_models << new PlayListModel(value, this);
        }
        else if (param == "file")
            infoList << new FileInfo(value);
        else if (infoList.isEmpty())
            continue;
        else if (param == "title")
            infoList.last()->setMetaData(Qmmp::TITLE, value);
        else if (param == "artist")
            infoList.last()->setMetaData(Qmmp::ARTIST, value);
        else if (param == "album")
            infoList.last()->setMetaData(Qmmp::ALBUM, value);
        else if (param == "comment")
            infoList.last()->setMetaData(Qmmp::COMMENT, value);
        else if (param == "genre")
            infoList.last()->setMetaData(Qmmp::GENRE, value);
        else if (param == "composer")
            infoList.last()->setMetaData(Qmmp::COMPOSER, value);
        else if (param == "year")
            infoList.last()->setMetaData(Qmmp::YEAR, value);
        else if (param == "track")
            infoList.last()->setMetaData(Qmmp::TRACK, value);
        else if (param == "disc")
            infoList.last()->setMetaData(Qmmp::DISCNUMBER, value);
        else if (param == "length")
            infoList.last()->setLength(value.toInt());
    }
    buffer.close();
    if(!m_models.isEmpty())
    {
        foreach(FileInfo *info, infoList)
            m_models.last()->add(new PlayListItem(info));
    }
    else
        m_models << new PlayListModel("Default",this);
    m_selected = m_models.at(0);
    m_current = m_models.at(0);
}

void PlayListManager::writePlayLists()
{
    QFile file(QDir::homePath() +"/.qmmp/playlist.txt");
    file.open(QIODevice::WriteOnly);
    foreach(PlayListModel *model, m_models)
    {
        QList<PlayListItem *> items = model->items();
        file.write(QString("playlist=%1").arg(model->name()).toUtf8() +"\n");
        foreach(PlayListItem* m, items)
        {
            file.write(QString("file=%1").arg(m->url()).toUtf8() +"\n");
            file.write(QString("title=%1").arg(m->title()).toUtf8() +"\n");
            file.write(QString("artist=%1").arg(m->artist()).toUtf8() +"\n");
            file.write(QString("album=%1").arg(m->album()).toUtf8() +"\n");
            file.write(QString("comment=%1").arg(m->comment()).toUtf8() +"\n");
            file.write(QString("genre=%1").arg(m->genre()).toUtf8() +"\n");
            file.write(QString("composer=%1").arg(m->composer()).toUtf8() +"\n");
            file.write(QString("year=%1").arg(m->year()).toUtf8() +"\n");
            file.write(QString("track=%1").arg(m->track()).toUtf8() +"\n");
            file.write(QString("disc=%1").arg(m->discNumber()).toUtf8() +"\n");
            file.write(QString("length=%1").arg(m->length()).toUtf8() +"\n");
        }
    }
    file.close();
}

void PlayListManager::clear()
{
    m_selected->clear();
}

void PlayListManager::clearSelection()
{
    m_selected->clearSelection();
}

void PlayListManager::removeSelected()
{
    m_selected->removeSelected();
}

void PlayListManager::removeUnselected()
{
    m_selected->removeUnselected();
}

void PlayListManager::removeAt (int i)
{
    m_selected->removeAt(i);
}

void PlayListManager::removeItem (PlayListItem *item)
{
    m_selected->removeItem(item);
}

void PlayListManager::invertSelection()
{
    m_selected->invertSelection();
}

void PlayListManager::selectAll()
{
    m_selected->selectAll();
}

void PlayListManager::showDetails()
{
    m_selected->showDetails();
}

void PlayListManager::addFile(const QString &path)
{
    m_selected->addFile(path);
}

void PlayListManager::addFiles(const QStringList& l)
{
    m_selected->addFiles(l);
}

void PlayListManager::addDirectory(const QString& dir)
{
    m_selected->addDirectory(dir);
}

void PlayListManager::randomizeList()
{
    m_selected->randomizeList();
}

void PlayListManager::reverseList()
{
    m_selected->reverseList();
}

void PlayListManager::sortSelection(int mode)
{
    m_selected->sortSelection(mode);
}

void PlayListManager::sort(int mode)
{
    m_selected->sort(mode);
}

void PlayListManager::addToQueue()
{
    m_selected->addToQueue();
}

void PlayListManager::clearInvalidItems()
{
    m_selected->clearInvalidItems();
}
