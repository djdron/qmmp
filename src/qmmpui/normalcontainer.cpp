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

#include "normalcontainer_p.h"

NormalContainer::NormalContainer()
{
}

NormalContainer::~NormalContainer()
{
    clear();
}

void NormalContainer::addTrack(PlayListTrack *track)
{
    m_items.append(track);
}

QList<PlayListGroup *> NormalContainer::groups()
{
    return QList<PlayListGroup *>();
}

QList<PlayListItem *> NormalContainer::items() const
{
    return m_items;
}

int NormalContainer::count() const
{
    return m_items.count();
}

int NormalContainer::trackCount() const
{
    return m_items.count();
}

QList<PlayListItem *> NormalContainer::mid(int pos, int count) const
{
    return m_items.mid(pos, count);
}

bool NormalContainer::isEmpty() const
{
    return m_items.isEmpty();
}

bool NormalContainer::isSelected(int index) const
{
    if (0 <= index && index < m_items.count())
        return m_items.at(index)->isSelected();
    return false;
}

void NormalContainer::setSelected(int index, bool selected)
{
    if (0 <= index && index < m_items.count())
        m_items.at(index)->setSelected(selected);
}

void NormalContainer::clearSelection()
{
    foreach (PlayListItem *item, m_items)
    {
        item->setSelected(false);
    }
}

int NormalContainer::indexOf(PlayListItem *item) const
{
    return m_items.indexOf(item);
}

PlayListItem *NormalContainer::item(int index) const
{
    if(index >= count() || index < 0)
    {
        qWarning("PlayListItem: index is out of range");
        return 0;
    }
    return m_items.at(index);
}

PlayListTrack *NormalContainer::track(int index) const
{
    PlayListItem *i = item(index);
    return dynamic_cast<PlayListTrack *> (i);
}

bool NormalContainer::contains(PlayListItem *item) const
{
    return m_items.contains(item);
}

int NormalContainer::numberOfTrack(int index) const
{
    return index;
}

void NormalContainer::removeTrack(int index)
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

void NormalContainer::removeTrack(PlayListTrack *track)
{
    m_items.removeAll(track);
}

void NormalContainer::removeTracks(QList<PlayListTrack *> tracks)
{
    foreach(PlayListTrack *t, tracks)
        removeTrack(t);
}

bool NormalContainer::move(QList<int> indexes, int from, int to)
{
    if (from > to)
    {
        foreach(int i, indexes)
        {
            if (i + to - from < 0)
                break;

            else
                m_items.move(i,i + to - from);
        }
    }
    else
    {
        for (int i = indexes.count() - 1; i >= 0; i--)
        {
            if (indexes[i] + to - from >= m_items.count())
                break;
            else
                m_items.move(indexes[i], indexes[i] + to - from);
        }
    }
    return true;
}

void NormalContainer::clear()
{
    qDeleteAll(m_items);
    m_items.clear();
}
