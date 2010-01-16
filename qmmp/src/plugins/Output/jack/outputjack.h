/***************************************************************************
 *   Copyright (C) 2007-2008 by Zhuravlev Uriy                             *
 *   stalkerg@gmail.com                                                    *
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

#ifndef   OUTPUTJACK_H
#define   OUTPUTJACK_H

class OutputJACK;

#include <qmmp/output.h>
#include <QObject>
extern "C"
{
#include <jack/jack.h>
}

#include "bio2jack.h"

class OutputJACK : public Output
{
    Q_OBJECT
public:
    OutputJACK(QObject * parent = 0);
    ~OutputJACK();

    bool initialize();
    void configure(quint32, int, Qmmp::AudioFormat format);
    qint64 latency();

private:
     //output api
    qint64 writeAudio(unsigned char *data, qint64 maxSize);
    void flush(){};

    // helper functions
    void uninitialize();
    qint64 m;
    bool m_inited, m_configure;
    int jack_device;
};

#endif
