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

#include <QtDBus>

#include "playerobject.h"
#include "rootobject.h"
#include "tracklistobject.h"
#include "mpris.h"

MPRIS::MPRIS(QObject *parent)
        : General(parent)
{
    PlayerObject *player = new PlayerObject(this);
    RootObject *root = new RootObject(this);
    TrackListObject *trackList = new TrackListObject(this);
    QDBusConnection connection = QDBusConnection::sessionBus();
    connection.registerObject("/TrackList", trackList, QDBusConnection::ExportAllContents);
    connection.registerObject("/Player", player, QDBusConnection::ExportAllContents);
    connection.registerObject("/", root, QDBusConnection::ExportAllContents);
    connection.registerService("org.mpris.qmmp");
}


MPRIS::~MPRIS()
{
}


