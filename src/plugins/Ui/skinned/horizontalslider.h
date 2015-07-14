/***************************************************************************
 *   Copyright (C) 2015 by Ilya Kotov                                      *
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
#ifndef HORIZONTALSLIDER_H
#define HORIZONTALSLIDER_H

#include <QWidget>
#include <QColor>

class Skin;
class PixmapWidget;
/**
    @author Ilya Kotov <forkotov02@hotmail.ru>
*/
class HorizontalSlider : public QWidget
{
Q_OBJECT
public:
    HorizontalSlider(QWidget *parent = 0);

    ~HorizontalSlider();

public slots:
    void setPos(int pos, int max);

signals:
    void sliderMoved (int);

private slots:
    void updateSkin();

private:
    Skin *m_skin;
    PixmapWidget *m_scroll;
    int m_old;
    bool m_moving, m_pressed;
    int press_pos;
    int m_min, m_max, m_value, pos, m_pos;
    int convert(int);   // value = convert(position);
    QColor m_normal, m_normal_bg;

protected:
    void paintEvent(QPaintEvent*);
    void mousePressEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);
    void mouseMoveEvent(QMouseEvent*);
};

#endif
