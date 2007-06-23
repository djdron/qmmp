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
#include <QMouseEvent>
#include <QMenu>

#include "skin.h"
#include "dock.h"
#include "mainwindow.h"

#include "eqtitlebar.h"

EqTitleBar::EqTitleBar(QWidget *parent)
        : PixmapWidget(parent)
{
    m_skin = Skin::getPointer();
    setActive(FALSE);
    m_eq = parentWidget();
    m_mw = qobject_cast<MainWindow*>(m_eq->parent());
}


EqTitleBar::~EqTitleBar()
{}

void EqTitleBar::setActive(bool active)
{
    if (active)
       setPixmap(m_skin->getEqPart(Skin::EQ_TITLEBAR_A));
    else
       setPixmap(m_skin->getEqPart(Skin::EQ_TITLEBAR_I));
}

void EqTitleBar::mousePressEvent(QMouseEvent* event)
{
    switch((int) event->button ())
    {
         case Qt::LeftButton:
             {
                 m_pos = event->pos();
                 break;
             }
         case Qt::RightButton:
             {
                 m_mw->menu()->exec(event->globalPos());
             }
    }
}

void EqTitleBar::mouseMoveEvent(QMouseEvent* event)
{
    QPoint npos = (event->globalPos()-m_pos);
    //parentWidget()->move(npos);
    Dock::getPointer()->move(m_eq, npos);
}

void EqTitleBar::mouseReleaseEvent(QMouseEvent*)
{
    Dock::getPointer()->updateDock();
}
