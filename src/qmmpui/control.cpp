/***************************************************************************
 *   Copyright (C) 2008 by Ilya Kotov                                      *
 *   forkotov02@hotmail.ru                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Control Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Control Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Control Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "control.h"

Control::Control(QObject *parent)
 : QObject(parent)
{
}


Control::~Control()
{
}

void Control::play()
{
    emit commandCalled(Play);
}

void Control::pause()
{
    emit commandCalled(Pause);
}

void Control::stop()
{
    emit commandCalled(Stop);
}

void Control::next()
{
    emit commandCalled(Next);
}

void Control::previous()
{
    emit commandCalled(Previous);
}

void Control::seek(int pos)
{
    emit seekCalled(pos);
}

void Control::exit()
{
    emit commandCalled(Exit);
}

void Control::toggleVisibility()
{
    emit commandCalled(ToggleVisibility);
}

void Control::setVolume(int left, int right)
{
    emit volumeChanged(left, right);
}

