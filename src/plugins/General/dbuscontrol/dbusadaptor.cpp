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

#include "dbuscontrol.h"
#include "dbusadaptor.h"

DBUSAdaptor::DBUSAdaptor(Control *ctrl, QObject *parent)
        : QDBusAbstractAdaptor(parent)
{
    m_control = ctrl;
    setAutoRelaySignals(TRUE);
    connect(parent, SIGNAL(stateChanged()), SLOT (processState()));
    connect(parent, SIGNAL(volumeChanged()), SLOT (processVolume()));
    connect(parent, SIGNAL(timeChanged()), SLOT (processTime()));
    connect(parent, SIGNAL(songChanged()), SIGNAL(songChanged()));
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
    if(left || right)
        return (right-left)*100/qMax(left, right);
    else
        return 0;
}

void DBUSAdaptor::setBalance(int bal)
{
    bal = qMin(bal,100);
    bal = qMax(bal,-100);
    int left = volume()-qMax(bal,0)*volume()/100;
    int right = volume()+qMin(bal,0)*volume()/100;
    QMetaObject::invokeMethod(m_control, "setVolume", Q_ARG(int, left), Q_ARG(int, right));
}

int DBUSAdaptor::length()
{
    return qobject_cast<DBUSControl *>(parent())->info()->length();
}

int DBUSAdaptor::year()
{
    return qobject_cast<DBUSControl *>(parent())->info()->year();
}

QString DBUSAdaptor::title()
{
    return qobject_cast<DBUSControl *>(parent())->info()->title();
}

QString DBUSAdaptor::artist()
{
    return qobject_cast<DBUSControl *>(parent())->info()->artist();
}

QString DBUSAdaptor::album()
{
    return qobject_cast<DBUSControl *>(parent())->info()->album();
}

QString DBUSAdaptor::comment()
{
    return qobject_cast<DBUSControl *>(parent())->info()->comment();
}

QString DBUSAdaptor::genre()
{
    return qobject_cast<DBUSControl *>(parent())->info()->genre();
}

QString DBUSAdaptor::path()
{
    return qobject_cast<DBUSControl *>(parent())->info()->path();
}

bool DBUSAdaptor::isPlaying()
{
    return qobject_cast<DBUSControl *>(parent())->state() == General::Playing;
}

bool DBUSAdaptor::isPaused()
{
     return qobject_cast<DBUSControl *>(parent())->state() == General::Paused;
}

bool DBUSAdaptor::isStopped()
{
     return qobject_cast<DBUSControl *>(parent())->state() == General::Stopped;
}

int DBUSAdaptor::elapsedTime()
{
    return qobject_cast<DBUSControl *>(parent())->elapsedTime();
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

void DBUSAdaptor::toggleVisibility()
{
    QMetaObject::invokeMethod(m_control, "toggleVisibility");
}

void DBUSAdaptor::exit()
{
    QMetaObject::invokeMethod(m_control, "exit");
}

void DBUSAdaptor::seek(int time)
{
    if ((time < 0) || (time > length()))
        return;
    QMetaObject::invokeMethod(m_control, "seek", Q_ARG(int, time));
}

void DBUSAdaptor::processState()
{
    uint state = qobject_cast<DBUSControl *>(parent())->state();
    if(state == General::Playing)
        emit started();
    else if(state == General::Stopped)
        emit stopped();
    else if(state == General::Paused)
        emit paused();
}

void DBUSAdaptor::processVolume()
{
    emit volumeChanged(volume(), balance());
}

void DBUSAdaptor::processTime()
{
    emit timeChanged(elapsedTime());
}
