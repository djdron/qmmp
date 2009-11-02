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
#include <QPaintEvent>
#include <QMouseEvent>

#include    "playlistcontrol.h"
#include "skin.h"

PlaylistControl::PlaylistControl(QWidget* parent) : PixmapWidget(parent)
{
	m_skin = Skin::instance();
	connect(m_skin, SIGNAL(skinChanged()), this, SLOT(updateSkin()));
}

void PlaylistControl::paintEvent(QPaintEvent *)
{
	QPainter painter(this);
	painter.drawPixmap(0,0,m_skin->getPlPart(Skin::PL_CONTROL));
}

void PlaylistControl::mouseReleaseEvent(QMouseEvent *me)
{
	QPoint pt = me->pos();
	if(QRect(4,1,7,7).contains(pt))
		emit previousClicked();
	else if(QRect(12,1,7,7).contains(pt))
		emit playClicked();
	else if(QRect(21,1,7,7).contains(pt))
		emit pauseClicked();
	else if(QRect(31,1,7,7).contains(pt))
		emit stopClicked();
	else if(QRect(40,1,7,7).contains(pt))
		emit nextClicked();
	else if(QRect(49,1,7,7).contains(pt))
		emit ejectClicked();
}

void PlaylistControl::updateSkin()
{
	setCursor(m_skin->getCursor(Skin::CUR_PNORMAL));
	update();
}
