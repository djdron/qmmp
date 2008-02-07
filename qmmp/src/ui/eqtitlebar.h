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
#ifndef EQTITLEBAR_H
#define EQTITLEBAR_H

#include <pixmapwidget.h>

/**
	@author Ilya Kotov <forkotov02@hotmail.ru>
*/

class QMouseEvent;

class Skin;
class MainWindow;
class Button;
class ShadedBar;

class EqTitleBar : public PixmapWidget
{
Q_OBJECT
public:
    EqTitleBar(QWidget *parent = 0);

    ~EqTitleBar();

    void setActive(bool);
    void setVolume(int left, int right);

private slots:
    void shade();
    void updateVolume();

private:
    Skin* m_skin;
    bool m_active;
    int m_left;
    int m_right;
    QPoint m_pos;
    QWidget* m_eq;
    MainWindow* m_mw;
    Button* m_close;
    Button* m_shade;
    Button* m_shade2;
    bool m_shaded, m_align;
    ShadedBar* m_volumeBar;
    ShadedBar* m_balanceBar;

protected:
    void mousePressEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);
    void mouseMoveEvent(QMouseEvent*);

};

#endif
