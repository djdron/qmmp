/***************************************************************************
 *   Copyright (C) 2012 by Ilya Kotov                                      *
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

#include <QTabBar>
#include "qsuitabbar.h"
#include "qsuitabwidget.h"

QSUiTabWidget::QSUiTabWidget(QWidget *parent) : QTabWidget(parent)
{
    setTabBar(new QSUiTabBar(this));
    setMovable(true);
    connect(tabBar(), SIGNAL(tabMoved(int,int)), SIGNAL(tabMoved(int,int)));
    connect(tabBar(), SIGNAL(tabCloseRequested(int)), SLOT(onTabCloseRequest(int)));
}

void QSUiTabWidget::onTabCloseRequest(int i)
{
    if(!tabsClosable())
        emit tabCloseRequested(i);
}


void QSUiTabWidget::setTabsVisible(bool visible)
{
    tabBar()->setVisible(visible);
}
