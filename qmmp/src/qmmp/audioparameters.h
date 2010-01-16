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

#ifndef AUDIOPARAMETERS_H
#define AUDIOPARAMETERS_H

#include <QtGlobal>
#include "qmmp.h"

/*!
 * @author Ilya Kotov <forkotov02@hotmail.ru>
 */
class AudioParameters
{
public:
    AudioParameters();
    AudioParameters(quint32 srate, int chan, Qmmp::AudioFormat format);
    AudioParameters(const AudioParameters &other);

    void operator=(const AudioParameters &p);
    bool operator==(const AudioParameters &p) const;
    bool operator!=(const AudioParameters &p) const;

    quint32 sampleRate() const;
    int channels() const;
    Qmmp::AudioFormat format() const;
    int sampleSize() const;
    static int sampleSize(Qmmp::AudioFormat format);

private:
    quint32 m_srate;
    int m_chan;
    Qmmp::AudioFormat m_format;
};

#endif // AUDIOPARAMETERS_H
