/***************************************************************************
 *   Copyright (C) 2006-2013 by Ilya Kotov                                 *
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

#include <QKeyEvent>
#include <qmmpui/playlistmodel.h>
#include <qmmpui/playlistmanager.h>
#include <qmmpui/mediaplayer.h>
#include "playlist.h"
#include "listwidget.h"
#include "keyboardmanager.h"

KeyboardManager::KeyboardManager (ListWidget *l)
{
    m_listWidget = l;
}

bool KeyboardManager::handleKeyPress (QKeyEvent* ke)
{
    bool handled = true;
    switch (ke->key())
    {
        case Qt::Key_Up:
            keyUp (ke);
            break;
        case Qt::Key_Down:
            keyDown (ke);
            break;
        case Qt::Key_PageUp:
            keyPgUp (ke);
            break;
        case Qt::Key_PageDown:
            keyPgDown (ke);
            break;
        case Qt::Key_Enter:
        case Qt::Key_Return:
            keyEnter (ke);
            break;
        case Qt::Key_Home:
            keyHome(ke);
            break;
         case Qt::Key_End:
            keyEnd(ke);
            break;
        default:
            handled = false;
    }
    return handled;
}

bool KeyboardManager::handleKeyRelease (QKeyEvent*)
{
    return false;
}

void KeyboardManager::keyUp (QKeyEvent * ke)
{
    QList<int> rows = m_listWidget->model()->selectedIndexes();

    if (rows.count() > 0)
    {
       if(rows.first() == 0 && rows.count() == 1)
          return;

        if (! (ke->modifiers() & Qt::ShiftModifier || ke->modifiers() & Qt::AltModifier))
        {
            m_listWidget->model()->clearSelection();
            m_listWidget->setAnchorRow(-1);
        }

        bool select_top = false;
        int first_visible = m_listWidget->firstVisibleRow();
        int last_visible = m_listWidget->visibleRows() + first_visible - 1;
        foreach (int i, rows)
        {
            if (i > last_visible || i < first_visible)
            {
                select_top = true;
                break;
            }
        }

        if (!select_top || ke->modifiers() & Qt::ShiftModifier || ke->modifiers() & Qt::AltModifier)
        {
            if (ke->modifiers() == Qt::AltModifier)
            {
                if(rows.first() == 0)
                    return;
                m_listWidget->model()->moveItems (rows.first(),rows.first() - 1);
                m_listWidget->setAnchorRow (m_listWidget->anchorRow() - 1);
            }
            else
            {
                if (rows.last() > m_listWidget->anchorRow() && ke->modifiers() & Qt::ShiftModifier)
                {
                    m_listWidget->model()->setSelected (rows.last(),false);
                }
                else if (rows.first() > 0)
                {
                    m_listWidget->model()->setSelected (rows.first() - 1,true);
                }
                else
                {
                    m_listWidget->model()->setSelected (rows.first(),true);
                    if(m_listWidget->anchorRow() == -1)
                     m_listWidget->setAnchorRow(rows.first());
                }

                if (! (ke->modifiers() & Qt::ShiftModifier) && rows.first() > 0)
                    m_listWidget->setAnchorRow (rows.first() - 1);
            }
        }
        else
        {
            m_listWidget->model()->setSelected (m_listWidget->firstVisibleRow(),true);
            m_listWidget->setAnchorRow(m_listWidget->firstVisibleRow());
        }

        rows = m_listWidget->model()->selectedIndexes();

        if (rows.first()  < m_listWidget->firstVisibleRow() && m_listWidget->firstVisibleRow() > 0)
        {
          int r = rows.last() > m_listWidget->anchorRow() ? rows.last(): rows.first();
          if(ke->modifiers() & Qt::ShiftModifier && (r >= m_listWidget->firstVisibleRow()))
              ;
            else
                m_listWidget->scroll (m_listWidget->firstVisibleRow() - 1);
        }
    }
    else
    {
       //if(m_listWidget->getAnchorRow() == -1)
          m_listWidget->setAnchorRow(m_listWidget->firstVisibleRow());
        m_listWidget->model()->setSelected (m_listWidget->firstVisibleRow(),true);
    }
}

void KeyboardManager::keyDown (QKeyEvent * ke)
{
    QList<int> rows = m_listWidget->model()->selectedIndexes();
    //qWarning("count: %d",rows.count());
    if (rows.count() > 0)
    {
        if (! (ke->modifiers() & Qt::ShiftModifier || ke->modifiers() & Qt::AltModifier))
        {
            m_listWidget->model()->clearSelection();
            m_listWidget->setAnchorRow(-1);
        }

        bool select_top = false;
        int first_visible = m_listWidget->firstVisibleRow();
        int last_visible = m_listWidget->visibleRows() + first_visible - 1;
        foreach (int i, rows)
        {
            if (i > last_visible || i < first_visible)
            {
                select_top = true;
                break;
            }
        }

        if (!select_top || ke->modifiers() & Qt::ShiftModifier || ke->modifiers() & Qt::AltModifier)
        {
            if (ke->modifiers() == Qt::AltModifier)
            {
                if(rows.last() == m_listWidget->model()->count() - 1)
                    return;
                m_listWidget->model()->moveItems (rows.last(),rows.last() + 1);
                m_listWidget->setAnchorRow (m_listWidget->anchorRow() + 1);
            }
            else
            {
               //qWarning("list_widget %d",m_listWidget->getAnchorRow());
                //qWarning("model count: %d rows.last(): %d",m_listWidget->model()->count(),rows.last());
                if (rows.first() < m_listWidget->anchorRow()  && ke->modifiers() & Qt::ShiftModifier)
                    m_listWidget->model()->setSelected (rows.first(),false);
                else if (rows.last() < m_listWidget->model()->count() - 1)
                {
                    m_listWidget->model()->setSelected (rows.last() + 1,true);
                }
                else
                {
                    m_listWidget->model()->setSelected (rows.last(),true);
                    if(m_listWidget->anchorRow() == -1)
                       m_listWidget->setAnchorRow(rows.last());
                }

                if (! (ke->modifiers() & Qt::ShiftModifier) && rows.last() < m_listWidget->model()->count() - 1)
                    m_listWidget->setAnchorRow (rows.last() + 1);
            }
        }
        else
        {
            m_listWidget->model()->setSelected (m_listWidget->firstVisibleRow(),true);
            m_listWidget->setAnchorRow(m_listWidget->firstVisibleRow());
        }

        rows = m_listWidget->model()->selectedIndexes();

        if (!rows.isEmpty() && rows.last() >= m_listWidget->visibleRows() + m_listWidget->firstVisibleRow())
        {
            int r = rows.first() < m_listWidget->anchorRow() ? rows.first(): rows.last();
            if(ke->modifiers() & Qt::ShiftModifier &&
               (r < m_listWidget->firstVisibleRow() + m_listWidget->visibleRows()))
              ;
            else
                m_listWidget->scroll (m_listWidget->firstVisibleRow() + 1);
        }
    }
    else
    {
        m_listWidget->model()->setSelected (m_listWidget->firstVisibleRow(),true);
        //if(m_listWidget->getAnchorRow() == -1)
           m_listWidget->setAnchorRow(m_listWidget->firstVisibleRow());
    }
}

void KeyboardManager::keyPgUp (QKeyEvent *)
{
    int page_size = m_listWidget->visibleRows();
    int offset= (m_listWidget->firstVisibleRow()-page_size >= 0) ?m_listWidget->firstVisibleRow()-page_size:0;
    m_listWidget->scroll (offset);
}

void KeyboardManager::keyPgDown (QKeyEvent *)
{
    int page_size = m_listWidget->visibleRows();
    int offset = (m_listWidget->firstVisibleRow() +page_size < m_listWidget->model()->count()) ?
                 m_listWidget->firstVisibleRow() +page_size:m_listWidget->model()->count() - 1;
    m_listWidget->scroll (offset);
}

void KeyboardManager::keyEnter (QKeyEvent *)
{
    QList<int> rows = m_listWidget->model()->selectedIndexes();
    if (rows.count() > 0)
    {
        m_listWidget->model()->setCurrent (rows.first());
        MediaPlayer::instance()->stop();
        PlayListManager::instance()->activatePlayList(m_listWidget->model());
        MediaPlayer::instance()->play();
    }
}

void KeyboardManager::keyHome(QKeyEvent *ke)
{
    m_listWidget->scroll (0);
    if(ke->modifiers() & Qt::ShiftModifier)
    {
       for(int i = 0; i <= m_listWidget->anchorRow(); ++i)
           m_listWidget->model()->setSelected (i, true);
    }
}

void KeyboardManager::keyEnd(QKeyEvent *ke)
{
   int page_size = m_listWidget->visibleRows();
   int scroll_to = m_listWidget->model()->count() - page_size;
   if(scroll_to >= 0)
       m_listWidget->scroll(scroll_to);
   if(ke->modifiers() & Qt::ShiftModifier)
   {
       for(int i = m_listWidget->anchorRow(); i < m_listWidget->model()->count(); ++i)
           m_listWidget->model()->setSelected (i, true);
   }
}
