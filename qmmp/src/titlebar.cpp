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
    skin = Skin::getPointer();
    setPixmap(skin->getTitleBar(Skin::TITLEBAR_A));
    mw = qobject_cast<MainWindow*>(parent);
    //buttons
    menu = new Button(this,Skin::BT_MENU_N,Skin::BT_MENU_P);
    connect(menu,SIGNAL(clicked()),this,SLOT(showMainMenu()));
    menu->move(6,3);
    minimize = new Button(this,Skin::BT_MINIMIZE_N,Skin::BT_MINIMIZE_P);
    minimize->move(244,3);
    connect(minimize, SIGNAL(clicked()), mw, SLOT(showMinimized()));
    shade = new Button(this,Skin::BT_SHADE1_N,Skin::BT_SHADE1_P);
    shade->move(254,3);
    close = new Button(this,Skin::BT_CLOSE_N,Skin::BT_CLOSE_P);
    close->move(264,3);
	 connect(close, SIGNAL(clicked()), mw, SLOT(handleCloseRequest()));
    setActive(FALSE);
    connect(skin, SIGNAL(skinChanged()), this, SLOT(updateSkin()));
}


TitleBar::~TitleBar()
{}



void TitleBar::mousePressEvent(QMouseEvent* event)
{
    switch((int) event->button ())
    {
         case Qt::LeftButton:
             {
                 pos = event->pos();
                 PlayList *pl = mw->getPLPointer();
                 Dock::getPointer()->calculateDistances();
                 x_diff = - mw->x() + pl->x();
                 y_diff = - mw->y() + pl->y();
                 break;
             }
         case Qt::RightButton:
             {
                 mw->menu()->exec(event->globalPos());
             }
    }
}

void TitleBar::mouseReleaseEvent(QMouseEvent*)
{
    Dock::getPointer()->updateDock();
}
void TitleBar::mouseMoveEvent(QMouseEvent* event)
{
    QPoint npos = (event->globalPos()-pos);
    Dock::getPointer()->move(mw, npos);
}

void TitleBar::setActive(bool a)
{
    if(a)
    {
        setPixmap(skin->getTitleBar(Skin::TITLEBAR_A));
        menu->show();
        minimize->show();
        shade->show();
        close->show();
    }
    else
    {
        setPixmap(skin->getTitleBar(Skin::TITLEBAR_I));
        menu->hide();
        minimize->hide();
        shade->hide();
        close->hide();
    }
}

void TitleBar::updateSkin()
{
    setActive(FALSE);
}

void TitleBar::showMainMenu()
{
    mw->menu()->exec(menu->mapToGlobal(menu->pos()));
}
