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
#include "dbusadaptor.h"

DBUSAdaptor::DBUSAdaptor(QObject *parent)
 : QDBusAbstractAdaptor(parent)
{
    setAutoRelaySignals(TRUE);
}

DBUSAdaptor::~DBUSAdaptor()
{
}

void DBUSAdaptor::play()
{
    QMetaObject::invokeMethod(parent(), "play");
}

void DBUSAdaptor::stop()
{
    QMetaObject::invokeMethod(parent(), "stop");
}

void DBUSAdaptor::next()
{
    QMetaObject::invokeMethod(parent(), "next");
}

void DBUSAdaptor::previous()
{
    QMetaObject::invokeMethod(parent(), "previous");
}

void DBUSAdaptor::pause()
{
    QMetaObject::invokeMethod(parent(), "pause");
}

void DBUSAdaptor::exit()
{
    QMetaObject::invokeMethod(parent(), "exit");
}

void DBUSAdaptor::setVolume(int left, int right)
{
    QMetaObject::invokeMethod(parent(), "setVolume", Q_ARG(int, left), Q_ARG(int, right));
}
