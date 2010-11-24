/***************************************************************************
 *   Copyright (C) 2008-2010 by Ilya Kotov                                 *
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
#include "root2object.h"
#include "mpris.h"

MPRIS::MPRIS(QObject *parent) : General(parent)
{
    QDBusConnection connection = QDBusConnection::sessionBus();
    //MPRISv1
    connection.registerObject("/TrackList", new TrackListObject(this), QDBusConnection::ExportAllContents);
    connection.registerObject("/Player", new PlayerObject(this), QDBusConnection::ExportAllContents);
    connection.registerObject("/", new RootObject(this), QDBusConnection::ExportAllContents);
    //MPRISv2
    connection.registerObject("/org/mpris/MediaPlayer2", new Root2Object(this),
                              QDBusConnection::ExportAllContents);
    connection.registerService("org.mpris.qmmp");
    connection.registerService("org.mpris.MediaPlayer2.qmmp");
}

MPRIS::~MPRIS()
{
    QDBusConnection::sessionBus().unregisterService("org.mpris.qmmp");
    QDBusConnection::sessionBus().unregisterService("org.mpris.MediaPlayer2.qmmp");
}
