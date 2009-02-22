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

#include <math.h>
#include <stdlib.h>
#include <qmmp/qmmp.h>

#include "bs2b-2.1.0/bs2b.h"
#include "bs2bplugin.h"

Bs2bPlugin::Bs2bPlugin(QObject* parent) : Effect(parent)
{
}

Bs2bPlugin::~Bs2bPlugin()
{
    bs2b_clear();
}

ulong Bs2bPlugin::process(char *in_data, const ulong size, char **out_data)
{
    memcpy(*out_data, in_data, size);
    if(channels() != 2)
        return size;
    short *bs2b_data = (short *) *out_data;
    uint samples = size / sizeof(short) / 2;
    while (samples--)
    {
        bs2b_cross_feed_16(bs2b_data);
        bs2b_data +=2;
    }
    return size;
}

void Bs2bPlugin::configure(quint32 freq, int chan, int res)
{
    Effect::configure(freq, chan, res);
    bs2b_set_srate(freq);
    bs2b_set_level(BS2B_DEFAULT_CLEVEL);
}
