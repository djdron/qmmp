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

#ifndef   OUTPUTALSA_H
#define   OUTPUTALSA_H

class OutputALSA;


#include <QObject>
extern "C"
{
#include <alsa/asoundlib.h>
}

#include <qmmp/output.h>
#include <qmmp/volumecontrol.h>


class OutputALSA : public Output
{
    Q_OBJECT
public:
    OutputALSA(QObject * parent = 0);
    ~OutputALSA();

    bool initialize();
    void configure(quint32, int, int);
    qint64 latency();
    void pause();

private:
    //output api
    qint64 writeAudio(unsigned char *data, qint64 maxSize);
    void flush();

    // helper functions
    void reset();
    long alsa_write(unsigned char *data, long size);
    void uninitialize();

    bool m_inited;
    bool m_use_mmap;
    //alsa
    snd_pcm_t *pcm_handle;
    char *pcm_name;
    snd_pcm_uframes_t m_chunk_size;
    size_t m_bits_per_frame;
    //prebuffer
    uchar *m_prebuf;
    qint64 m_prebuf_size;
    qint64 m_prebuf_fill;
    bool m_pause;
    bool m_can_pause;
};

class VolumeControlALSA : public VolumeControl
{
    Q_OBJECT
public:
    VolumeControlALSA(QObject *parent = 0);
    ~VolumeControlALSA();

    void setVolume(int left, int right) ;

protected:
    void volume(int *left, int *right);

private:
    //alsa mixer
    int setupMixer(QString card, QString device);
    void parseMixerName(char *str, char **name, int *index);
    int getMixer(snd_mixer_t **mixer, QString card);
    snd_mixer_elem_t* getMixerElem(snd_mixer_t *mixer, char *name, int index);
    snd_mixer_t *mixer;
    snd_mixer_elem_t *pcm_element;
    bool m_use_mmap;
};


#endif // OUTPUTALSA_H
