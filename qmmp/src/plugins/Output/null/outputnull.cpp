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

#include "outputnull.h"


OutputNull::OutputNull(QObject * parent) : Output(parent)
{
   m_bytes_per_second = 0;
}

OutputNull::~OutputNull()
{}

void OutputNull::configure(quint32 freq, int chan, Qmmp::AudioFormat format)
{
    switch (format)
    {
    case Qmmp::PCM_S8:
        m_bytes_per_second = freq * chan;
        break;
    case Qmmp::PCM_S24LE:
    case Qmmp::PCM_S32LE:
         m_bytes_per_second = freq * chan * 4;
        break;
    case Qmmp::PCM_S16LE:
    default:
         m_bytes_per_second = freq * chan * 2;
    }
    Output::configure(freq, chan, format);
}

bool OutputNull::initialize()
{
    return true;
}


qint64 OutputNull::latency()
{
    return 0;
}

qint64 OutputNull::writeAudio(unsigned char *data, qint64 maxSize)
{
   Q_UNUSED(data);
   usleep(maxSize * 1000000 / m_bytes_per_second);
   return maxSize;
}

void OutputNull::drain()
{}

void OutputNull::reset()
{}
