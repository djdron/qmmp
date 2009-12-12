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

#include "replaygaininfo.h"

ReplayGainInfo::ReplayGainInfo()
{
    m_trackGain = 0;
    m_trackPeak = 0;
    m_albumGain = 0;
    m_albumPeak = 0;
}

ReplayGainInfo::ReplayGainInfo(const ReplayGainInfo &other)
{
    m_trackGain = other.trackGain();
    m_trackPeak = other.trackPeak();
    m_albumGain = other.albumGain();
    m_albumPeak = other.albumPeak();
}

void ReplayGainInfo::operator=(const ReplayGainInfo &ri)
{
    m_trackGain = ri.trackGain();
    m_trackPeak = ri.trackPeak();
    m_albumGain = ri.albumGain();
    m_albumPeak = ri.albumPeak();
}

bool ReplayGainInfo::operator==(const ReplayGainInfo &ri) const
{
    return m_trackGain == ri.trackGain() &&
           m_trackPeak == ri.trackPeak() &&
           m_albumGain == ri.albumGain() &&
           m_albumPeak == ri.albumPeak();

}

bool ReplayGainInfo::operator!=(const ReplayGainInfo &ri) const
{
    return !operator==(ri);
}

double ReplayGainInfo::trackGain() const
{
    return m_trackGain;
}

double ReplayGainInfo::trackPeak() const
{
    return m_trackPeak;
}

double ReplayGainInfo::albumGain() const
{
    return m_albumGain;
}

double ReplayGainInfo::albumPeak() const
{
     return m_albumPeak;
}

void ReplayGainInfo::setTrackGain(double r)
{
    m_trackGain = r;
}

void ReplayGainInfo::setTrackPeak(double r)
{
    m_trackPeak = r;
}

void ReplayGainInfo::setAlbumGain(double r)
{
    m_albumGain = r;
}

void ReplayGainInfo::setAlbumPeak(double r)
{
    m_albumPeak = r;
}
