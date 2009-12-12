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

#ifndef REPLAYGAININFO_H
#define REPLAYGAININFO_H

/**
    @author Ilya Kotov <forkotov02@hotmail.ru>
*/
class ReplayGainInfo
{
public:
    ReplayGainInfo();
    ReplayGainInfo(const ReplayGainInfo &other);
    void operator=(const ReplayGainInfo &ri);
    bool operator==(const ReplayGainInfo &ri) const;
    bool operator!=(const ReplayGainInfo &ri) const;
    double trackGain() const;
    double trackPeak() const;
    double albumGain() const;
    double albumPeak() const;
    void setTrackGain(double r);
    void setTrackPeak(double r);
    void setAlbumGain(double r);
    void setAlbumPeak(double r);

private:
    double m_trackGain;
    double m_trackPeak;
    double m_albumGain;
    double m_albumPeak;
};

#endif // REPLAYGAININFO_H
