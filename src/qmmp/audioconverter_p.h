/***************************************************************************
 *   Copyright (C) 2010-2015 by Ilya Kotov                                 *
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

#ifndef AUDIOCONVERTER_P_H
#define AUDIOCONVERTER_P_H

#include <stddef.h>
#include "qmmp.h"

/*! @internal
 * @author Ilya Kotov <forkotov02@hotmail.ru>
 */
class AudioConverter
{
public:
    AudioConverter();

    void configure(Qmmp::AudioFormat f);
    void toFloat(const unsigned char *in, float *out, size_t samples);
    void fromFloat(const float *in, const unsigned char *out, size_t samples);


private:
    Qmmp::AudioFormat m_format;
    bool m_swap;

};

#endif // AUDIOCONVERTER_P_H
