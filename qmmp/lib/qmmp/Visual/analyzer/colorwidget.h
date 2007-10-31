/***************************************************************************
 *   Copyright (C) 2005 by Ilya Kotov                                      *
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
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef COLORWIDGET_H
#define COLORWIDGET_H

#include <QFrame>
#include <QColorDialog>
#include <QPaintEvent>

/**
@author user
*/
class ColorWidget : public QFrame
{
    Q_OBJECT
public:
    ColorWidget(QWidget *parent = 0);

    ~ColorWidget();

    QString colorName();

public slots:
    void setColor (QString);


protected:
    virtual void mousePressEvent ( QMouseEvent *);


};

#endif
