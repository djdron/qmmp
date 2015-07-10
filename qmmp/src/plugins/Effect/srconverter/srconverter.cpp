/***************************************************************************
 *   Copyright (C) 2007-2015 by Ilya Kotov                                 *
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

#include <QSettings>
#include <math.h>
#include <stdlib.h>
#include <qmmp/qmmp.h>
#include "srconverter.h"

SRConverter::SRConverter() : Effect()
{
    int converter_type_array[] = {SRC_SINC_BEST_QUALITY, SRC_SINC_MEDIUM_QUALITY, SRC_SINC_FASTEST,
                                  SRC_ZERO_ORDER_HOLD,  SRC_LINEAR};
    m_src_state = 0;
    m_srcError = 0;
    m_sz = 0;
    m_src_data.data_in = 0;
    m_src_data.data_out = 0;
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    m_overSamplingFs = settings.value("SRC/sample_rate",48000).toInt();
    m_converter_type = converter_type_array[settings.value("SRC/engine", 0).toInt()];
}

SRConverter::~SRConverter()
{
    freeSRC();
    m_sz = 0;
}

void SRConverter::applyEffect(Buffer *b)
{
    if (m_src_state && b->nbytes > 0)
    {
        m_src_data.end_of_input = 0;
        m_src_data.input_frames = b->nbytes / m_sz / channels();
        m_src_data.output_frames = m_src_data.src_ratio * m_src_data.input_frames + 1;

        if(format() == Qmmp::PCM_S16LE)
        {
            src_short_to_float_array((short*) b->data, m_src_data.data_in,
                                     m_src_data.input_frames * channels());
        }
        else
        {
            qint32 *data = (int *) b->data;
            if(format() == Qmmp::PCM_S24LE)
            {
                uint samples =  b->size >> 2;
                for(unsigned int i = 0; i < samples; ++i)
                {
                    if(data[i] & 0x800000)
                        data[i] |= 0xff000000;
                }
            }
            src_int_to_float_array((int*) b->data, m_src_data.data_in,
                                   m_src_data.input_frames * channels());
        }

        if ((m_srcError = src_process(m_src_state, &m_src_data)) > 0)
        {
            qWarning("SRConverter: src_process(): %s\n", src_strerror(m_srcError));
        }
        else
        {
            b->nbytes = m_src_data.output_frames_gen * channels() * m_sz; //bytes

            //increase buffer size
            if(b->nbytes > b->size)
            {
                delete [] b->data;
                b->data = new unsigned char[b->nbytes];
                b->size = b->nbytes;
            }


            if(format() == Qmmp::PCM_S16LE)
            {
                src_float_to_short_array(m_src_data.data_out, (short*)b->data,
                                         m_src_data.output_frames_gen * channels());

            }
            else
            {
                src_float_to_int_array(m_src_data.data_out, (int*)b->data,
                                       m_src_data.output_frames_gen * channels());
            }
        }
    }
}

void SRConverter::configure(quint32 freq, ChannelMap map,  Qmmp::AudioFormat format)
{
    freeSRC();
    if(freq != m_overSamplingFs && format != Qmmp::PCM_S8)
    {
        m_src_state = src_new(m_converter_type, map.count(), &m_srcError);
        if (m_src_state)
        {
            m_src_data.src_ratio = (float)m_overSamplingFs/(float)freq;
            src_set_ratio(m_src_state, m_src_data.src_ratio);
        }
        else
            qDebug("SRConverter: src_new(): %s", src_strerror(m_srcError));

        m_sz = audioParameters().sampleSize();
        m_src_data.data_in = new float[QMMP_BLOCK_FRAMES * map.count() * 2];
        m_src_data.data_out = new float[int(m_src_data.src_ratio * QMMP_BLOCK_FRAMES * map.count() * 2 + 2)];
    }
    Effect::configure(m_overSamplingFs, map, format);
}

void SRConverter::freeSRC()
{
    if (m_src_state)
    {
        src_reset(m_src_state);
        src_delete(m_src_state);
    }
    m_src_state = 0;

    if(m_src_data.data_in)
    {
        delete [] m_src_data.data_in;
        m_src_data.data_in = 0;
    }
    if(m_src_data.data_out)
    {
        delete [] m_src_data.data_out;
        m_src_data.data_out = 0;
    }

    m_src_data.end_of_input = 0;
    m_src_data.input_frames = 0;
    m_src_data.output_frames = 0;
}
