/***************************************************************************
 *   Copyright (C) 2011-2015 by Ilya Kotov                                 *
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

#include <QMouseEvent>
#include <QWheelEvent>
#include <QStyleOptionSlider>
#include <QStyle>
#include "positionslider.h"

PositionSlider::PositionSlider(QWidget *parent) : QSlider(Qt::Horizontal, parent)
{}

void PositionSlider::mousePressEvent (QMouseEvent *event)
{
    QStyleOptionSlider opt;
    initStyleOption(&opt);
    QRect sr = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);

    if (event->button() == Qt::LeftButton &&
            sr.contains(event->pos()) == false)
    {
        int val;
        if (orientation() == Qt::Vertical)
            val = minimum() + ((maximum() - minimum()) * (height() - event->y())) / height();
        else if(layoutDirection() == Qt::RightToLeft)
            val = maximum() - ((maximum() - minimum()) * event->x()) / width();
        else
            val = minimum() + ((maximum() - minimum()) * event->x()) / width();

        if (invertedAppearance() == true)
            setValue(maximum() - val);
        else
            setValue(val);

        setSliderDown (true);
        event->accept();
    }
    QSlider::mousePressEvent(event);
}

void PositionSlider::mouseReleaseEvent (QMouseEvent *)
{
    setSliderDown (false);
}

void PositionSlider::wheelEvent(QWheelEvent *event)
{
    setValue(value() + event->delta() / 20);
    sliderReleased();
}
