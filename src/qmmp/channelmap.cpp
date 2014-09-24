/***************************************************************************
 *   Copyright (C) 2014 by Ilya Kotov                                      *
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
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#include "channelmap.h"

Qmmp::AudioChannel ChannelMap::m_internal_map[8] = { Qmmp::CHAN_FRONT_LEFT,
                                                     Qmmp::CHAN_FRONT_RIGHT,
                                                     Qmmp::CHAN_REAR_LEFT,
                                                     Qmmp::CHAN_REAR_RIGHT,
                                                     Qmmp::CHAN_CENTER,
                                                     Qmmp::CHAN_LFE,
                                                     Qmmp::CHAN_SIDE_LEFT,
                                                     Qmmp::CHAN_SIDE_RIGHT };

ChannelMap::ChannelMap()
{
}

int ChannelMap::mask() const
{
    int mask = 0;
    foreach (Qmmp::AudioChannel channel, *this)
    {
        mask |= channel;
    }
    return mask;
}

const ChannelMap ChannelMap::remaped() const
{
    ChannelMap map;
    for(int i = 0; i < 8; ++i)
    {
        foreach (Qmmp::AudioChannel channel, *this)
        {
            if(channel == m_internal_map[i])
            {
                map.append(channel);
                break;
            }
        }
    }
    return map;
}
