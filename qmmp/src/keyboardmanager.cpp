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


#include <QKeyEvent>

#include "playlist.h"
#include "playlistmodel.h"
#include "listwidget.h"
#include "keyboardmanager.h"
#include "mainwindow.h"


KeyboardManager::KeyboardManager ( PlayList* pl )
{
    m_playlist = pl;
}

bool KeyboardManager::handleKeyPress ( QKeyEvent* ke )
{
    bool handled = TRUE;
    switch ( ke->key() )
    {
        case Qt::Key_Up:
            keyUp ( ke );
            break;
        case Qt::Key_Down:
            keyDown ( ke );
            break;
        case Qt::Key_PageUp:
            keyPgUp ( ke );
            break;
        case Qt::Key_PageDown:
            keyPgDown ( ke );
            break;
        case Qt::Key_Enter:
        case Qt::Key_Return:
            keyEnter ( ke );
        default:
            handled = FALSE;
    }
    return handled;
}

bool KeyboardManager::handleKeyRelease ( QKeyEvent* )
{
    return FALSE;
}



void KeyboardManager::setModel ( PlayListModel *m )
{
    m_playListModel = m;
}

void KeyboardManager::keyUp ( QKeyEvent * ke )
{
    QList<int> rows = m_playListModel->getSelectedRows();
    ListWidget* list_widget = m_playlist->listWidget();

    if ( rows.count() > 0 )
    {
       if(rows[0] == 0 && rows.count() == 1)
          return;

        if ( ! ( ke->modifiers() & Qt::ShiftModifier || ke->modifiers() & Qt::AltModifier ) )
        {
            m_playListModel->clearSelection();
            list_widget->setAnchorRow(-1);
        }

        bool select_top = false;
        int first_visible = list_widget->firstVisibleRow();
        int last_visible = list_widget->visibleRows() + first_visible - 1;
        foreach ( int i, rows )
        {
            if ( i > last_visible || i < first_visible )
            {
                select_top = true;
                break;
            }
        }

        if ( !select_top || ke->modifiers() & Qt::ShiftModifier || ke->modifiers() & Qt::AltModifier )
        {
            if ( ke->modifiers() == Qt::AltModifier )
            {
                m_playListModel->moveItems ( rows[0],rows[0] - 1 );
                list_widget->setAnchorRow ( list_widget->getAnchorRow() - 1 );
            }
            else
            {
                if ( rows.last() > list_widget->getAnchorRow() && ke->modifiers() & Qt::ShiftModifier )
                {
                    m_playListModel->setSelected ( rows.last(),false );
                }
                else if ( rows[0] > 0 ) 
                {
                    m_playListModel->setSelected ( rows[0] - 1,true );
                }
                else
                {
                    m_playListModel->setSelected ( rows[0],true );
                    if(list_widget->getAnchorRow() == -1)
                     list_widget->setAnchorRow(rows[0]);
                }

                if ( ! ( ke->modifiers() & Qt::ShiftModifier ) && rows[0] > 0 )
                    list_widget->setAnchorRow ( rows[0] - 1 );
            }
        }
        else
        { 
            m_playListModel->setSelected ( list_widget->firstVisibleRow(),true );
            list_widget->setAnchorRow(list_widget->firstVisibleRow());
        }

        rows = m_playListModel->getSelectedRows();

        if ( rows[0]  < list_widget->firstVisibleRow() && list_widget->firstVisibleRow() > 0 )
        {
          int r = rows.last() > list_widget->getAnchorRow() ? rows.last(): rows.first();
          if(ke->modifiers() & Qt::ShiftModifier && (r >= list_widget->firstVisibleRow() ))
              ;
            else
                list_widget->scroll ( list_widget->firstVisibleRow() - 1 );
        }
    }
    else
    {
       //if(list_widget->getAnchorRow() == -1)
          list_widget->setAnchorRow(list_widget->firstVisibleRow());
        m_playListModel->setSelected ( list_widget->firstVisibleRow(),true );
    }
}

