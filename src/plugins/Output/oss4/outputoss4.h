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

#ifndef OUTPUTOSS4_H
#define OUTPUTOSS4_H

#include <qmmp/output.h>
#include <qmmp/volumecontrol.h>

#define DEFAULT_DEV "/dev/dsp"
#define DEFAULT_MIXER "/dev/mixer"

/**
    @author Ilya Kotov <forkotov@hotmail.ru>
*/
class OutputOSS4 : public Output
{
Q_OBJECT
public:
    OutputOSS4(QObject * parent);
    virtual ~OutputOSS4();

    bool initialize();
    void configure(quint32, int, Qmmp::AudioFormat format);
    qint64 latency();
    int fd();

    static OutputOSS4 *instance();

private:
    //output api
    qint64 writeAudio(unsigned char *data, qint64 maxSize);
    void drain();
    void reset();

private:
    void post();
    void sync();
    QString m_audio_device;
    bool m_do_select;
    int m_audio_fd;
    static OutputOSS4 *m_instance;
};

class VolumeControlOSS4 : public VolumeControl
{
    Q_OBJECT
public:
    VolumeControlOSS4(QObject *parent);
    ~VolumeControlOSS4();

    void setVolume(int left, int right);
    void volume(int *left, int *right);
};

#endif
