/***************************************************************************
 *   Copyright (C) 2010-2011 by Ilya Kotov                                 *
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

#include <QtGlobal>
#include "eqsettings.h"

EqSettings::EqSettings(int bands)
{
    if(bands != 10 && bands != 15 && bands != 25 && bands != 31)
    {
        qWarning("EqSettings: invalid number of bands (%d), using 10 bands as fallback", bands);
        bands = 10;
    }
    for(int i = 0; i < bands; ++i)
        m_gains[i] = 0;
    m_bands = bands;
    m_preamp = 0;
    m_is_enabled = false;
}

bool EqSettings::isEnabled() const
{
    return m_is_enabled;
}

double EqSettings::gain(int chan) const
{
    return m_gains[chan];
}

double EqSettings::preamp() const
{
    return m_preamp;
}

int EqSettings::bands() const
{
    return m_bands;
}

void EqSettings::setEnabled(bool enabled)
{
    m_is_enabled = enabled;
}

void EqSettings::setGain(int band, double gain)
{
    m_gains[band] = gain;
}

void EqSettings::setPreamp(double preamp)
{
    m_preamp = preamp;
}

void EqSettings::operator=(const EqSettings &s)
{
    for(int i = 0; i < 10; ++i)
        m_gains[i] = s.m_gains[i];
    m_preamp = s.m_preamp;
    m_is_enabled = s.m_is_enabled;
    m_bands = s.m_bands;
}

bool EqSettings::operator==(const EqSettings &s) const
{
    for(int i = 0; i < 10; ++i)
    {
        if(m_gains[i] != s.m_gains[i])
            return false;
    }
    return (m_preamp == s.m_preamp) && (m_is_enabled == s.m_is_enabled) && (m_bands == s.m_bands);
}

bool EqSettings::operator!=(const EqSettings &s) const
{
    return !operator==(s);
}
