/***************************************************************************
 *   Copyright (C) 2007-2008 by Zhuravlev Uriy                             *
 *   stalkerg@gmail.com                                                    *
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

#include <QObject>
#include <QApplication>
#include <QtGlobal>
#include <QDir>
#include <QSettings>
#include "outputjack.h"
#include <qmmp/audioparameters.h>
#include <qmmp/buffer.h>
#include <qmmp/visual.h>

#include <stdio.h>
#include <string.h>

#define JACK_TIMEOUT 500000L

OutputJACK::OutputJACK(QObject *parent) : Output(parent), m_inited(false)
{
    JACK_Init();
    m_wait_time = 0;
}

OutputJACK::~OutputJACK()
{
    uninitialize();
}

bool OutputJACK::initialize(quint32 freq, int chan, Qmmp::AudioFormat format)
{
    qDebug("OutputJACK: initialize");
    if(JACK_Open(&jack_device, AudioParameters::sampleSize(format)*8, (unsigned long *)&freq, chan))
    {
        m_inited = false;
        return false;
    }
    m_inited = true;
    configure(freq, chan, format);
    return true;
}

qint64 OutputJACK::latency()
{
    return 0;
}

qint64 OutputJACK::writeAudio(unsigned char *data, qint64 maxSize)
{
    if(!m_inited)
         return -1;
    m = JACK_Write(jack_device, (unsigned char*)data, maxSize);

    if (!m)
    {
        usleep(2000);
        if(JACK_GetState(jack_device) != PLAYING)
            m_wait_time += 2000;
        if(m_wait_time > JACK_TIMEOUT)
            return -1;
    }
    else
        m_wait_time = 0;
    return m;
}

void OutputJACK::reset()
{
    JACK_Reset(jack_device);
}

void OutputJACK::uninitialize()
{
    if (!m_inited)
        return;
    JACK_Close(jack_device);
}
