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
    m_bits = 0;
    m_scale = 1.0;
}

void ReplayGain::setSampleSize(int bits)
{
    m_bits = bits;
    updateScale();
}

void ReplayGain::setReplayGainInfo(const ReplayGainInfo &info)
{
    m_info = info;
    updateScale();
}

void ReplayGain::setReplayGainSettings(const ReplayGainSettings &settings)
{
    m_settings = settings;
    updateScale();
}

void ReplayGain::applyReplayGain(char *data, qint64 size)
{
    if(m_settings.mode() == ReplayGainSettings::DISABLED || m_scale == 1.0)
        return;
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

void ReplayGain::updateScale()
{
    double peak = 0.0;
    m_scale = 0.0;
    switch((int) m_settings.mode())
    {
    case ReplayGainSettings::TRACK:
        m_scale = pow(10.0, m_info.trackGain()/20);
        peak = m_info.trackPeak();
        break;
    case ReplayGainSettings::ALBUM:
        m_scale = pow(10.0, m_info.albumGain()/20);
        peak = m_info.albumPeak();
        break;
    case ReplayGainSettings::DISABLED:
        m_scale = 1.0;
    }
    if(m_scale == 0.0)
        m_scale = pow(10.0, m_settings.defaultGain()/20);
    if(peak > 0.0 && m_scale != 1.0 && m_scale > 0.0)
    {
        m_scale *= pow(10.0, m_settings.preamp()/20);
        if(m_settings.preventClipping())
            m_scale = m_scale*peak > 1.0 ? 1.0 / peak : m_scale;
    }
    if(m_scale < 0.0)
        m_scale = 1.0;
    m_scale = qMin(m_scale, 15.0);
}
