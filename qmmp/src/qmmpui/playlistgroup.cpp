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

#include "playlistgroup.h"

PlayListGroup::PlayListGroup(const QString &name)
{
    m_name = name;
    firstIndex = 0;
    lastIndex = 0;
}

PlayListGroup::~PlayListGroup()
{
    while(!m_tracks.isEmpty())
    {
        PlayListTrack* mf = m_tracks.takeFirst();

        if (mf->flag() == PlayListTrack::FREE)
        {
            delete mf;
        }
        else if (mf->flag() == PlayListTrack::EDITING)
        {
            mf->setFlag(PlayListTrack::SCHEDULED_FOR_DELETION);
        }
    }
}

const QString PlayListGroup::formattedTitle()
{
    return m_name;
}

void PlayListGroup::addTrack(PlayListTrack *track)
{
    m_tracks.append(track);
}

void PlayListGroup::addTracks(QList<PlayListTrack *> tracks)
{
    m_tracks.append(tracks);
}

void PlayListGroup::insertTrack(int pos, PlayListTrack *track)
{
    m_tracks.insert(pos, track);
}

bool PlayListGroup::contains(PlayListTrack *track) const
{
    return m_tracks.contains(track);
}

bool PlayListGroup::isEmpty() const
{
    return m_tracks.isEmpty();
}

void PlayListGroup::remove(PlayListTrack *track)
{
    m_tracks.removeAll(track);
}

QList<PlayListTrack *> PlayListGroup::tracks()
{
    return m_tracks;
}

int PlayListGroup::count() const
{
    return m_tracks.count();
}

bool PlayListGroup::isGroup() const
{
    return true;
}

void PlayListGroup::move(int from, int to)
{
    m_tracks.move(from, to);
}

QList<PlayListTrack *> PlayListGroup::takeAll()
{
    QList<PlayListTrack *> t = m_tracks;
    m_tracks.clear();
    return t;
}
