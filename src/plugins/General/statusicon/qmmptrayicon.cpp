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

#include <QEvent>
#include <QWheelEvent>

#include <qmmp/soundcore.h>

#include "qmmptrayicon.h"

QmmpTrayIcon::QmmpTrayIcon(QObject *parent)
        : QSystemTrayIcon(parent)
{
}


QmmpTrayIcon::~QmmpTrayIcon()
{
}

bool QmmpTrayIcon::event(QEvent *e)
{
    if (e->type() == QEvent::Wheel )
    {
        wheelEvent((QWheelEvent *) e);
        e->accept();
        return TRUE;
    }
    return QSystemTrayIcon::event(e);
}

void QmmpTrayIcon::wheelEvent(QWheelEvent *e)
{
    SoundCore *core = SoundCore::instance();
    int volume = qMax(core->leftVolume(), core->rightVolume());
    int balance = (core->rightVolume() - core->leftVolume()) * 100 / volume;
    volume += e->delta()/20;
    volume = qMax(volume,0);
    volume = qMin(volume,100);
    core->setVolume(volume - qMax(balance,0)*volume/100,
                    volume + qMin(balance,0)*volume/100);
}
