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

#include <QPainter>
#include <QMouseEvent>
#include "volumeslider.h"

VolumeSlider::VolumeSlider(QWidget *parent) : QAbstractSlider(parent)
{
    setRange(0, 100);
    setValue(50);
}

void VolumeSlider::mousePressEvent(QMouseEvent *e)
{
    emit sliderMoved(xToValue(e->x()));
    setValue(xToValue(e->x()));
    update();
}

void VolumeSlider::mouseMoveEvent(QMouseEvent *e)
{
    emit sliderMoved(xToValue(e->x()));
    setValue(xToValue(e->x()));
    update();
}

void VolumeSlider::wheelEvent(QWheelEvent *e)
{
    QAbstractSlider::wheelEvent(e);
    emit sliderMoved(value());
}

void VolumeSlider::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    //painter.setRenderHint(QPainter::Antialiasing, true);

    QPolygon polygon;
    polygon.putPoints(0, 4,
                      5, height() - 5,
                      5, height() - 8,
                      width() - 5, 5,
                      width() - 5, height() - 5);

    QPolygon polygon2;
    polygon2.putPoints(0, 4,
                       5, height(),
                       5, 0,
                       xFromValue(), 0,
                       xFromValue(), height());
    painter.drawPolygon(polygon);
    painter.setPen(Qt::NoPen);

    QColor color(palette().color(QPalette::Highlight));
    color.setAlpha(180);
    painter.setBrush(QBrush(color, Qt::Dense4Pattern));
    painter.drawPolygon(polygon.intersected(polygon2));
}

int VolumeSlider::xToValue(int x)
{
    return (x - 5) * (maximum() - minimum()) / (width() - 10) + minimum();
}

int VolumeSlider::xFromValue()
{
    return 5 + (value() - minimum()) * (width() - 10) / (maximum() - minimum());
}
