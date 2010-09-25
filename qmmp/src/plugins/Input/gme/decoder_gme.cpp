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

#include <qmmp/statehandler.h>
#include "gmehelper.h"
#include "decoder_gme.h"

// Decoder class
DecoderGme::DecoderGme(const QString &path) : Decoder()
{
    m_path = path;
    m_emu = 0;
}

DecoderGme::~DecoderGme()
{}

bool DecoderGme::initialize()
{
    int track = m_path.section("#", -1).toInt();
    m_emu = m_helper.load(m_path);
    if(!m_emu)
        return false;

    int count = m_emu->track_count();
    if(track > count + 1 || track < 0)
    {
        qWarning("DecoderGme: track number is out of range");
        delete m_emu;
        m_emu = 0;
        return false;
    }
    m_emu->start_track(track - 1);
    track_info_t track_info;
    if(!m_emu->track_info(&track_info))
    {
        if(track_info.length <= 0)
            track_info.length = track_info.intro_length + track_info.loop_length * 2;
    }
    if(track_info.length <= 0)
        track_info.length = (long) (2.5 * 60 * 1000);
    if(track_info.length < m_helper.fadeLength())
        track_info.length += m_helper.fadeLength();
    m_emu->set_fade(track_info.length - m_helper.fadeLength(), m_helper.fadeLength());
    QMap<Qmmp::MetaData, QString> metadata;
    metadata.insert(Qmmp::TITLE, track_info.song);
    metadata.insert(Qmmp::ARTIST, track_info.author);
    metadata.insert(Qmmp::COMMENT, track_info.comment);
    metadata.insert(Qmmp::TRACK, QString("%1").arg(track));
    metadata.insert(Qmmp::URL, m_path);
    StateHandler::instance()->dispatch(metadata);
    m_totalTime = track_info.length;
    configure(44100, 2);
    qDebug("DecoderGme: initialize succes");
    return true;
}

qint64 DecoderGme::totalTime()
{
    return m_totalTime;
}

void DecoderGme::seek(qint64 pos)
{
    m_emu->seek(pos);
}

int DecoderGme::bitrate()
{
    return 8;
}

qint64 DecoderGme::read(char *data, qint64 size)
{
    if(m_emu->track_ended())
        return 0;
    if (m_emu->play(size/2, (short*)data))
    {
        return 0;
    }
    return size;
}
