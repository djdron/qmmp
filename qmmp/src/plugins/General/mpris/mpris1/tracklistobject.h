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
#ifndef TRACKLISTOBJECT_H
#define TRACKLISTOBJECT_H

#include <QObject>
#include <QString>
#include <QVariantMap>

class PlayListModel;
class PlayListManager;
class MediaPlayer;

/**
    @author Ilya Kotov <forkotov02@hotmail.ru>
*/
class TrackListObject : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.freedesktop.MediaPlayer")

public:
    TrackListObject(QObject *parent = 0);

    ~TrackListObject();

public slots:
    int AddTrack(const QString &in0, bool in1);
    void DelTrack(int in0);
    int GetCurrentTrack();
    int GetLength();
    QVariantMap GetMetadata(int in0);
    void SetLoop(bool in0);
    void SetRandom(bool in0);

signals:
    void TrackListChange(int in0);

private slots:
    void disconnectPl();
    void checkNewItem();
    void updateTrackList();
    void switchPlayList(PlayListModel *cur, PlayListModel *prev);

private:
    PlayListModel *m_model;
    PlayListManager *m_pl_manager;
    MediaPlayer *m_player;
    int m_prev_count;

};

#endif
