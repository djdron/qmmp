/***************************************************************************
 *   Copyright (C) 2009 by Ilya Kotov                                      *
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

#include <math.h>
#include "replaygain.h"

ReplayGain::ReplayGain()
{
    m_freq = 0;
    m_chan = 0;
    m_bits = 0;
    m_scale = 0;
}

void ReplayGain::setSampleSize(int bits)
{
    m_bits = bits;
}

void ReplayGain::setReplayGainInfo(const ReplayGainInfo &info)
{
    m_info = info;
    m_scale = pow(10.0, (info.trackGain()/20) * info.trackPeak());
}

void ReplayGain::applyReplayGain(char *data, qint64 size)
{
    size = size*8/m_bits;
    if(m_bits == 16)
    {
        for (qint64 i = 0; i < size; i++)
            ((short*)data)[i]*= m_scale;

    }
    else if(m_bits == 8)
    {
        for (qint64 i = 0; i < size; i++)
            ((char*)data)[i]*= m_scale;
    }
    else if(m_bits == 32)
    {
        for (qint64 i = 0; i < size; i++)
           ((qint32*)data)[i]*= m_scale;
    }
}
