/***************************************************************************
 *   Copyright (C) 2007-2010 by Ilya Kotov                                 *
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
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    m_overSamplingFs = settings.value("SRC/sample_rate",48000).toInt();
    m_converter_type = converter_type_array[settings.value("SRC/engine", 0).toInt()];
}

SRConverter::~SRConverter()
{
    src_reset (m_src_state) ;
    freeSRC();
    m_src_data.data_in = 0;
    m_src_data.data_out = 0;
    m_src_data.end_of_input = 0;
    m_src_data.input_frames = 0;
    m_src_data.output_frames = 0;
}

void SRConverter::applyEffect(Buffer *b)
{
    if (m_src_state && b->nbytes > 0)
    {
        m_src_data.input_frames = b->nbytes / 2 / channels();
        m_src_data.data_in = new float [m_src_data.input_frames * channels()];
        m_src_data.output_frames = m_src_data.src_ratio * m_src_data.input_frames + 1;
        m_src_data.end_of_input = 0;
        m_src_data.data_out = new float [m_src_data.output_frames * channels()];
        src_short_to_float_array((short*) b->data, m_src_data.data_in,
                                 m_src_data.input_frames * channels());
        if ((m_srcError = src_process(m_src_state, &m_src_data)) > 0)
        {
            qWarning("SRConverter: src_process(): %s\n", src_strerror(m_srcError));
        }
        else
        {
            qint16 *out_data = new qint16 [m_src_data.output_frames_gen * channels()];
            src_float_to_short_array(m_src_data.data_out, out_data,
                                     m_src_data.output_frames_gen*channels());
            b->nbytes = m_src_data.output_frames_gen * channels() * 2;
            if(b->nbytes > b->size)
            {
                delete [] b->data;
                b->data = (uchar *) out_data;
            }
            else
            {
                memcpy(b->data, out_data, b->nbytes);
                delete [] out_data;
            }

        }
        delete [] m_src_data.data_in;
        delete [] m_src_data.data_out;
    }
}

void SRConverter::configure(quint32 freq, int chan,  Qmmp::AudioFormat format)
{
    freeSRC();
    if(freq != m_overSamplingFs)
    {
        m_src_state = src_new(m_converter_type, chan, &m_srcError);
        if (m_src_state)
        {
            m_src_data.src_ratio = (float)m_overSamplingFs/(float)freq;
            src_set_ratio(m_src_state, m_src_data.src_ratio);
        }
        else
            qDebug("SRConverter: src_new(): %s", src_strerror(m_srcError));
    }
    Effect::configure(m_overSamplingFs, chan, format);
}

void SRConverter::freeSRC()
{
    if (m_src_state)
        m_src_state = src_delete(m_src_state);
    m_src_state = 0;
}
