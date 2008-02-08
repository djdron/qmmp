/***************************************************************************
 *   Copyright (C) 2007 by Uriy Zhuravlev stalkerg@gmail.com               *
 *                                                                         *
 *   Copyright (c) 2000-2001 Brad Hughes bhughes@trolltech.com             *
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


#ifndef   OUTPUTOSS_H
#define   OUTPUTOSS_H

class OutputOSS;

#include <qmmp/output.h>
#include <QObject>

class OutputOSS : public Output
{
Q_OBJECT
public:
    OutputOSS(QObject * parent = 0);
    virtual ~OutputOSS();

    bool isInitialized() const { return m_inited; }
    bool initialize();
    void uninitialize();
    void configure(long, int, int, int);
    void stop();
    void pause();
    long written();
    long latency();
    void seek(long);
    void setVolume(int l, int r);
    void volume(int* l,int* r);
    void checkVolume();

private:
    // thread run function
    void run();

    // helper functions
    void reset();
    void resetDSP();
    void status();
    void post();
    void sync();
    void openMixer();

    QString m_audio_device, m_mixer_device;

    bool m_inited, m_pause, m_play, m_userStop, m_master;
    long m_totalWritten, m_currentSeconds, m_bps;
    int stat;
    int m_rate, m_frequency, m_channels, m_precision;

    bool do_select;
    int m_audio_fd, m_mixer_fd;
    long bl, br;
};


#endif
