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
#include <QMenu>

#include "dock.h"
#include "playlisttitlebar.h"
#include "skin.h"

PlayListTitleBar::PlayListTitleBar(QWidget *parent)
        : PixmapWidget(parent)
{
    m_active = FALSE;
    m_skin = Skin::getPointer();
    resize(275,20);
    setSizeIncrement(25,1);
    connect(m_skin, SIGNAL(skinChanged()), this, SLOT(updateSkin()));
    m_pl = qobject_cast<PlayList*>(parent);
    m_mw = qobject_cast<MainWindow*>(m_pl->parent());
}


PlayListTitleBar::~PlayListTitleBar()
{}
void PlayListTitleBar::drawPixmap(int sx)
{
    QPixmap pixmap(275+sx*25,20);
    pixmap.fill("black");
    QPainter paint;
    paint.begin(&pixmap);
    if (m_active)
    {
        paint.drawPixmap(0,0,m_skin->getPlPart(Skin::PL_CORNER_UL_A));
        for (int i = 1; i<sx+10; i++)
        {
           paint.drawPixmap(25*i,0,m_skin->getPlPart(Skin::PL_TFILL1_A));
        }
        paint.drawPixmap(100-12+12*sx,0,m_skin->getPlPart(Skin::PL_TITLEBAR_A));
        paint.drawPixmap(250+sx*25,0,m_skin->getPlPart(Skin::PL_CORNER_UR_A));
    }
    else
    {
        paint.drawPixmap(0,0,m_skin->getPlPart(Skin::PL_CORNER_UL_I));
        for (int i = 1; i<sx+10; i++)
        {
           paint.drawPixmap(25*i,0,m_skin->getPlPart(Skin::PL_TFILL1_I));
        }
        paint.drawPixmap(100-12+12*sx,0,m_skin->getPlPart(Skin::PL_TITLEBAR_I));
        paint.drawPixmap(250+sx*25,0,m_skin->getPlPart(Skin::PL_CORNER_UR_I));
    }
    paint.end();
    setPixmap(pixmap);
}

void PlayListTitleBar::resizeEvent(QResizeEvent *e)
{
    int m_sx = (e->size().width()-275)/25;
    drawPixmap(m_sx);

}

void PlayListTitleBar::mousePressEvent(QMouseEvent* event)
{
    switch((int) event->button ())
    {
         case Qt::LeftButton:
             {
                 pos = event->pos();
                 break;
             }
         case Qt::RightButton:
             {
                 m_mw->menu()->exec(event->globalPos());
             }
    }
}

void PlayListTitleBar::mouseReleaseEvent(QMouseEvent*)
{
   Dock::getPointer()->updateDock();
}

void PlayListTitleBar::mouseMoveEvent(QMouseEvent* event)
{ 
    QPoint npos = (event->globalPos()-pos);
    QPoint oldpos = npos;
    Dock::getPointer()->move(m_pl, npos);
}

void PlayListTitleBar::setActive(bool a)
{
    m_active = a;
    int m_sx = (width()-275)/25;
    drawPixmap(m_sx);
}

void PlayListTitleBar::updateSkin()
{
    drawPixmap((width()-275)/25);
}
