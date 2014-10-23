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

#include "playlistmodel.h"
#include "groupedcontainer_p.h"

GroupedContainer::GroupedContainer()
{
    m_reverted = false;
    m_update = true;
}

GroupedContainer::~GroupedContainer()
{
    clear();
}

void GroupedContainer::addGroup(PlayListGroup *group)
{
    m_groups.append(group);
    m_update = true;
}

void GroupedContainer::addTracks(QList<PlayListTrack *> tracks)
{
    PlayListGroup *group = m_groups.isEmpty() ? 0 : m_groups.last();

    foreach (PlayListTrack *track, tracks)
    {
        if(!group || track->groupName() != group->formattedTitle())
        {
            group = 0;
            foreach(PlayListGroup *g, m_groups)
            {
                if(track->groupName() == g->formattedTitle())
                {
                    group = g;
                    break;
                }
            }
        }

        if(!group)
        {
            group = new PlayListGroup(track->groupName());
            addGroup(group);
        }

        group->trackList.append(track);
    }
    m_update = true;
}

void GroupedContainer::insertTrack(int index, PlayListTrack *track)
{
    //insert if possible
    foreach(PlayListGroup *group, m_groups)
    {
        if(track->groupName() == group->formattedTitle() &&
                index > group->firstIndex && index <= group->lastIndex + 1)
        {
            group->trackList.insert(index - group->firstIndex - 1, track);
            m_update = true;
            return;
        }
    }
    //just add otherwise
    addTrack(track);
}

QList<PlayListGroup *> GroupedContainer::groups() const
{
    return m_groups;
}

QList<PlayListItem *> GroupedContainer::items() const
{
    updateIndex();
    return m_items;
}

int GroupedContainer::count() const
{
    updateIndex();
    return m_items.count();
}

int GroupedContainer::trackCount() const
{
    updateIndex();
    return m_items.count() - m_groups.count();
}

QList<PlayListItem *> GroupedContainer::mid(int pos, int count) const
{
    updateIndex();
    return m_items.mid(pos, count);
}

bool GroupedContainer::isEmpty() const
{
    return m_groups.isEmpty();
}

bool GroupedContainer::isSelected(int index) const
{
    updateIndex();
    if (0 <= index && index < m_items.count())
        return m_items.at(index)->isSelected();
    return false;
}

void GroupedContainer::setSelected(int index, bool selected)
{
    updateIndex();
    if (0 <= index && index < m_items.count())// && !m_items.at(index)->isGroup())
        m_items.at(index)->setSelected(selected);
}

void GroupedContainer::clearSelection()
{
    updateIndex();
    foreach (PlayListItem *item, m_items)
    {
        item->setSelected(false);
    }
}

int GroupedContainer::indexOf(PlayListItem *item) const
{
    updateIndex();
    return m_items.indexOf(item);
}

PlayListItem *GroupedContainer::item(int index) const
{
    updateIndex();
    if(index >= count() || index < 0)
    {
        qWarning("GroupedContainer: index is out of range");
        return 0;
    }
    return m_items.at(index);
}

PlayListTrack *GroupedContainer::track(int index) const
{
    updateIndex();
    PlayListItem *i = item(index);
    if(!i || i->isGroup())
        return 0;
    return dynamic_cast<PlayListTrack *> (i);
}

PlayListGroup *GroupedContainer::group(int index) const
{
    PlayListItem *i = item(index);
    if(i && i->isGroup())
        return dynamic_cast<PlayListGroup *> (i);
    return 0;
}

bool GroupedContainer::contains(PlayListItem *item) const
{
    updateIndex();
    return m_items.contains(item);
}

int GroupedContainer::numberOfTrack(int index) const
{
    updateIndex();
    for(int i = 0; i < m_groups.count(); ++i)
    {
        if(index >= m_groups[i]->firstIndex && index <= m_groups[i]->lastIndex)
        {
            return index - (i+1);
        }
    }
    return -1;
}

PlayListTrack *GroupedContainer::findTrack(int number) const
{
    updateIndex();
    int firstNumber = 0;
    foreach (PlayListGroup *group, m_groups)
    {
        if(number >= firstNumber && number < firstNumber + group->count())
        {
            return group->trackList.at(number - firstNumber);
        }
        firstNumber += group->count();
    }
    return 0;
}

void GroupedContainer::removeTrack(int index)
{
    updateIndex();
    PlayListTrack *t = track(index);
    if(t)
    {
        removeTrack(t);
        if (t->flag() == PlayListTrack::FREE)
        {
            delete t;
        }
        else if (t->flag() == PlayListTrack::EDITING)
        {
            t->setFlag(PlayListTrack::SCHEDULED_FOR_DELETION);
        }
    }
}

void GroupedContainer::removeTrack(PlayListTrack *track)
{
    foreach(PlayListGroup *group, m_groups)
    {
        if(group->contains(track))
        {
            group->trackList.removeAll(track);
            if(group->isEmpty())
            {
                m_groups.removeAll(group);
                delete group;
            }
            m_update = true;
            return;
        }
    }
}

