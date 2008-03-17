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
extern "C" {
#include <alsa/asoundlib.h>
}

#include <qmmp/output.h>


class OutputALSA : public Output
{
Q_OBJECT
public:
    OutputALSA(QObject * parent = 0, bool useVolume = TRUE);
    ~OutputALSA();

    bool initialize();
    bool isInitialized() const { return m_inited; }
    void uninitialize();
    void configure(long, int, int, int);
    void stop();
    void pause();
    long written();
    long latency();
    void seek(long);
    void setVolume(int l, int r);
    void volume(int *l, int *r);

private:
    // thread run function
    void run();

    // helper functions
    void reset();
    void status();

    bool m_inited, m_pause, m_play, m_userStop;
    long m_totalWritten, m_currentSeconds, m_bps;
    int m_rate, m_frequency, m_channels, m_precision;
    //alsa
    snd_pcm_t *pcm_handle;
    snd_pcm_stream_t stream;
    snd_pcm_hw_params_t *hwparams;
    char *pcm_name;
    //alsa

    //alsa mixer
    int setupMixer(QString card, QString device);
    void parseMixerName(char *str, char **name, int *index);
    int getMixer(snd_mixer_t **mixer, QString card);
    snd_mixer_elem_t* getMixerElem(snd_mixer_t *mixer, char *name, int index);
    snd_mixer_t *mixer;
    snd_mixer_elem_t *pcm_element;
};


#endif // OUTPUTALSA_H
