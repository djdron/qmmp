/***************************************************************************
 *   Copyright (C) 2012 by Ilya Kotov                                      *
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

#ifndef VOLUME_H
#define VOLUME_H

#endif // VOLUME_H


/*! @brief The Volume class is a provides volume API
 * @author Ilya Kotov <forkotov02@hotmail.ru>
 */
class Volume
{
public:
    virtual ~Volume(){}
    /*!
     * Volume control channel enum
     */
    enum Channel
    {
        LEFT_CHANNEL = 0, /*!< Right channel */
        RIGHT_CHANNEL     /*!< Left channel */
    };
    /*!
     * Setups volume level.
     * Subclass should reimplement this fucntion.
     * @param channel Channel
     * @param value Volume level. It should be \b 0..100
     */
    virtual void setVolume(int channel, int value) = 0;
    /*!
     * Returns volume level of the \b channel.
     */
    virtual int volume(int channel) = 0;
};