void GroupedContainer::removeTracks(QList<PlayListTrack *> tracks)
{
    foreach(PlayListTrack *t, tracks)
        removeTrack(t);
}

bool GroupedContainer::move(QList<int> indexes, int from, int to)
{
    updateIndex();
    PlayListGroup *group = 0;

    foreach (PlayListGroup *g, m_groups)
    {
        if(from > g->firstIndex && from <= g->lastIndex &&
                to > g->firstIndex && to <= g->lastIndex)
        {
            group = g;
            break;
        }
    }

    if(!group)
        return false;

    foreach (int i, indexes)
    {
        if(i <= group->firstIndex || i > group->lastIndex)
            return false;
        if(i + to - from - group->firstIndex - 1 >= group->count())
            return false;
        if(i + to - from - group->firstIndex - 1 < 0)
            return false;
        if(i + to - from < 0)
            return false;
    }

    if (from > to)
    {
        foreach(int i, indexes)
        {
            if (i + to - from < 0)
                break;
            else
            {
                //m_items.move(i,i + to - from);
                group->trackList.move(i - group->firstIndex - 1,
                                      i + to - from  - group->firstIndex - 1);
            }
        }
    }
    else
    {
        for (int i = indexes.count() - 1; i >= 0; i--)
        {
            if (indexes[i] + to - from >= m_items.count())
                break;
            else
            {
                //m_items.move(indexes[i], indexes[i] + to - from);
                group->trackList.move(indexes[i] - group->firstIndex - 1,
                                      indexes[i] + to - from - group->firstIndex - 1);
            }
        }
    }
    m_update = true;
    return true;
}

QList<PlayListTrack *> GroupedContainer::takeAllTracks()
{
    QList<PlayListTrack *> tracks;
    foreach (PlayListGroup *g, m_groups)
    {
        tracks.append(g->trackList);
        g->trackList.clear();
    }
    clear();
    return tracks;
}

void GroupedContainer::clear()
{
    while(!m_groups.isEmpty())
    {
        delete m_groups.takeFirst();
    }
    m_items.clear();
}

void GroupedContainer::reverseList()
{
    QList<PlayListTrack *> tracks = takeAllTracks();

    for (int i = 0; i < tracks.size()/2 ;i++)
        tracks.swap(i, tracks.size() - i - 1);

    addTracks(tracks);
}

void GroupedContainer::randomizeList()
{
    QList<PlayListTrack *> tracks = takeAllTracks();

    for (int i = 0; i < tracks.size(); i++)
        tracks.swap(qrand()%tracks.size(),qrand()%tracks.size());

    addTracks(tracks);
}

void GroupedContainer::sort(int mode)
{
    if(mode == PlayListModel::ARTIST || mode == PlayListModel::ALBUM
            || mode == PlayListModel::DATE || mode == PlayListModel::GROUP)
    {
        QList<PlayListTrack *> tracks = takeAllTracks();
        doSort(mode, tracks, m_reverted);
        addTracks(tracks);
    }
    else
    {
        m_items.clear();
        foreach (PlayListGroup *g, m_groups)
        {
            doSort(mode, g->trackList, m_reverted);
            m_items.append(g);
            foreach (PlayListTrack *t, g->trackList)
            {
                m_items.append(t);
            }
        }
        updateIndex();
    }
    m_reverted = !m_reverted;
}

void GroupedContainer::sortSelection(int mode)
{
    QList<PlayListTrack *> tracks = takeAllTracks();
    QList<PlayListTrack *> selected_tracks;
    QList<int> selected_indexes;
    for(int i = 0; i < tracks.count(); ++i)
    {
        if(tracks[i]->isSelected())
        {
            selected_tracks.append(tracks[i]);
            selected_indexes.append(i);
        }
    }
    doSort(mode, selected_tracks, m_reverted);

    for (int i = 0; i < selected_indexes.count(); i++)
        tracks.replace(selected_indexes[i], selected_tracks[i]);

    addTracks(tracks);

    m_reverted = !m_reverted;
}

void GroupedContainer::updateIndex() const
{
    if(!m_update)
        return;

    m_items.clear();

    for(int i = 0; i < m_groups.count(); ++i)
    {
        if(i == 0)
        {
            m_groups[i]->firstIndex = 0;
            m_groups[i]->lastIndex = m_groups[i]->count();
        }
        else
        {
            m_groups[i]->firstIndex = m_groups[i-1]->lastIndex + 1;
            m_groups[i]->lastIndex = m_groups[i]->firstIndex + m_groups[i]->count();
        }

        m_items.append(m_groups.at(i));
        foreach (PlayListTrack *track, m_groups.at(i)->trackList)
        {
            m_items.append(track);
        }
    }
    m_update = false;
}
