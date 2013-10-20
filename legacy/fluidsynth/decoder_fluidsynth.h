/***************************************************************************
 *   Copyright (C) 2013 by Ilya Kotov                                      *
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
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#ifndef DECODER_FLUIDSYNTH_H
#define DECODER_FLUIDSYNTH_H

#include <fluidsynth.h>
#include <qmmp/decoder.h>

class DecoderFluidSynth : public Decoder
{
public:
    DecoderFluidSynth(const QString &path);
    virtual ~DecoderFluidSynth();

    // Standard Decoder API
    bool initialize();
    qint64 totalTime();
    int bitrate();
    qint64 read(char *data, qint64 size);
    void seek(qint64);

private:
    void *midi_ptr;
    qint64 m_totalTime;
    quint32 m_sample_rate;
    QString m_path;
    fluid_player_t *m_player;
    fluid_synth_t *synth;
};

#endif // DECODER_FLUIDSYNTH_H
