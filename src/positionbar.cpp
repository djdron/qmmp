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
#include <QPainter>
#include <math.h>

#include "skin.h"
#include "button.h"
#include "mainwindow.h"

#include "positionbar.h"


PositionBar::PositionBar(QWidget *parent)
        : PixmapWidget(parent)
{
    m_skin = Skin::getPointer();
    connect(m_skin, SIGNAL(skinChanged()), this, SLOT(updateSkin()));
    setPixmap(m_skin->getPosBar());
    mw = qobject_cast<MainWindow*>(window());
    m_moving = FALSE;
    m_min = 0;
    m_max = 50;
    m_old = m_value = 0;
    draw(FALSE);
}


PositionBar::~PositionBar()
{}

void PositionBar::mousePressEvent(QMouseEvent *e)
{

    m_moving = TRUE;
    press_pos = e->x();
    if(m_pos<e->x() && e->x()<m_pos+29)
    {
        press_pos = e->x()-m_pos;
    }
    else
    {
        m_value = convert(qMax(qMin(width()-30,e->x()-15),0));
        press_pos = 15;
        if (m_value!=m_old)
        {
            emit sliderMoved(m_value);

        }
    }
    draw();
}

void PositionBar::mouseMoveEvent (QMouseEvent *e)
{
    if(m_moving)
    {
        int po = e->x();
        po = po - press_pos;

        if(0<=po && po<=width()-30)
        {
            m_value = convert(po);
            draw();
            emit sliderMoved(m_value);
        }
    }
}

void PositionBar::mouseReleaseEvent(QMouseEvent*)
{
    m_moving = FALSE;
    draw(FALSE);
    if (m_value!=m_old)
    {
        m_old = m_value;
        mw->seek(m_value);
    }

}

void PositionBar::setValue(int v)
{
    if (m_moving || m_max == 0)
        return;
    m_value = v;
    draw(FALSE);
}

void PositionBar::setMax(int max)
{
    m_max = max;
    draw(FALSE);
}

void PositionBar::updateSkin()
{
    draw(FALSE);
    //setPixmap(m_skin->getPosBar());
    //setButtonPixmap(Skin::BT_POSBAR_N);
}

void PositionBar::draw(bool pressed)
{
    int p=int(ceil(double(m_value-m_min)*(width()-30)/(m_max-m_min)));
    m_pixmap = m_skin->getPosBar();
    QPainter paint(&m_pixmap);
    if(pressed)
        paint.drawPixmap(p,0,m_skin->getButton(Skin::BT_POSBAR_P));
    else
        paint.drawPixmap(p,0,m_skin->getButton(Skin::BT_POSBAR_N));
    setPixmap(m_pixmap);
    m_pos = p;
}

int PositionBar::convert(int p)
{
    return int(ceil(double(m_max-m_min)*(p)/(width()-30)+m_min));
}
