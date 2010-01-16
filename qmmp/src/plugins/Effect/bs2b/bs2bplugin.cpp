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

#include <QSettings>
#include <math.h>
#include <stdlib.h>
#include <qmmp/qmmp.h>
#include "bs2bplugin.h"

Bs2bPlugin *Bs2bPlugin::m_instance = 0;

Bs2bPlugin::Bs2bPlugin() : Effect()
{
    m_instance = this;
    m_bs2b_handler = bs2b_open();
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    bs2b_set_level(m_bs2b_handler, settings.value("bs2b/level", BS2B_DEFAULT_CLEVEL).toUInt());
    m_chan = 0;
}

Bs2bPlugin::~Bs2bPlugin()
{
    m_instance = 0;
    bs2b_clear(m_bs2b_handler);
}

#define CASE_BS2B(bitsPerSample, dataType, functionToCall, samples, out_data) \
    case bitsPerSample: \
        { \
            dataType * data = reinterpret_cast<dataType *>(out_data); \
            functionToCall(m_bs2b_handler, data, samples); \
        } \
        break;

void Bs2bPlugin::applyEffect(Buffer *b)
{
    if(m_chan != 2)
        return;
    uint samples = b->nbytes / audioParameters().sampleSize() / 2;
    m_mutex.lock();
    switch (format())
    {
        CASE_BS2B(Qmmp::PCM_S8,  int8_t,  bs2b_cross_feed_s8, samples, b->data)
        CASE_BS2B(Qmmp::PCM_S16LE, int16_t, bs2b_cross_feed_s16le, samples, b->data)
        //CASE_BS2B(Qmmp::PCM_S24LE, bs2b_int24_t,  bs2b_cross_feed_s24le, samples, out_data)
        CASE_BS2B(Qmmp::PCM_S32LE, int32_t,  bs2b_cross_feed_s32le, samples, b->data)
    default:
        ; // noop
    }
    m_mutex.unlock();
}

void Bs2bPlugin::configure(quint32 freq, int chan, Qmmp::AudioFormat format)
{
    m_chan = chan;
    Effect::configure(freq, chan, format);
    bs2b_set_srate(m_bs2b_handler,freq);
}

void Bs2bPlugin::setCrossfeedLevel(uint32_t level)
{
    m_mutex.lock();
    bs2b_set_level(m_bs2b_handler, level);
    m_mutex.unlock();
}

Bs2bPlugin* Bs2bPlugin::instance()
{
    return m_instance;
}
