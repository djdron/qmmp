/***************************************************************************
 *   Copyright (C) 2006-2008 by Ilya Kotov                                 *
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
extern "C" {
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
    OutputPulseAudio(QObject * parent = 0, bool useVolume = TRUE);
    ~OutputPulseAudio();

    bool initialize();
    bool isInitialized() const { return m_inited; }
    void uninitialize();
    void configure(long, int, int, int);
    void stop();
    void pause();
    long written();
    long latency();
    void seek(long);
    //void setVolume(int l, int r);
    //void volume(int *l, int *r);

private:
    // thread run function
    void run();

    // helper function
    void status();

    bool m_inited, m_pause, m_play, m_userStop;
    long m_totalWritten, m_currentSeconds, m_bps;
    int m_rate, m_frequency, m_channels, m_precision;
    pa_simple *m_connection;

};


#endif // OUTPUTPULSEAUDIO_H
