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

#include <QMouseEvent>

#include "titlebarcontrol.h"

TitleBarControl::TitleBarControl(QWidget *parent)
 : QWidget(parent)
{
    //setAutoFillBackground(TRUE);
    setFixedSize(57,10);
}


TitleBarControl::~TitleBarControl()
{
}

void TitleBarControl::mousePressEvent (QMouseEvent *)
{}

void TitleBarControl::mouseReleaseEvent (QMouseEvent * event)
{
    QPoint pt = event->pos();
    if(QRect(0,0,8,10).contains(pt))
        emit previousClicked();
    else if(QRect(8,0,11,10).contains(pt))
        emit playClicked();
    else if(QRect(19,0,10,10).contains(pt))
        emit pauseClicked();
    else if(QRect(29,0,8,10).contains(pt))
        emit stopClicked();
    else if(QRect(37,0,10,10).contains(pt))
        emit nextClicked();
    else if(QRect(47,0,10,10).contains(pt))
        emit ejectClicked();
}

void TitleBarControl::mouseMoveEvent(QMouseEvent*)
{}
