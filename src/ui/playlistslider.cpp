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
#include <QPainter>
#include <QResizeEvent>
#include <math.h>

#include "skin.h"
#include "playlistslider.h"
#include "pixmapwidget.h"

PlayListSlider::PlayListSlider(QWidget *parent)
        : QWidget(parent)
{
    m_skin = Skin::getPointer();

    m_moving = FALSE;
    m_pressed = FALSE;
    m_min = 0;
    m_max = 0;
    m_value = 0;
    pos = 0;
    connect(m_skin, SIGNAL(skinChanged()), this, SLOT(updateSkin()));
}


PlayListSlider::~PlayListSlider()
{}

void PlayListSlider::paintEvent(QPaintEvent *)
{
    int sy = (height()-58)/29;
    int p=int(ceil(double(m_value-m_min)*(height()-18)/(m_max-m_min)));
    QPainter paint(this);
    paint.drawPixmap(0,0,m_skin->getPlPart(Skin::PL_RFILL));
    paint.drawPixmap(0,29,m_skin->getPlPart(Skin::PL_RFILL));

    for (int i = 0; i<sy; i++)
    {
        paint.drawPixmap(0,58+i*29,m_skin->getPlPart(Skin::PL_RFILL));
    }
    if (m_pressed)
        paint.drawPixmap(5,p,m_skin->getButton(Skin::PL_BT_SCROLL_P));
    else
        paint.drawPixmap(5,p,m_skin->getButton(Skin::PL_BT_SCROLL_N));
    m_pos = p;
}

void PlayListSlider::mousePressEvent(QMouseEvent *e)
{

    m_moving = TRUE;
    m_pressed = TRUE;
    press_pos = e->y();
    if (m_pos<e->y() && e->y()<m_pos+18)
    {
        press_pos = e->y()-m_pos;
    }
    else
    {
        m_value = convert(qMax(qMin(height()-18,e->y()-9),0));
        press_pos = 9;
        if (m_value!=m_old)
        {
            emit sliderMoved(m_value);
            m_old = m_value;
            //qDebug ("%d",m_value);
        }
    }
    update();
}

void PlayListSlider::mouseReleaseEvent(QMouseEvent*)
{
    m_moving = FALSE;
    m_pressed = FALSE;
    update();
}

void PlayListSlider::mouseMoveEvent(QMouseEvent* e)
{
    if (m_moving)
    {
        int po = e->y();
        po = po - press_pos;

        if (0<=po && po<=height()-18)
        {
            m_value = convert(po);
            update();
            if (m_value!=m_old)
            {

                m_old = m_value;
                emit sliderMoved(m_value);
            }
        }
    }
}

void PlayListSlider::setPos(int p, int max)
{
    m_max = max;
    m_value = p;
    if(m_moving)
        return;
    update();
}

void PlayListSlider::updateSkin()
{
    update();
}

int PlayListSlider::convert(int p)
{
    return int(floor(double(m_max-m_min)*(p)/(height()-18)+m_min));
}

