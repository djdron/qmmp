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

#include "audiosettings.h"

AudioSettings::AudioSettings()
{
    m_settings[REPLAYGAIN_MODE] = REPLAYGAIN_DISABLED;
    m_settings[REPLAYGAIN_PREAMP] = 0.0;
    m_settings[REPLAYGAIN_DEFAULT_GAIN] = 0.0;
    m_settings[REPLAYGAIN_PREVENT_CLIPPING] = FALSE;
    m_settings[SOFTWARE_VOLUME] = FALSE;
    m_settings[OUTPUT_16BIT] = FALSE;
}

AudioSettings::AudioSettings(const AudioSettings &settings)
{
    m_settings = settings.m_settings;
}

void AudioSettings::operator=(const AudioSettings &settings)
{
    m_settings = settings.m_settings;
}

bool AudioSettings::operator==(const AudioSettings &settings) const
{
    return m_settings == settings.m_settings;
}

bool AudioSettings::operator!=(const AudioSettings &settings) const
{
    return !operator==(settings);
}

void AudioSettings::setValue(Key key, QVariant value)
{
    m_settings[key] = value;
}

QVariant AudioSettings::value(Key key) const
{
    return m_settings[key];
}

