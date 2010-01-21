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

#ifndef AUDIOSETTINGS_H
#define AUDIOSETTINGS_H

#include <QVariant>
#include <QMap>

/*!
 * @author Ilya Kotov <forkotov02@hotmail.ru>
 */
class AudioSettings
{
public:
    AudioSettings();
    AudioSettings(const AudioSettings &settings);

    enum ReplayGainMode
    {
        REPLAYGAIN_TRACK = 0,
        REPLAYGAIN_ALBUM,
        REPLAYGAIN_DISABLED
    };
    enum Key
    {
        REPLAYGAIN_MODE = 0,
        REPLAYGAIN_PREAMP,
        REPLAYGAIN_DEFAULT_GAIN,
        REPLAYGAIN_PREVENT_CLIPPING,
        SOFTWARE_VOLUME,
        OUTPUT_16BIT,
    };
    void operator=(const AudioSettings &settings);
    bool operator==(const AudioSettings &settings) const;
    bool operator!=(const AudioSettings &settings) const;
    void setValue(Key key, QVariant value);
    QVariant value(Key key) const;

private:
    QMap <Key, QVariant> m_settings;
};

#endif // AUDIOSETTINGS_H
