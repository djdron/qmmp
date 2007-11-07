/***************************************************************************
 *   Copyright (C) 2007 by Ilya Kotov                                      *
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

#include <QMainWindow>
#include <QApplication>
#include <QMouseEvent>
#include <QMenu>

#include "titlebar.h"
#include "skin.h"
#include "button.h"
#include "dock.h"

TitleBar::TitleBar(QWidget *parent)
        : PixmapWidget(parent)
{
    m_skin = Skin::getPointer();
    setPixmap(m_skin->getTitleBar(Skin::TITLEBAR_A));
    m_mw = qobject_cast<MainWindow*>(parent);
    //buttons
    m_menu = new Button(this,Skin::BT_MENU_N,Skin::BT_MENU_P);
    connect(m_menu,SIGNAL(clicked()),this,SLOT(showMainMenu()));
    m_menu->move(6,3);
    m_minimize = new Button(this,Skin::BT_MINIMIZE_N,Skin::BT_MINIMIZE_P);
    m_minimize->move(244,3);
    connect(m_minimize, SIGNAL(clicked()), m_mw, SLOT(showMinimized()));
    m_shade = new Button(this,Skin::BT_SHADE1_N,Skin::BT_SHADE1_P);
    m_shade->move(254,3);
    m_close = new Button(this,Skin::BT_CLOSE_N,Skin::BT_CLOSE_P);
    m_close->move(264,3);
    connect(m_close, SIGNAL(clicked()), m_mw, SLOT(handleCloseRequest()));
    setActive(FALSE);
    connect(m_skin, SIGNAL(skinChanged()), this, SLOT(updateSkin()));
}


TitleBar::~TitleBar()
{}



void TitleBar::mousePressEvent(QMouseEvent* event)
{
    switch((int) event->button ())
    {
         case Qt::LeftButton:
             {
                 m_pos = event->pos();
                 PlayList *pl = m_mw->getPLPointer();
                 Dock::getPointer()->calculateDistances();
                 x_diff = - m_mw->x() + pl->x();
                 y_diff = - m_mw->y() + pl->y();
                 break;
             }
         case Qt::RightButton:
             {
                 m_mw->menu()->exec(event->globalPos());
             }
    }
}

void TitleBar::mouseReleaseEvent(QMouseEvent*)
{
    Dock::getPointer()->updateDock();
}
void TitleBar::mouseMoveEvent(QMouseEvent* event)
{
    QPoint npos = (event->globalPos()-m_pos);
    Dock::getPointer()->move(m_mw, npos);
}

void TitleBar::setActive(bool a)
{
    if(a)
    {
        setPixmap(m_skin->getTitleBar(Skin::TITLEBAR_A));
        m_menu->show();
        m_minimize->show();
        m_shade->show();
        m_close->show();
    }
    else
    {
        setPixmap(m_skin->getTitleBar(Skin::TITLEBAR_I));
        m_menu->hide();
        m_minimize->hide();
        m_shade->hide();
        m_close->hide();
    }
}

void TitleBar::updateSkin()
{
    setActive(FALSE);
}

void TitleBar::showMainMenu()
{
    m_mw->menu()->exec(m_menu->mapToGlobal(m_menu->pos()));
}
