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

#ifndef PLAYLISTCONTAINER_H
#define PLAYLISTCONTAINER_H

#include <QList>
#include "playlistitem.h"
#include "playlisttrack.h"
#include "playlistgroup.h"


class PlayListContainer
{
public:
    PlayListContainer();

    void addGroup(PlayListGroup *group);
    void addTrack(PlayListTrack *item);

    QList<PlayListGroup *> groups();
    QList<PlayListItem *> items() const;
    int count() const;
    int trackCount() const;
    QList<PlayListItem *> mid(int pos, int count) const;
    bool isEmpty() const;

    bool isSelected(int index) const;
    void setSelected(int index, bool selected);
    void clearSelection();
    int indexOf(PlayListItem *item) const;
    PlayListItem *item(int index) const;
    PlayListTrack *track(int index) const;
    bool contains(PlayListItem *item) const;
    int numberOfTrack(int index) const;

    void removeTrack(int index);
    void removeTrack(PlayListTrack *track);
    void removeTracks(QList<PlayListTrack *> tracks);

    void move(QList<int> indexes, int from, int to);

    void clear();

private:
    void updateIndex();
    QList<PlayListGroup *> m_groups;
    QList<PlayListItem *> m_items;

};

#endif // PLAYLISTCONTAINER_H
