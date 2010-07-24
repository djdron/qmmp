/***************************************************************************
 *   Copyright (C) 2006-2010 by Ilya Kotov                                 *
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

#ifndef OUTPUTPULSEAUDIO_H
#define OUTPUTPULSEAUDIO_H

#include <QObject>
extern "C"{
#include <pulse/simple.h>
}
#include <qmmp/output.h>

/**
    @author Ilya Kotov <forkotov02@hotmail.ru>
*/
class OutputPulseAudio : public Output
{
    Q_OBJECT
public:
    OutputPulseAudio(QObject * parent = 0);
    ~OutputPulseAudio();

    bool initialize();
    void configure(quint32, int, Qmmp::AudioFormat format);
    qint64 latency();

private:
    //output api
    qint64 writeAudio(unsigned char *data, qint64 maxSize);
    void drain();
    void reset();

    // helper functions
    void uninitialize();

    pa_simple *m_connection;
};


#endif // OUTPUTPULSEAUDIO_H
