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

#include "eqsettings.h"

EqSettings::EqSettings()
{
    for(int i = 0; i < 10; ++i)
        m_gain[i] = 0;
    m_preamp = 0;
}

bool EqSettings::isEnabled() const
{
    return m_is_enabled;
}

double EqSettings::gain(int chan) const
{
    return m_gain[chan];
}

double EqSettings::preamp() const
{
    return m_preamp;
}

void EqSettings::setEnabled(bool enabled)
{
    m_is_enabled = enabled;
}

void EqSettings::setGain(int chan, double gain)
{
    m_gain[chan] = gain;
}

void EqSettings::setPreamp(double preamp)
{
    m_preamp = preamp;
}

void EqSettings::operator=(const EqSettings &s)
{
    for(int i = 0; i < 10; ++i)
        m_gain[i] = s.m_gain[i];
    m_preamp = s.m_preamp;
    m_is_enabled = s.m_is_enabled;
}

bool EqSettings::operator==(const EqSettings &s) const
{
    for(int i = 0; i < 10; ++i)
    {
        if(m_gain[i] != s.m_gain[i])
            return false;
    }
    return (m_preamp == s.m_preamp) && (m_is_enabled == s.m_is_enabled);
}

bool EqSettings::operator!=(const EqSettings &s) const
{
    return !operator==(s);
}
