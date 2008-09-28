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
#include <QSettings>
#include <QDir>

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
    QSettings settings(QDir::homePath()+"/.qmmp/qmmprc", QSettings::IniFormat);
    if (settings.value("Volume/software_volume", FALSE).toBool())
        return new SoftwareVolume(parent);
    VolumeControl *control = Output::currentFactory()->createVolumeControl(parent);
    if (!control)
        return new SoftwareVolume(parent);
    QTimer *m_timer = new QTimer(control);
    connect(m_timer, SIGNAL(timeout()), control, SLOT(checkVolume()));
    m_timer->start(125);
    return control;
}

int VolumeControl::left()
{
    return m_left;
}

int VolumeControl::right()
{
    return m_right;
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

SoftwareVolume *SoftwareVolume::m_instance = 0;

SoftwareVolume::SoftwareVolume(QObject *parent)
        : VolumeControl(parent)
{
    QSettings settings(QDir::homePath()+"/.qmmp/qmmprc", QSettings::IniFormat);
    m_left = settings.value("Volume/left", 80).toInt();
    m_right = settings.value("Volume/right", 80).toInt();
    QTimer::singleShot(125, this, SLOT(checkVolume()));
    m_instance = this;
    //checkVolume();
}

SoftwareVolume::~SoftwareVolume()
{
    QSettings settings(QDir::homePath()+"/.qmmp/qmmprc", QSettings::IniFormat);
    settings.setValue("Volume/left", m_left);
    settings.setValue("Volume/right", m_right);
    m_instance = 0;
}

void SoftwareVolume::setVolume(int left, int right)
{
    m_left = left;
    m_right = right;
    checkVolume();
}

void SoftwareVolume::volume(int *left, int *right)
{
    *left = m_left;
    *right = m_right;
}
//static
SoftwareVolume *SoftwareVolume::instance()
{
    return m_instance;
}

void  SoftwareVolume::setEnabled(bool b)
{
     QSettings settings(QDir::homePath()+"/.qmmp/qmmprc", QSettings::IniFormat);
     settings.setValue("Volume/software_volume", b);
}
