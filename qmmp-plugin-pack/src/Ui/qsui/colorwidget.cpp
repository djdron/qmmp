/***************************************************************************
 *   Copyright (C) 2005-2012 by Ilya Kotov                                 *
 *   qmmeter_freedevelop@mail.ru                                           *
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

#include <QColorDialog>
#include <QPalette>
#include "colorwidget.h"

ColorWidget::ColorWidget(QWidget *parent) : QFrame(parent)
{
    setFrameShape(QFrame::Box);
    setAutoFillBackground(true);
}

ColorWidget::~ColorWidget()
{}

void ColorWidget::mousePressEvent(QMouseEvent *)
{
    QColor color = QColorDialog::getColor(palette().color(backgroundRole()), parentWidget(),
                                          tr("Select Color"));
    if (color.isValid())
    {
        QPalette palette;
        palette.setColor(backgroundRole(), color);
        setPalette(palette);
    }
}

void ColorWidget::setColor(QString c)
{
    QPalette palette;
    palette.setColor(backgroundRole(), c);
    setPalette(palette);
}

const QString ColorWidget::colorName() const
{
    return palette().color(backgroundRole()).name();
}
