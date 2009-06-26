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
#include <qmmp/volumecontrol.h>

class OutputOSS : public Output
{
Q_OBJECT
public:
    OutputOSS(QObject * parent = 0);
    virtual ~OutputOSS();

    bool initialize();
    void configure(quint32, int, int);
    qint64 latency();
    int audio_fd();
    static OutputOSS* instance();

private:
    //output api
    qint64 writeAudio(unsigned char *data, qint64 maxSize);
    void flush(){};

private:
    // thread run function
    //void run();

    // helper functions
    void reset();
    void resetDSP();
    void post();
    void sync();
    //void openMixer();
    void uninitialize();

    QString m_audio_device, m_mixer_device;

    bool m_inited;
    quint32 m_frequency;
    int m_channels, m_precision;

    bool do_select;
    int m_audio_fd;
    long bl, br;
    static OutputOSS *m_instance;
};

class VolumeControlOSS : public VolumeControl
{
    Q_OBJECT
public:
    VolumeControlOSS(QObject *parent = 0);
    ~VolumeControlOSS();

    void setVolume(int left, int right);
    static VolumeControlOSS* instance();

protected:
    void volume(int *left, int *right);

private:
    //oss mixer
    QString m_audio_device;
    void openMixer();
    int m_mixer_fd;
    QString m_mixer_device;
    bool m_master;
    int m_left2, m_right2;
    static VolumeControlOSS *m_instance;
};


#endif
