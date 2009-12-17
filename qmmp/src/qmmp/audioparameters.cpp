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

#include "audioparameters.h"

AudioParameters::AudioParameters()
{
    m_srate = 0;
    m_chan = 0;
    m_bps = 0;
}

AudioParameters::AudioParameters(const AudioParameters &other)
{
    m_srate = other.sampleRate();
    m_chan = other.channels();
    m_bps = other.bits();
}

AudioParameters::AudioParameters(quint32 srate, int chan, int bps)
{
    m_srate = srate;
    m_chan = chan;
    m_bps = bps;
}

void AudioParameters::operator=(const AudioParameters &p)
{
    m_srate = p.sampleRate();
    m_chan = p.channels();
    m_bps = p.bits();
}

bool AudioParameters::operator==(const AudioParameters &p) const
{
    return m_srate == p.sampleRate() && m_chan == p.channels() && m_bps == p.bits();
}

bool AudioParameters::operator!=(const AudioParameters &p) const
{
    return !operator==(p);
}

quint32 AudioParameters::sampleRate() const
{
    return m_srate;
}

int AudioParameters::channels() const
{
    return m_chan;
}

int AudioParameters::bits() const
{
    return m_bps;
}
