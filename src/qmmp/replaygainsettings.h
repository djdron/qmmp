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

#ifndef REPLAYGAINSETTINGS_H
#define REPLAYGAINSETTINGS_H

/**
    @author Ilya Kotov <forkotov02@hotmail.ru>
*/
class ReplayGainSettings
{
public:
    enum Mode
    {
        TRACK = 0,
        ALBUM,
        DISABLED
    };
    ReplayGainSettings();
    ReplayGainSettings(int mode, double preamp, double defaultGain, bool preventClipping);
    ReplayGainSettings(const ReplayGainSettings &other);
    void operator=(const ReplayGainSettings &settings);
    bool operator==(const ReplayGainSettings &settings) const;
    bool operator!=(const ReplayGainSettings &settings) const;

    int mode() const;
    double preamp() const;
    double defaultGain() const;
    bool preventClipping() const;

private:
    int m_mode;
    double m_preamp;
    double m_defaultGain;
    bool m_preventClipping;
};

#endif // REPLAYGAINSETTINGS_H
