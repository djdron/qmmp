/***************************************************************************
 *   Copyright (C) 2013 by Ilya Kotov                                      *
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

#ifndef PLAYLISTGROUP_H
#define PLAYLISTGROUP_H

#include "playlisttrack.h"
#include "playlistitem.h"

class PlayListGroup : public PlayListItem
{
public:
    PlayListGroup(const QString &formattedTitle);

    virtual ~PlayListGroup();

    int firstIndex;
    int lastIndex;

    const QString formattedTitle();
    void addTrack(PlayListTrack *track);
    void addTracks(QList<PlayListTrack *> tracks);
    bool contains(PlayListTrack *track) const;
    bool isEmpty() const;
    void remove(PlayListTrack *track);
    QList<PlayListTrack *> tracks();
    int count() const;
    /*!
     *  Returns formatted length of the item.
     */
    const QString formattedLength() { return QString(); }
    virtual bool isGroup() const { return true; }
    void move(int from, int to);

    QList<PlayListTrack *> takeAll();

private:
    QList<PlayListTrack *> m_tracks;
    QString m_name;
};

#endif // PLAYLISTGROUP_H
