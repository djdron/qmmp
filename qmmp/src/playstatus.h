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
#ifndef PLAYSTATUS_H
#define PLAYSTATUS_H

#include <pixmapwidget.h>

/**
	@author Ilya Kotov <forkotov02@hotmail.ru>
*/
class Skin;

class PlayStatus : public PixmapWidget
{
Q_OBJECT
public:
    PlayStatus(QWidget *parent = 0);

    ~PlayStatus();
 
    enum Type 
    {
       PLAY,
       STOP,
       PAUSE,
    };

void setStatus(Type);

private slots:
    void updateSkin();

private:
   Skin *m_skin;
   Type m_status;
};

#endif
