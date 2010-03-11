/***************************************************************************
 *   Copyright (C) 2006 by Ilya Kotov                                      *
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

#include "playstate.h"

ShufflePlayState::ShufflePlayState(PlayListModel * model) : PlayState(model)
{
    prepare();
}

bool ShufflePlayState::next()
{
    int itm_count = m_model->items().count();

    if (itm_count > 0)
    {
        if (m_shuffled_current >= m_shuffled_indexes.count() -1 )
        {
            if (!m_model->isRepeatableList())
                return false;
            else
                prepare();
        }

        if (m_shuffled_current < m_shuffled_indexes.count() - 1)m_shuffled_current++;

        return m_model->setCurrent(m_shuffled_indexes.at(m_shuffled_current));
    }
    return false;
}

bool ShufflePlayState::previous()
{
    int itm_count = m_model->items().count();

    if (itm_count > 0)
    {
        if (m_shuffled_current <= 0)
        {
            if (!m_model->isRepeatableList())
                return false;
            else
            {
                prepare();
                m_shuffled_current = m_shuffled_indexes.count() - 1;
            }
        }

        if (itm_count > 1) m_shuffled_current --;

        m_model->setCurrent(m_shuffled_indexes.at(m_shuffled_current));
        return true;
    }
    return false;
}

void ShufflePlayState::prepare()
{
    resetState();
    for (int i = 0;i < m_model->items().count();i++)
    {
        if (i != m_model->currentRow())
            m_shuffled_indexes << i;
    }

    for (int i = 0;i < m_shuffled_indexes.count();i++)
        m_shuffled_indexes.swap(qrand()%m_shuffled_indexes.size(),qrand()%m_shuffled_indexes.size());

    m_shuffled_indexes.prepend(m_model->currentRow());
}

void ShufflePlayState::resetState()
{
    m_shuffled_indexes.clear();
    m_shuffled_current = 0;
}

NormalPlayState::NormalPlayState(PlayListModel * model) : PlayState(model)
{}


bool NormalPlayState::next()
{
    int itm_count = m_model->items().count();

    if (itm_count > 0)
    {
        if ( m_model->currentRow() == itm_count - 1)
        {
            if (m_model->isRepeatableList())
                return m_model->setCurrent(0);
            else
                return false;
        }
        return m_model->setCurrent(m_model->currentRow() + 1);
    }
    else
        return false;
}

bool NormalPlayState::previous()
{
    int itm_count = m_model->items().count();

    if (itm_count > 0)
    {
        if ( m_model->currentRow() < 1 && !m_model->isRepeatableList())
            return false;
        else if (m_model->setCurrent(m_model->currentRow() - 1))
            return true;
        else if (m_model->isRepeatableList())
            return m_model->setCurrent(m_model->items().count() - 1);
    }

    return false;
}

