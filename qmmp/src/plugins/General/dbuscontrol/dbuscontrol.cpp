/***************************************************************************
 *   Copyright (C) 2008 by Ilya Kotov                                      *
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

#include <qmmpui/control.h>

#include "dbusadaptor.h"
#include "dbuscontrol.h"

DBUSControl::DBUSControl(Control *control, QObject *parent)
        : General(parent)
{
    new DBUSAdaptor(control, this);
    QDBusConnection connection = QDBusConnection::sessionBus();
    connection.registerObject("/Qmmp", this);
    connection.registerService("org.qmmp.dbus");
    m_left = 0;
    m_right = 0;
    m_time = 0;
    m_state = General::Stopped;
}


DBUSControl::~DBUSControl()
{
}

void DBUSControl::setState(const uint &state)
{
    m_state = state;
    emit stateChanged();
    switch ((uint) state)
    {
    case General::Playing:
    {
        break;
    }
    case General::Paused:
    {
        break;
    }
    case General::Stopped:
    {
        m_song.clear();
        m_time = 0;
        break;
    }
    }
}

void DBUSControl::setSongInfo(const SongInfo &song)
{
    m_song = song;
}

void DBUSControl::setVolume(int left, int right)
{
    m_left = left;
    m_right = right;
    emit volumeChanged();
}

void DBUSControl::setTime(int time)
{
    m_time = time;
    emit timeChanged();
}

SongInfo *DBUSControl::info()
{
    return &m_song;
}

uint DBUSControl::state()
{
    return m_state;
}

int DBUSControl::elapsedTime()
{
    return m_time;
}

int DBUSControl::leftVolume()
{
    return m_left;
}

int DBUSControl::rightVolume()
{
    return m_right;
}

