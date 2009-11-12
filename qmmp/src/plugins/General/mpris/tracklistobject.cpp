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

#include <QFile>
#include <QUrl>

#include <qmmpui/playlistmanager.h>
#include <qmmpui/mediaplayer.h>
#include <qmmpui/playlistitem.h>

#include "tracklistobject.h"

TrackListObject::TrackListObject(QObject *parent) : QObject(parent)
{
    m_player = MediaPlayer::instance();
    m_pl_manager = m_player->playListManager();
    m_model = m_pl_manager->currentPlayList();
    connect (m_model, SIGNAL(listChanged()), SLOT(updateTrackList()));
    connect (m_pl_manager, SIGNAL(currentPlayListChanged(PlayListModel*,PlayListModel*)),
             SLOT(switchPlayList(PlayListModel*,PlayListModel*)));
}


TrackListObject::~TrackListObject()
{
}

int TrackListObject::AddTrack(const QString &in0, bool in1)
{
    int old_count = m_model->count();
    if(in0.startsWith("file://"))
        m_model->addFile(QUrl(in0).toLocalFile ()); //converts url to local file path
    else
        m_model->addFile(in0);
    int new_count = m_model->count();
    if(new_count == old_count)
        return 0;
    if(in1)
    {
        m_model->setCurrent(new_count-1);
        m_player->stop();
        m_player->play();
    }
    return 1;
}

void TrackListObject::DelTrack(int in0)
{
    m_model->removeAt(in0);
}

int TrackListObject::GetCurrentTrack()
{
    return m_model->currentRow();
}

int TrackListObject::GetLength()
{
    return m_model->count();
}

QVariantMap TrackListObject::GetMetadata(int in0)
{
    QVariantMap map;
    PlayListItem *item = m_model->item(in0);
    if (item)
    {
        if (QFile::exists(item->url()))
            map.insert("location", "file://" + item->url());
        else
            map.insert("location", item->url());
        map.insert("title", item->title());
        map.insert("artist", item->artist());
        map.insert("album", item->album());
        map.insert("tracknumber", item->track());
        map.insert("time", item->length());
        map.insert("mtime", item->length() * 1000);
        map.insert("genre", item->genre());
        map.insert("comment", item->comment());
        map.insert("year", item->year());
    }
    return map;
}

void TrackListObject::SetLoop(bool in0)
{
    m_model->prepareForRepeatablePlaying(in0);
}

void TrackListObject::SetRandom(bool in0)
{
    m_model->prepareForShufflePlaying(in0);
}

void  TrackListObject::updateTrackList()
{
    emit TrackListChange(m_model->count());
}

void TrackListObject::switchPlayList(PlayListModel *cur, PlayListModel *prev)
{
    m_model = cur;
    connect (m_model, SIGNAL(listChanged()), SLOT(updateTrackList()));
    if(prev)
        disconnect(prev,0,this,0);
    updateTrackList();
}
