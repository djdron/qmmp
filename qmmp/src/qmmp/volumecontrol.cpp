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

#include <QTimer>
#include <QDir>
#include <QSettings>
#include "qmmpsettings.h"
#include "output.h"
#include "volumecontrol.h"

VolumeControl::VolumeControl(QObject *parent)
        : QObject(parent)
{
    m_left = 0;
    m_right = 0;
    m_prev_block = FALSE;
}

VolumeControl::~VolumeControl()
{
}

VolumeControl *VolumeControl::create(QObject *parent)
{
    if(QmmpSettings::instance()->useSoftVolume())
        return new SoftwareVolume(parent);
    VolumeControl *control = 0;
    if (Output::currentFactory())
        control = Output::currentFactory()->createVolumeControl(parent);
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
    if (m_left != l || m_right != r) //volume has been changed
    {
        m_left = l;
        m_right = r;
        emit volumeChanged(m_left, m_right);
    }
    else if(m_prev_block && !signalsBlocked ()) //signals have been unblocked
        emit volumeChanged(m_left, m_right);
    m_prev_block = signalsBlocked ();
}

SoftwareVolume *SoftwareVolume::m_instance = 0;

SoftwareVolume::SoftwareVolume(QObject *parent)
        : VolumeControl(parent)
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    m_left = settings.value("Volume/left", 80).toInt();
    m_right = settings.value("Volume/right", 80).toInt();
    blockSignals(true);
    checkVolume();
    blockSignals(FALSE);
    QTimer::singleShot(125, this, SLOT(checkVolume()));
    m_scaleLeft = (double)m_left/100.0;
    m_scaleRight = (double)m_right/100.0;
    m_instance = this;
}

SoftwareVolume::~SoftwareVolume()
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.setValue("Volume/left", m_left);
    settings.setValue("Volume/right", m_right);
    m_instance = 0;
}

void SoftwareVolume::setVolume(int left, int right)
{
    m_left = left;
    m_right = right;
    checkVolume();
    m_scaleLeft = (double)m_left/100.0;
    m_scaleRight = (double)m_right/100.0;
}

void SoftwareVolume::volume(int *left, int *right)
{
    *left = m_left;
    *right = m_right;
}

void SoftwareVolume::changeVolume(Buffer *b, int chan, Qmmp::AudioFormat format)
{
    int samples = 0;
    switch(format)
    {
    case Qmmp::PCM_S8:
        samples = b->nbytes;
        if (chan > 1)
        {
            for (int i = 0; i < samples; i+=2)
            {
                ((char*)b->data)[i]*= m_scaleLeft;
                ((char*)b->data)[i+1]*= m_scaleRight;
            }
        }
        else
        {
            for (int i = 0; i < samples; i++)
                ((char*)b->data)[i]*= qMax(m_scaleRight, m_scaleLeft);
        }
    case Qmmp::PCM_S16LE:
        samples = b->nbytes/2;
        if (chan > 1)
        {
            for (int i = 0; i < samples; i+=2)
            {
                ((short*)b->data)[i]*= m_scaleLeft;
                ((short*)b->data)[i+1]*= m_scaleRight;
            }
        }
        else
        {
            for (int i = 0; i < samples; i++)
                ((short*)b->data)[i]*= qMax(m_scaleRight, m_scaleLeft);
        }
        break;
    case Qmmp::PCM_S24LE:
    case Qmmp::PCM_S32LE:
        samples = b->nbytes/4;
        if (chan > 1)
        {
            for (qint64 i = 0; i < samples; i+=2)
            {
                ((qint32*)b->data)[i]*= m_scaleLeft;
                ((qint32*)b->data)[i+1]*= m_scaleRight;
            }
        }
        else
        {
            for (qint64 i = 0; i < samples; i++)
                ((qint32*)b->data)[i]*= qMax(m_scaleRight, m_scaleLeft);
        }
        break;
    default:
        ;
    }
}

//static
SoftwareVolume *SoftwareVolume::instance()
{
    return m_instance;
}

