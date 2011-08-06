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

#ifndef OUTPUTWAVEOUT_H
#define OUTPUTWAVEOUT_H

#include <QObject>
#include <stdio.h>
#include <windows.h>
#include <qmmp/output.h>

/**
    @author Ilya Kotov <forkotov02@hotmail.ru>
*/
class OutputWaveOut : public Output
{
    Q_OBJECT
public:
    OutputWaveOut(QObject * parent = 0);
    ~OutputWaveOut();

    bool initialize(quint32, int,  Qmmp::AudioFormat format);
    qint64 latency();

private:
    //output api
    qint64 writeAudio(unsigned char *data, qint64 size);
    void drain(){};
    void suspend(){};
    void resume(){};
    void reset(){}; 

    // helper functions
    void status();
    void uninitialize();
};


#endif // OUTPUTWAVEOUT_H
