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
    m_isSrcAlloc = FALSE;
    int converter_type_array[] = {SRC_SINC_BEST_QUALITY, SRC_SINC_MEDIUM_QUALITY, SRC_SINC_FASTEST,
                                  SRC_ZERO_ORDER_HOLD,  SRC_LINEAR};
    m_srcIn = 0;
    m_srcOut = 0;
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
    if (m_isSrcAlloc)
    {
        delete [] m_srcIn;
        delete [] m_srcOut;
        delete [] m_wOut;
        m_isSrcAlloc = FALSE;
    }
}

void SRConverter::applyEffect(Buffer *b)
{
    if (m_isSrcAlloc)
    {
        delete [] m_srcIn;
        delete [] m_srcOut;
        delete [] m_wOut;
        m_isSrcAlloc = FALSE;
    }
    ulong wbytes = 0;

    if (m_src_state && b->nbytes > 0)
    {
        int lrLength = b->nbytes/2;
        int overLrLength= (int)floor(lrLength*(m_src_data.src_ratio+1));
        m_srcIn = new float [lrLength];
        m_srcOut = new float [overLrLength];
        m_wOut = new short[overLrLength];
        src_short_to_float_array((short *)b->data, m_srcIn, lrLength);
        m_isSrcAlloc = true;
        m_src_data.data_in = m_srcIn;
        m_src_data.data_out = m_srcOut;
        m_src_data.end_of_input = 0;
        m_src_data.input_frames = lrLength/2;
        m_src_data.output_frames = overLrLength/2;
        if ((m_srcError = src_process(m_src_state, &m_src_data)) > 0)
        {
            qWarning("SRConverter: src_process(): %s\n", src_strerror(m_srcError));
        }
        else
        {
            src_float_to_short_array(m_srcOut, m_wOut, m_src_data.output_frames_gen*2);
            wbytes = m_src_data.output_frames_gen*4;
            delete [] b->data;
            b->data = (unsigned char *) m_wOut;
            b->nbytes = wbytes;
            m_wOut = 0;
        }
    }
}

void SRConverter::configure(quint32 freq, int chan,  Qmmp::AudioFormat format)
{
    freeSRC();
    uint rate = freq;
    {
        m_src_state = src_new(m_converter_type, 2, &m_srcError);
        if (m_src_state)
        {
            m_src_data.src_ratio = (float)m_overSamplingFs/(float)rate;
            rate = m_overSamplingFs;
        }
        else
            qDebug("SRConverter: src_new(): %s", src_strerror(m_srcError));
    }
    Effect::configure(m_overSamplingFs, chan, format);
}

void SRConverter::freeSRC()
{
    if (m_src_state != NULL)
        m_src_state = src_delete(m_src_state);
}
