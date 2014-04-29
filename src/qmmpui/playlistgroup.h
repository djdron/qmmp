/***************************************************************************
 *   Copyright (C) 2013-2014 by Ilya Kotov                                 *
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

/** @brief The PlayListTrack class provides a group for use with the PlayListModel class.
 * @author Ilya Kotov <forkotov02@hotmail.ru>
 */
class PlayListGroup : public PlayListItem
{
public:
    /*!
     * Constructor.
     * @param formattedTitle Title of the group.
     */
    PlayListGroup(const QString &formattedTitle);
    /*!
     * Object destructor.
     */
    virtual ~PlayListGroup();
    /*!
     * First index of the group.
     */
    int firstIndex;
    /*!
     * Last index of the  group.
     */
    int lastIndex;
    /*!
     * Returns formatted title of the  group.
     */
    const QString formattedTitle();
    /*!
     * Adds track \b track to the the group.
     */
    void addTrack(PlayListTrack *track);
    /*!
     * Adds a list of tracks \b tracks to the the group.
     */
    void addTracks(QList<PlayListTrack *> tracks);
    /*!
     * Inserts a track \b tracks to the the group at the position \b pos.
     */
    void insertTrack(int pos, PlayListTrack *track);
    /*!
     * Returns \b true if the group contains track \b track.
     * Otherwise returns \b false.
     */
    bool contains(PlayListTrack *track) const;
    /*!
     * Returns \b true if the group is empty.
     * Otherwise returns \b false.
     */
    bool isEmpty() const;
    /*!
     * Returns track \b track from the group.
     */
    void remove(PlayListTrack *track);
    /*!
     * Returns a list of tracks if the group.
     */
    QList<PlayListTrack *> tracks();
    /*!
     * Returns number of tracks if the group.
     */
    int count() const;
    /*!
     *  Returns formatted length of the item.
     */
    const QString formattedLength() { return QString(); }
    /*!
     * Returns \b true.
     */
    bool isGroup() const;
    /*!
     * Moves the track from position \b from to position \b to.
     */
    void move(int from, int to);
    /*!
     * Removes all tracks from the group and returns a list of them.
     */
    QList<PlayListTrack *> takeAll();

private:
    QList<PlayListTrack *> m_tracks;
    QString m_name;
};

#endif // PLAYLISTGROUP_H
