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
    m_prev_count = 0;
}


TrackListObject::~TrackListObject()
{
}

int TrackListObject::AddTrack(const QString &in0, bool in1)
{
    QString path = in0;
    if(in0.startsWith("file://"))
    {
        path = QUrl(in0).toLocalFile ();
        if(!QFile::exists(path))
            return 1; //error
    }
    if(in1)
    {
        m_pl_manager->selectPlayList(m_model);
        m_player->stop();
        qDebug("1");
        m_prev_count = m_model->count();
        connect(m_model, SIGNAL(listChanged()), this, SLOT(checkNewItem()));
        connect(m_model, SIGNAL(loaderFinished()), this, SLOT(disconnectPl()));
        qDebug("2");
    }
    m_model->add(path);
    return 0;
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
        map.insert("title", item->value(Qmmp::TITLE));
        map.insert("artist", item->value(Qmmp::ARTIST));
        map.insert("album", item->value(Qmmp::ALBUM));
        map.insert("tracknumber", item->value(Qmmp::TRACK));
        map.insert("time", (quint32)item->length());
        map.insert("mtime", (quint32)item->length() * 1000);
        map.insert("genre", item->value(Qmmp::GENRE));
        map.insert("comment", item->value(Qmmp::COMMENT));
        map.insert("year", item->value(Qmmp::YEAR).toUInt());
    }
    return map;
}

void TrackListObject::SetLoop(bool in0)
{
    m_pl_manager->setRepeatableList(in0);
}

void TrackListObject::SetRandom(bool in0)
{
    m_pl_manager->setShuffle(in0);
}

void TrackListObject::disconnectPl()
{
    disconnect(m_model, SIGNAL(listChanged()), this, SLOT(checkNewItem()));
    disconnect(m_model, SIGNAL(loaderFinished()), this, SLOT(disconnectPl()));
}

void TrackListObject::checkNewItem() //checks for new item in playlist
{
    if(m_model->count() > m_prev_count)
    {
        disconnectPl(); //disconnect playlist;
        m_model->setCurrent(m_prev_count); // activate first added item
        m_player->play(); // ... and play it
    }
}

void  TrackListObject::updateTrackList()
{
    emit TrackListChange(m_model->count());
}

void TrackListObject::switchPlayList(PlayListModel *cur, PlayListModel *prev)
{
    disconnectPl();
    m_model = cur;
    connect (m_model, SIGNAL(listChanged()), SLOT(updateTrackList()));
    if(prev)
        disconnect(prev,0,this,0);
    updateTrackList();
}
