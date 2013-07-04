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

#include "tagupdater_p.h"

TagUpdater::TagUpdater(QObject* o, PlayListItem* item) : m_observable(o), m_item(item)
{
    m_item->setFlag(PlayListItem::EDITING);
    connect(m_observable, SIGNAL(destroyed(QObject *)),SLOT(updateTag()));
    connect(m_observable, SIGNAL(destroyed(QObject *)),SLOT(deleteLater()));
}

void TagUpdater::updateTag()
{
    if (m_item->flag() == PlayListItem::SCHEDULED_FOR_DELETION)
    {
        delete m_item;
        m_item = NULL;
    }
    else
    {
        m_item->updateTags();
        m_item->setFlag(PlayListItem::FREE);
    }
}
