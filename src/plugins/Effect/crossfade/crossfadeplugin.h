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
#ifndef CROSSFADEPLUGIN_H
#define CROSSFADEPLUGIN_H

#include <QMutex>
#include <qmmp/effect.h>

class SoundCore;
class StateHandler;

/**
    @author Ilya Kotov <forkotov02@hotmail.ru>
*/

class CrossfadePlugin : public Effect
{
public:
    CrossfadePlugin();

    virtual ~CrossfadePlugin();

    void applyEffect(Buffer *b);
    void configure(quint32 freq, int chan, Qmmp::AudioFormat format);

private:
    enum State
    {
        WAITING = 0,
        CHECKING,
        PREPARING,
        PROCESSING,
    };

    void mix8(uchar *cur_buf, uchar *prev_buf, uint samples, double volume);
    void mix16(uchar *cur_buf, uchar *prev_buf, uint samples, double volume);
    void mix32(uchar *cur_buf, uchar *prev_buf, uint samples, double volume);

    uchar *m_buffer;
    ulong  m_buffer_at;
    ulong  m_buffer_size;
    qint64 m_overlap;
    int m_state;
    SoundCore *m_core;
    StateHandler *m_handler;

};

#endif
