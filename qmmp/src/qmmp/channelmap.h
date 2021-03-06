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

#ifndef CHANNELMAP_H
#define CHANNELMAP_H

#include <QList>
#include "qmmp.h"

class ChannelMap : public QList<Qmmp::ChannelPosition>
{
public:
    ChannelMap();
    ChannelMap(int channels);

    int mask() const;
    const ChannelMap remaped() const;
    const QString toString() const;

private:
    static Qmmp::ChannelPosition m_internal_map[9];
    void generateMap(int channels);
};

#endif // CHANNELMAP_H
