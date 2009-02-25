/***************************************************************************
 *   Copyright (C) 2009 by Ilya Kotov <forkotov02@hotmail.ru>              *
 *   Copyright (C) 2009 by Sebastian Pipping <sebastian@pipping.org>       *
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

#define CASE_BS2B(bitsPerSample, dataType, functionToCall, samples, out_data) \
    case bitsPerSample: \
        { \
            dataType * data = reinterpret_cast<dataType *>(*out_data); \
            while (samples--) { \
                functionToCall(data); \
                data += 2; \
            } \
        } \
        break;

ulong Bs2bPlugin::process(char *in_data, const ulong size, char **out_data)
{
    memcpy(*out_data, in_data, size);
    if(channels() != 2)
        return size;

    uint samples = size / (bitsPerSample() / 8) / 2;
    switch (bitsPerSample()) {
    CASE_BS2B(8,  char,  bs2b_cross_feed_s8, samples, out_data)
    CASE_BS2B(16, short, bs2b_cross_feed_16, samples, out_data)
    CASE_BS2B(32, long,  bs2b_cross_feed_32, samples, out_data)
    default:
        ; // noop
    }
    return size;
}

void Bs2bPlugin::configure(quint32 freq, int chan, int res)
{
    Effect::configure(freq, chan, res);
    bs2b_set_srate(freq);
    bs2b_set_level(BS2B_DEFAULT_CLEVEL);
}
