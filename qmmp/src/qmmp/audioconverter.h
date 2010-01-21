/***************************************************************************
 *   Copyright (C) 2010 by Ilya Kotov                                      *
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

#ifndef AUDIOCONVERTER_H
#define AUDIOCONVERTER_H

#include "effect.h"

/*!
 * @author Ilya Kotov <forkotov02@hotmail.ru>
 */
class AudioConverter : public Effect
{
public:
    AudioConverter();
    void configure(quint32 srate = 44100, int chan = 2, Qmmp::AudioFormat f = Qmmp::PCM_S16LE);
    void applyEffect(Buffer *b);

private:
    Qmmp::AudioFormat m_format;

};

#endif // AUDIOCONVERTER_H
