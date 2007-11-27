/***************************************************************************
 *   Copyright (C) 2007 by Ilya Kotov                                      *
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
#ifndef SRCONVERTER_H
#define SRCONVERTER_H

#include <effect.h>

extern "C"
{
#include <samplerate.h>
}

/**
    @author Ilya Kotov <forkotov02@hotmail.ru>
*/

class SRConverter : public Effect
{
    Q_OBJECT
public:
    SRConverter(QObject *parent = 0);

    virtual ~SRConverter();

    //bool process(char *in_data, char *out_data, const ulong maxsize, ulong &rbytes, ulong &wbytes);
    const ulong process(char *in_data, const ulong size, char **out_data);
    void configure(ulong freq, int chan, int res);
    const ulong frequency();

private:
    void freeSRC();
    SRC_STATE *m_src_state;
    SRC_DATA m_src_data;
    int m_overSamplingFs;
    int m_srcError;
    int m_converter_type;
    bool m_isSrcAlloc;
    float *m_srcIn, *m_srcOut;
    short *m_wOut;
    ulong m_freq;
};

#endif
