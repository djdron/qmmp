/***************************************************************************
 *   Copyright (C) 2010 by Ilya Kotov                                      *
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

#include <QDBusInterface>
#include <QDBusConnection>

#include "udisksdevice.h"


UDisksDevice::UDisksDevice(QDBusObjectPath o, QObject *parent)
        : QObject(parent)
{
    m_interface = new QDBusInterface("org.freedesktop.UDisks", o.path(),
                                     "org.freedesktop.UDisks.Device", QDBusConnection::systemBus(), this);
    m_interface->connection().connect("org.freedesktop.UDisks", o.path(),
                                      "org.freedesktop.UDisks.Device","Changed",
                                      this, SIGNAL(changed()));
    m_path = o;
}

QVariant UDisksDevice::property (const QString &key)
{
    return m_interface->property(key.toAscii().data());
}

UDisksDevice::~UDisksDevice()
{
}

QDBusObjectPath UDisksDevice::objectPath() const
{
    return m_path;
}
