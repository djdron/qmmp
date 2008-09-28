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

#include <QTimer>
#include "output.h"

#include "volumecontrol.h"

VolumeControl::VolumeControl(QObject *parent)
        : QObject(parent)
{
    m_left = 0;
    m_right = 0;
}


VolumeControl::~VolumeControl()
{
}

VolumeControl *VolumeControl::create(QObject *parent)
{
    VolumeControl *control = Output::currentFactory()->createVolumeControl(parent);
    QTimer *m_timer = new QTimer(control);
    connect(m_timer, SIGNAL(timeout()), control, SLOT(checkVolume()));
    m_timer->start(125);
    return control;
}

void VolumeControl::checkVolume()
{
    int l = 0, r = 0;
    volume(&l, &r);
    l = (l > 100) ? 100 : l;
    r = (r > 100) ? 100 : r;
    l = (l < 0) ? 0 : l;
    r = (r < 0) ? 0 : r;
    if (m_left != l || m_right != r)
    {
        m_left = l;
        m_right = r;
        emit volumeChanged(m_left, m_right);
    }
}
