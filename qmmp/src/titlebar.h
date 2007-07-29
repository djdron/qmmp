/***************************************************************************
 *   Copyright (C) 2006 by Ilya Kotov                                       *
 *   forkotov02@hotmail.ru                                                     *
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
#ifndef TITLEBAR_H
#define TITLEBAR_H

#include <QMainWindow>
#include "pixmapwidget.h"

#include "playlist.h"
#include <QPoint>
#include "mainwindow.h"
/**
	@author Ilya Kotov <forkotov02@hotmail.ru>
*/
class MainWindow;
class QMouseEvent;

class Skin;
class Button;


class TitleBar : public PixmapWidget
{
Q_OBJECT
public:
    TitleBar(QWidget *parent = 0);

    ~TitleBar();

    void setActive(bool);

private slots:
    void updateSkin();
    void showMainMenu();

private:
    Skin *skin;
    QPoint pos;
    MainWindow *mw;
    Button *menu;
    Button *minimize;
    Button *shade;
    Button *close;
    int x_diff, y_diff;
    

protected:
    void mousePressEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);
    void mouseMoveEvent(QMouseEvent*);
};



#endif
