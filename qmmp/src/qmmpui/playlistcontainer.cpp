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

#include "playlistcontainer.h"

PlayListContainer::PlayListContainer()
{
    qDebug("%s", Q_FUNC_INFO);
}

void PlayListContainer::addGroup(PlayListGroup *group)
{
    m_groups.append(group);
    m_items.append(group);
    foreach (PlayListTrack *item, *group->tracks())
    {
        m_items.append(item);
    }
    updateIndex();
}

void PlayListContainer::addTrack(PlayListTrack *track)
{
    if(!m_groups.isEmpty() && track->groupName() == m_groups.last()->formattedTitle())
    {
        PlayListGroup *group = m_groups.last();
        group->addTrack(track);
        m_items.insert(group->lastIndex + 1, track);
        updateIndex();
        return;
    }
    foreach(PlayListGroup *group, m_groups)
    {
        if(track->groupName() == group->formattedTitle())
        {
            group->addTrack(track);
            m_items.insert(group->lastIndex + 1, track);
            updateIndex();
            return;
        }
    }

    PlayListGroup *group = new PlayListGroup(track->groupName());
    group->addTrack(track);
    addGroup(group);
}

QList<PlayListGroup *> PlayListContainer::groups()
{
    return m_groups;
}

QList<PlayListItem *> PlayListContainer::items() const
{
    return m_items;
}

int PlayListContainer::count() const
{
    return m_items.count();
}

int PlayListContainer::trackCount() const
{
    return m_items.count() - m_groups.count();
}

QList<PlayListItem *> PlayListContainer::mid(int pos, int count) const
{
    return m_items.mid(pos, count);
}

bool PlayListContainer::isEmpty() const
{
    return m_items.isEmpty();
}

bool PlayListContainer::isSelected(int index) const
{
    if (0 <= index && index < m_items.count())
        return m_items.at(index)->isSelected();
    return false;
}

void PlayListContainer::setSelected(int index, bool selected)
{
    if (0 <= index && index < m_items.count())// && !m_items.at(index)->isGroup())
        m_items.at(index)->setSelected(selected);
}

void PlayListContainer::clearSelection()
{
    foreach (PlayListItem *item, m_items)
    {
        item->setSelected(false);
    }
}

int PlayListContainer::indexOf(PlayListItem *item) const
{
    return m_items.indexOf(item);
}

PlayListItem *PlayListContainer::item(int index) const
{
    if(index >= count() || index < 0)
    {
        qWarning("PlayListItem: index is out of range");
        return 0;
    }
    return m_items.at(index);
}

PlayListTrack *PlayListContainer::track(int index) const
{
    PlayListItem *i = item(index);
    if(!i || i->isGroup())
        return 0;
    return dynamic_cast<PlayListTrack *> (i);
}

bool PlayListContainer::contains(PlayListItem *item) const
{
    return m_items.contains(item);
}

int PlayListContainer::numberOfTrack(int index) const
{
    for(int i = 0; i < m_groups.count(); ++i)
    {
        if(index >= m_groups[i]->firstIndex && index <= m_groups[i]->lastIndex)
        {
            return index - (i+1);
        }
    }
    return -1;
}

void PlayListContainer::removeTrack(int index)
{
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

void PlayListContainer::removeTrack(PlayListTrack *track)
{
    m_items.removeAll(track);

    foreach(PlayListGroup *group, m_groups)
    {
        if(group->contains(track))
        {
            group->remove(track);
            if(group->isEmpty())
            {
                m_groups.removeAll(group);
                m_items.removeAll(group);
                delete group;
            }
            updateIndex();
            return;
        }
    }
}

void PlayListContainer::removeTracks(QList<PlayListTrack *> tracks)
{
    foreach(PlayListTrack *t, tracks)
        removeTrack(t);
}

void PlayListContainer::clear()
{
    while(!m_groups.isEmpty())
    {
        delete m_groups.takeFirst();
    }
    m_items.clear();
}

void PlayListContainer::updateIndex()
{
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
    }
}
