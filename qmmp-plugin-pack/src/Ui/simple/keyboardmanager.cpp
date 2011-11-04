/***************************************************************************
 *   Copyright (C) 2011 by Ilya Kotov                                      *
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

#include <QAction>
#include <QKeySequence>
#include <qmmpui/playlistmanager.h>
#include <qmmpui/playlistmodel.h>
#include <qmmpui/mediaplayer.h>
#include <qmmp/soundcore.h>
#include "listwidget.h"
#include "keyboardmanager.h"

KeyboardManager::KeyboardManager(QObject *parent) :
    QObject(parent)
{
    m_listWidget = 0;

    addAction(Qt::Key_Up, SLOT(processUp()));
    addAction(Qt::Key_Up + Qt::ShiftModifier, SLOT(processUp()));
    addAction(Qt::Key_Up + Qt::AltModifier, SLOT(processUp()));

    addAction(Qt::Key_Down, SLOT(processDown()));
    addAction(Qt::Key_Down + Qt::ShiftModifier, SLOT(processDown()));
    addAction(Qt::Key_Down + Qt::AltModifier, SLOT(processDown()));

    addAction(Qt::Key_Return, SLOT(processEnter()));
    addAction(Qt::Key_PageUp, SLOT(processPgUp()));
    addAction(Qt::Key_PageDown, SLOT(processPgDown()));
    addAction(Qt::Key_Home, SLOT(processHome()));
    addAction(Qt::Key_End, SLOT(processEnd()));
}

QList<QAction *> KeyboardManager::actions()
{
    return m_actions;
}

void KeyboardManager::processUp()
{
    if(!m_listWidget)
        return;

    int keys = qobject_cast<QAction *>(sender())->shortcut()[0];

    QList<int> rows = m_listWidget->model()->selectedRows();
    int first_visible = m_listWidget->firstVisibleRow();
    int last_visible = m_listWidget->visibleRows() + first_visible - 1;
    if(rows.isEmpty())
    {
        m_listWidget->setAnchorRow(first_visible);
        m_listWidget->model()->setSelected(first_visible, true);
    }
    else
    {
        int select_row = rows.first();

        if(select_row > 0)
            select_row--;

        if(keys & Qt::ShiftModifier)
        {
            if(m_listWidget->anchorRow() != rows.last())
            {
                select_row = rows.last();
                if(select_row == first_visible)
                    m_listWidget->scroll(select_row - 1);
            }
            else if(select_row < first_visible)
                m_listWidget->scroll(select_row);
            m_listWidget->model()->setSelected(select_row, m_listWidget->anchorRow() == rows.last());
        }
        else if(keys & Qt::AltModifier)
        {
            if(rows.first() == 0)
                return;
            m_listWidget->model()->moveItems (rows.first(),rows.first() - 1);
            m_listWidget->setAnchorRow (m_listWidget->anchorRow() - 1);
            if(select_row < first_visible)
                m_listWidget->scroll(select_row);
        }
        else
        {
            m_listWidget->model()->clearSelection();
            foreach(int row, rows)
            {
                if(row < first_visible || row > last_visible)
                {
                    m_listWidget->setAnchorRow(first_visible);
                    m_listWidget->model()->setSelected(first_visible, true);
                    return;
                }
            }
            if(select_row < first_visible)
                m_listWidget->scroll(select_row);
            if(!(keys & Qt::ShiftModifier))
                m_listWidget->setAnchorRow(select_row);
            m_listWidget->model()->setSelected(select_row, true);
        }
    }
}

void KeyboardManager::processDown()
{
    if(!m_listWidget)
        return;

    int keys = qobject_cast<QAction *>(sender())->shortcut()[0];

    QList<int> rows = m_listWidget->model()->selectedRows();
    int first_visible = m_listWidget->firstVisibleRow();
    int last_visible = m_listWidget->visibleRows() + first_visible - 1;
    if(rows.isEmpty())
    {
        m_listWidget->setAnchorRow(first_visible);
        m_listWidget->model()->setSelected(first_visible, true);
    }
    else
    {
        int select_row = rows.last();

        if(select_row < m_listWidget->model()->count() - 1)
            select_row++;

        if(keys & Qt::ShiftModifier)
        {
            if(m_listWidget->anchorRow() != rows.first())
            {
                select_row = rows.first();
                if(select_row == last_visible)
                    m_listWidget->scroll(first_visible + 1);
            }
            else if(select_row > last_visible)
                m_listWidget->scroll(first_visible + 1);
            m_listWidget->model()->setSelected(select_row, m_listWidget->anchorRow() == rows.first());
        }
        else if(keys & Qt::AltModifier)
        {
            if(rows.last() == m_listWidget->model()->count() - 1)
                return;
            m_listWidget->model()->moveItems (rows.last(),rows.last() + 1);
            m_listWidget->setAnchorRow (m_listWidget->anchorRow() + 1);
            if(select_row > last_visible)
                m_listWidget->scroll(first_visible + 1);
        }
        else
        {
            m_listWidget->model()->clearSelection();
            foreach(int row, rows)
            {
                if(row < first_visible || row > last_visible)
                {
                    m_listWidget->setAnchorRow(first_visible);
                    m_listWidget->model()->setSelected(first_visible, true);
                    return;
                }
            }
            if(select_row > last_visible)
                m_listWidget->scroll(first_visible + 1);
            m_listWidget->setAnchorRow(select_row);
            m_listWidget->model()->setSelected(select_row, true);
        }
    }
}

void KeyboardManager::setListWidget(ListWidget *listWidget)
{
    m_listWidget = listWidget;
}

void KeyboardManager::processEnter()
{
    if(!m_listWidget)
        return;
    QList<int> rows = m_listWidget->model()->selectedRows();
    if(rows.isEmpty())
        return;
    SoundCore::instance()->stop();
    PlayListManager::instance()->activatePlayList(m_listWidget->model());
    m_listWidget->model()->setCurrent (rows.first());
    MediaPlayer::instance()->play();
}

void KeyboardManager::processPgUp()
{
    if(!m_listWidget)
        return;

    int offset = qMax(m_listWidget->firstVisibleRow() - m_listWidget->visibleRows(), 0);
    m_listWidget->scroll (offset);
}

void KeyboardManager::processPgDown()
{
    if(!m_listWidget)
        return;

    int offset = qMin(m_listWidget->firstVisibleRow() + m_listWidget->visibleRows(),
                      m_listWidget->model()->count() - 1);
    m_listWidget->scroll (offset);
}

void KeyboardManager::processHome()
{
    if(!m_listWidget)
        return;
    m_listWidget->scroll (0);
}

void KeyboardManager::processEnd()
{
    if(!m_listWidget)
        return;
    m_listWidget->scroll (0);

    int scroll_to = m_listWidget->model()->count() - m_listWidget->visibleRows();
    if(scroll_to >= 0)
        m_listWidget->scroll(scroll_to);
}

void KeyboardManager::addAction(int keys, const char *method)
{
    QAction *action = new QAction(this);
    action->setShortcut(QKeySequence(keys));
    connect(action, SIGNAL(triggered()), method);
    m_actions << action;
}
