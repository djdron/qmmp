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

#include "decoder_fluidsynth.h"

// Decoder class
DecoderFluidSynth::DecoderFluidSynth(const QString &path) : Decoder()
{
    m_path = path;
    midi_ptr =  0;
    m_sample_rate = 0;
}

DecoderFluidSynth::~DecoderFluidSynth()
{
    /*if(midi_ptr)
    {
        FluidSynthHelper::instance()->removePtr(midi_ptr);
        FluidSynth_Close(midi_ptr);
    }*/
}

bool DecoderFluidSynth::initialize()
{
    m_totalTime = 0;

    fluid_settings_t *settings = new_fluid_settings();
    synth = new_fluid_synth(settings);
    fluid_synth_sfload(synth, "/usr/share/sounds/sf2/FluidR3_GM.sf2", 1);
    m_player = new_fluid_player(synth);
    fluid_player_add(m_player, qPrintable(m_path));
    fluid_player_play(m_player);
    configure(48000/*m_sample_rate*/, 2, Qmmp::PCM_S16LE);
    qDebug("DecoderFluidSynth: initialize succes");
    return true;
}

qint64 DecoderFluidSynth::totalTime()
{
    return m_totalTime;
}

void DecoderFluidSynth::seek(qint64 pos)
{
}

int DecoderFluidSynth::bitrate()
{
    return 8;
}

qint64 DecoderFluidSynth::read(char *data, qint64 size)
{
    if(fluid_player_get_status(m_player) == FLUID_PLAYER_DONE)
        return 0;

    if(fluid_synth_write_s16(synth, size/4, (int16_t*)data,0,2, (int16_t*)data,1,2) == FLUID_OK)
        return size;
    return 0;
}
