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

DBUSAdaptor::DBUSAdaptor(Control *ctrl, QObject *parent)
        : QDBusAbstractAdaptor(parent)
{
    m_control = ctrl;
    setAutoRelaySignals(TRUE);
}

DBUSAdaptor::~DBUSAdaptor()
{}

int DBUSAdaptor::volume()
{
    int left, right;
    QMetaObject::invokeMethod(parent(), "leftVolume", Q_RETURN_ARG(int, left));
    QMetaObject::invokeMethod(parent(), "rightVolume", Q_RETURN_ARG(int, right));
    return qMax(left, right);
}

void DBUSAdaptor::setVolume(int volume)
{
    volume = qMin(volume, 100);
    volume = qMax(volume, 0);
    int bal = balance();
    int left = volume-qMax(bal,0)*volume/100;
    int right = volume+qMin(bal,0)*volume/100;
    QMetaObject::invokeMethod(m_control, "setVolume", Q_ARG(int, left), Q_ARG(int, right));
}

int DBUSAdaptor::balance()
{
    int left, right;
    QMetaObject::invokeMethod(parent(), "leftVolume", Q_RETURN_ARG(int, left));
    QMetaObject::invokeMethod(parent(), "rightVolume", Q_RETURN_ARG(int, right));
    return (right-left)*100/qMax(left, right);
}

void DBUSAdaptor::setBalance(int bal)
{
    bal = qMin(bal,100);
    bal = qMax(bal,-100);
    int left = volume()-qMax(bal,0)*volume()/100;
    int right = volume()+qMin(bal,0)*volume()/100;
    QMetaObject::invokeMethod(m_control, "setVolume", Q_ARG(int, left), Q_ARG(int, right));
}

void DBUSAdaptor::play()
{
    QMetaObject::invokeMethod(m_control, "play");
}

void DBUSAdaptor::stop()
{
    QMetaObject::invokeMethod(m_control, "stop");
}

void DBUSAdaptor::next()
{
    QMetaObject::invokeMethod(m_control, "next");
}

void DBUSAdaptor::previous()
{
    QMetaObject::invokeMethod(m_control, "previous");
}

void DBUSAdaptor::pause()
{
    QMetaObject::invokeMethod(m_control, "pause");
}

void DBUSAdaptor::exit()
{
    QMetaObject::invokeMethod(m_control, "exit");
}

