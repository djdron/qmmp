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

#ifndef PLAYLISTCONTAINER_P_H
#define PLAYLISTCONTAINER_P_H

#include <QList>
#include "playlistitem.h"
#include "playlisttrack.h"
#include "playlistgroup.h"

class PlayListContainer
{
public:
    PlayListContainer(){}
    virtual ~PlayListContainer(){}

    virtual void addTrack(PlayListTrack *track) = 0;
    void addTracks(QList<PlayListTrack *> tracks);
    virtual QList<PlayListGroup *> groups() = 0;
    virtual QList<PlayListItem *> items() const = 0;
    virtual int count() const = 0;
    virtual int trackCount() const = 0;
    virtual QList<PlayListItem *> mid(int pos, int count) const = 0;
    virtual bool isEmpty() const = 0;
    virtual bool isSelected(int index) const = 0;
    virtual void setSelected(int index, bool selected) = 0;
    virtual void clearSelection() = 0;
    virtual int indexOf(PlayListItem *item) const = 0;
    virtual PlayListItem *item(int index) const = 0;
    virtual PlayListTrack *track(int index) const = 0;
    virtual bool contains(PlayListItem *item) const = 0;
    virtual int numberOfTrack(int index) const = 0;
    virtual void removeTrack(int index) = 0;
    virtual void removeTrack(PlayListTrack *track) = 0;
    virtual void removeTracks(QList<PlayListTrack *> tracks) = 0;
    virtual bool move(QList<int> indexes, int from, int to) = 0;
    virtual QList<PlayListTrack *> takeAllTracks() = 0;
    virtual void clear() = 0;

    virtual void reverseList() = 0;
    virtual void randomizeList() = 0;
    virtual void sort(int mode);
    virtual void sortSelection(int mode);

protected:
    /*!
     * This internal method performs sorting of \b list_to_sort list of items.
     */
    void doSort(int sort_mode, QList<PlayListTrack*>& list_to_sort);
};

#endif // PLAYLISTCONTAINER_P_H
