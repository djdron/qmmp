/***************************************************************************
 *   Copyright (C) 2008 by Ilya Kotov                                      *
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


#include <QObject>
#include <QIODevice>
#include <QFile>
#include <math.h>
#include <stdint.h>

#include <qmmp/buffer.h>
#include <qmmp/output.h>
#include <qmmp/recycler.h>

#include "decoder_wildmidi.h"

// Decoder class
bool Iinited = false;

DecoderWildMidi::DecoderWildMidi(const QString &path) : Decoder()
{
    m_path = path;
    midi_ptr =  0;
}

DecoderWildMidi::~DecoderWildMidi()
{
    deinit();
}

bool DecoderWildMidi::initialize()
{
    m_freq = m_bitrate = 0;
    m_chan = 0;
    m_output_size = 0;
    m_totalTime = 0;

    unsigned long int mixer_options = 0;

    if (WildMidi_Init ("/etc/timidity/timidity.cfg", 48000, 0) == -1)
    {
        qDebug("FATAL ERROR");
        return false;
    }

    midi_ptr = WildMidi_Open (m_path.toLocal8Bit());

    _WM_Info *wm_info = WildMidi_GetInfo(midi_ptr);

    m_totalTime = (qint64)wm_info->approx_total_samples  / 48;

    configure(48000, 2, Qmmp::PCM_S16LE);

    qDebug("DecoderWildMidi: initialize succes");
    return true;
}

qint64 DecoderWildMidi::totalTime()
{
    return m_totalTime;
}

void DecoderWildMidi::seek(qint64 pos)
{
    ulong sample = ulong(pos*48);
    WildMidi_FastSeek(midi_ptr, &sample);
}

int DecoderWildMidi::bitrate()
{
    return 8;
}

qint64 DecoderWildMidi::read(char *data, qint64 size)
{
    return WildMidi_GetOutput (midi_ptr, data, size);
}

void DecoderWildMidi::deinit()
{
    WildMidi_Close(midi_ptr);
    WildMidi_Shutdown();
    m_freq = m_bitrate = 0;
    m_chan = 0;
}
