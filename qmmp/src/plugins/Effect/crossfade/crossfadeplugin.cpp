/***************************************************************************
 *   Copyright (C) 2009 by Ilya Kotov <forkotov02@hotmail.ru>              *
 *   Copyright (C) 2009 by Sebastian Pipping <sebastian@pipping.org>       *
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

#include <QSettings>
#include <qmmp/qmmp.h>
#include <qmmp/statehandler.h>
#include <qmmp/soundcore.h>
#include "crossfadeplugin.h"

CrossfadePlugin::CrossfadePlugin() : Effect()
{
    m_buffer = 0;
    m_buffer_at = 0;
    m_buffer_size = 0;
    m_core = SoundCore::instance() ;
    m_handler = StateHandler::instance();
    m_state = WAITING;
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    m_overlap = settings.value("Crossfade/overlap", 6000).toLongLong();
}

CrossfadePlugin::~CrossfadePlugin()
{
    if(m_buffer)
        free(m_buffer);
}

void CrossfadePlugin::applyEffect(Buffer *b)
{
    switch (m_state)
    {
    case WAITING:
        if(m_core->totalTime() - m_handler->elapsed() < m_overlap + 2000)
        {
            StateHandler::instance()->sendNextTrackRequest();
            m_state = PREPARING;
        }
        return;
    case PREPARING:
        if(m_core->totalTime() - m_handler->elapsed() <  m_overlap)
        {
            if(m_buffer_at + b->nbytes > m_buffer_size)
            {
                m_buffer_size = m_buffer_at + b->nbytes;
                m_buffer = (uchar *)realloc(m_buffer, m_buffer_size);
            }
            memcpy(m_buffer + m_buffer_at, b->data, b->nbytes);
            m_buffer_at += b->nbytes;
            b->nbytes = 0;
            return;
        }
        else if(m_buffer_at > 0)
            m_state = PROCESSING;
        else
            return;
    case PROCESSING:
        if (m_buffer_at > 0)
        {
            double volume = (double)m_buffer_at/m_buffer_size;
            uint size = qMin((ulong)m_buffer_at, b->nbytes);
            for (uint i = 0; i < size / 2; i++)
            {
                ((short*)b->data)[i] =((short*)b->data)[i]*(1.0 - volume)+((short*)m_buffer)[i]*volume;
            }
            m_buffer_at -= size;
            memmove(m_buffer, m_buffer + size, m_buffer_at);
        }
        else
            m_state = WAITING;
    default:
        ;
    }
    return;
}

void CrossfadePlugin::configure(quint32 freq, int chan, Qmmp::AudioFormat format)
{
    //m_buffer_size = m_overlap * freq * chan * AudioParameters::sampleSize(format) / 1000;
    //m_buffer =(uchar *) malloc(m_buffer_size);
    Effect::configure(freq, chan, format);
}
