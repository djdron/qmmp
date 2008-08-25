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
    bool isInitialized() const
    {
        return m_inited;
    }
    void uninitialize();
    void configure(long, int, int, int);
    void stop();
    void pause();
    long written();
    long latency();
    void seek(long);

private:
    // thread run function
    void run();
    // helper functions
    void status();
    QString audio_device;
    bool m_inited, m_configure, m_pause, m_play, m_userStop;
    long m_totalWritten, m_currentSeconds, m_bps;
    int m_rate, m_frequency, m_channels, m_precision, jack_device;
    bool do_select;
    int audio_fd;
};


#endif

