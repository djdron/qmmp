/***************************************************************************
 *   Copyright (C) 2010-2014 by Ilya Kotov                                 *
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

#include "audioconverter_p.h"

//static functions
static inline void s8_to_s16(qint8 *in, qint16 *out, qint64 samples)
{
    for(qint64 i = 0; i < samples; ++i)
        out[i] = in[i] << 8;
    return;
}

static inline void s24_to_s16(qint32 *in, qint16 *out, qint64 samples)
{
    for(qint64 i = 0; i < samples; ++i)
        out[i] = in[i] >> 8;
    return;
}

static inline void s32_to_s16(qint32 *in, qint16 *out, qint64 samples)
{
    for(qint64 i = 0; i < samples; ++i)
        out[i] = in[i] >> 16;
    return;
}

#define INT_TO_FLOAT(TYPE,in,out,samples,offset,max) \
{ \
    TYPE *in_copy = (TYPE *) (in); \
    float *out_copy = out; \
    for(size_t i = 0; i < samples; ++i) \
        out_copy[i] = (float) ((*in_copy++) - offset) / max; \
}

AudioConverter::AudioConverter()
{
    m_format = Qmmp::PCM_UNKNOWM;
    m_channels = 0;
    m_swap = false;
}

void AudioConverter::configure(quint32 srate, ChannelMap map, Qmmp::AudioFormat f)
{
    m_format = f;
    Effect::configure(srate, map, Qmmp::PCM_S16LE);
}

void AudioConverter::applyEffect(Buffer *b)
{
    switch(m_format)
    {
    case Qmmp::PCM_S8:
    {
        unsigned char *out = new unsigned char[b->nbytes*2];
        s8_to_s16((qint8 *)b->data, (qint16 *) out, b->nbytes);
        delete [] b->data;
        b->data = out;
        b->nbytes <<= 1;
        break;
    }
    case Qmmp::PCM_S24LE:
        s24_to_s16((qint32 *)b->data, (qint16 *)b->data, b->nbytes >> 2);
        b->nbytes >>= 1;
        break;
    case Qmmp::PCM_S32LE:
        s32_to_s16((qint32 *)b->data, (qint16 *)b->data, b->nbytes >> 2);
        b->nbytes >>= 1;
        break;
    default:
        ;
    }
}

void AudioConverter::configure(quint8 chan, Qmmp::AudioFormat f)
{
    m_format = f;
    m_channels = chan;

    switch (f)
    {
    case Qmmp::PCM_UNKNOWM:
    case Qmmp::PCM_S8:
    case Qmmp::PCM_U8:
    case Qmmp::PCM_FLOAT:
        m_swap = false;
        break;
    case Qmmp::PCM_S16LE:
    case Qmmp::PCM_U16LE:
    case Qmmp::PCM_S24LE:
    case Qmmp::PCM_U24LE:
    case Qmmp::PCM_S32LE:
    case Qmmp::PCM_U32LE:
#if Q_BYTE_ORDER == Q_BIG_ENDIAN
        m_swap = true;
#else
        m_swap = false;
#endif
        break;
    case Qmmp::PCM_S16BE:
    case Qmmp::PCM_U16BE:
    case Qmmp::PCM_S24BE:
    case Qmmp::PCM_U24BE:
    case Qmmp::PCM_S32BE:
    case Qmmp::PCM_U32BE:
#if Q_BYTE_ORDER == Q_BIG_ENDIAN
        m_swap = false;
#else
        m_swap = true;
#endif
        break;

    }
}

void AudioConverter::toFloat(const unsigned char *in, float *out, size_t samples)
{
    switch (m_format)
    {
    case Qmmp::PCM_S8:
        INT_TO_FLOAT(qint8, in, out, samples, 0, 0x80);
        break;
    case Qmmp::PCM_U8:
        INT_TO_FLOAT(quint8, in, out, samples, 0x80, 0x80);
        break;
    case Qmmp::PCM_S16LE:
    case Qmmp::PCM_S16BE:
        INT_TO_FLOAT(qint16, in, out, samples, 0, 0x8000);
        break;
    case Qmmp::PCM_U16LE:
    case Qmmp::PCM_U16BE:
        INT_TO_FLOAT(quint16, in, out, samples, 0x8000, 0x8000);
        break;
    case Qmmp::PCM_S24LE:
    case Qmmp::PCM_S24BE:
        INT_TO_FLOAT(qint32, in, out, samples, 0, 0x800000);
        break;
    case Qmmp::PCM_U24LE:
    case Qmmp::PCM_U24BE:
        INT_TO_FLOAT(quint32, in, out, samples, 0x800000, 0x800000);
        break;
    case Qmmp::PCM_S32LE:
    case Qmmp::PCM_S32BE:
        INT_TO_FLOAT(qint32, in, out, samples, 0, 0x80000000);
        break;
    case Qmmp::PCM_U32LE:
    case Qmmp::PCM_U32BE:
        INT_TO_FLOAT(quint32, in, out, samples, 0x80000000, 0x80000000);
        break;
    case Qmmp::PCM_FLOAT:
    case Qmmp::PCM_UNKNOWM:
        ;
    }
}

void AudioConverter::fromFloat(const float *in, const unsigned *out, size_t samples)
{

}