void KeyboardManager::keyDown ( QKeyEvent * ke )
{
    QList<int> rows = m_playListModel->getSelectedRows();
    ListWidget* list_widget = m_playlist->listWidget();
    //qWarning("count: %d",rows.count());
    if ( rows.count() > 0 )
    {
        if ( ! ( ke->modifiers() & Qt::ShiftModifier || ke->modifiers() & Qt::AltModifier ) )
        {
            m_playListModel->clearSelection();
            list_widget->setAnchorRow(-1);
        }

        bool select_top = false;
        int first_visible = list_widget->firstVisibleRow();
        int last_visible = list_widget->visibleRows() + first_visible - 1;
        foreach ( int i, rows )
        {
            if ( i > last_visible || i < first_visible )
            {
                select_top = true;
                break;
            }
        }

        if ( !select_top || ke->modifiers() & Qt::ShiftModifier || ke->modifiers() & Qt::AltModifier )
        {
            if ( ke->modifiers() == Qt::AltModifier )
            {
                m_playListModel->moveItems ( rows.last(),rows.last() + 1 );
                list_widget->setAnchorRow ( list_widget->getAnchorRow() + 1 );
            }
            else
            {
               //qWarning("list_widget %d",list_widget->getAnchorRow());
                //qWarning("model count: %d rows.last(): %d",m_playListModel->count(),rows.last());
                if ( rows[0] < list_widget->getAnchorRow()  && ke->modifiers() & Qt::ShiftModifier )
                    m_playListModel->setSelected ( rows[0],false );
                else if ( rows.last() < m_playListModel->count() - 1 )
                {
                    m_playListModel->setSelected ( rows.last() + 1,true );
                }
                else
                {
                    m_playListModel->setSelected ( rows.last(),true );
                    if(list_widget->getAnchorRow() == -1)
                       list_widget->setAnchorRow(rows.last());
                }

                if ( ! ( ke->modifiers() & Qt::ShiftModifier ) && rows.last() < m_playListModel->count() - 1 )
                    list_widget->setAnchorRow ( rows.last() + 1 );
            }
        }
        else
        {
            m_playListModel->setSelected ( list_widget->firstVisibleRow(),true );
            list_widget->setAnchorRow(list_widget->firstVisibleRow());
        }

        rows = m_playListModel->getSelectedRows();

        if ( !rows.isEmpty() && rows.last() >= list_widget->visibleRows() + list_widget->firstVisibleRow() )
        {
            int r = rows.first() < list_widget->getAnchorRow() ? rows.first(): rows.last();
            if(ke->modifiers() & Qt::ShiftModifier && 
               (r < list_widget->firstVisibleRow() + list_widget->visibleRows() ))
              ;
            else
                list_widget->scroll ( list_widget->firstVisibleRow() + 1 );
        }
    }
    else
    {
        m_playListModel->setSelected ( list_widget->firstVisibleRow(),true );
        //if(list_widget->getAnchorRow() == -1)
           list_widget->setAnchorRow(list_widget->firstVisibleRow());
    }
}

void KeyboardManager::keyPgUp ( QKeyEvent * )
{
    ListWidget* list_widget = m_playlist->listWidget();
    int page_size = list_widget->visibleRows();
    int offset= ( list_widget->firstVisibleRow()-page_size >= 0 ) ?list_widget->firstVisibleRow()-page_size:0;
    list_widget->scroll ( offset );
}

void KeyboardManager::keyPgDown ( QKeyEvent * )
{
    ListWidget* list_widget = m_playlist->listWidget();
    int page_size = list_widget->visibleRows();
    int offset = ( list_widget->firstVisibleRow() +page_size < m_playListModel->count() ) ?
                 list_widget->firstVisibleRow() +page_size:m_playListModel->count() - 1;
    list_widget->scroll ( offset );
}

void KeyboardManager::keyEnter ( QKeyEvent * )
{
    QList<int> rows = m_playListModel->getSelectedRows();
    MainWindow* mw = qobject_cast<MainWindow*> ( m_playlist->parentWidget() );
    if ( mw && rows.count() > 0 )
    {
        m_playListModel->setCurrent ( rows[0] );
        mw->replay();
    }
}
