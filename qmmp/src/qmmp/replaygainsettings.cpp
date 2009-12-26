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

#include "replaygainsettings.h"

ReplayGainSettings::ReplayGainSettings()
{
    m_mode = DISABLED;
    m_preamp = 0.0;
    m_defaultGain = -8.0;
    m_preventClipping = false;
}

ReplayGainSettings::ReplayGainSettings(int mode, double preamp, double defaultGain, bool preventClipping)
{
    m_mode = mode;
    m_preamp = preamp;
    m_defaultGain = defaultGain;
    m_preventClipping = preventClipping;
}

ReplayGainSettings::ReplayGainSettings(const ReplayGainSettings &other)
{
    operator=(other);
}

void ReplayGainSettings::operator=(const ReplayGainSettings &settings)
{
    m_mode = settings.mode();
    m_preamp = settings.preamp();
    m_defaultGain = settings.defaultGain();
    m_preventClipping = settings.preventClipping();
}

bool ReplayGainSettings::operator==(const ReplayGainSettings &settings) const
{
    return m_mode == settings.mode() &&
           m_preamp == settings.preamp() &&
           m_defaultGain == settings.defaultGain() &&
           m_preventClipping == settings.preventClipping();
}

bool ReplayGainSettings::operator!=(const ReplayGainSettings &settings) const
{
    return !operator==(settings);
}

int ReplayGainSettings::mode() const
{
    return m_mode;
}

double ReplayGainSettings::preamp() const
{
    return m_preamp;
}

double ReplayGainSettings::defaultGain() const
{
    return m_defaultGain;
}

bool ReplayGainSettings::preventClipping() const
{
    return m_preventClipping;
}
