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

#include <QFile>
#include <qmmp/soundcore.h>

#include "playerobject.h"

PlayerObject::PlayerObject(QObject *parent)
        : QDBusAbstractAdaptor(parent)
{
    setAutoRelaySignals(TRUE);
    m_core = SoundCore::instance();
}

PlayerObject::~PlayerObject()
{}

void PlayerObject::Next()
{

}

void PlayerObject::Prev()
{

}

void PlayerObject::Pause()
{
    m_core->pause();
}

void PlayerObject::Stop()
{
    m_core->stop();
}

void PlayerObject::Play()
{
    //m_core->play();
}

PlayerStatus PlayerObject::GetStatus()
{
    PlayerStatus st;
    st.state = 2;
    if (m_core->state() == Qmmp::Playing)
        st.state = 0;
    else if (m_core->state() == Qmmp::Paused)
        st.state = 1;
    st.random = 0; //TODO playlist support
    st.repeat = 0;
    st.repeatPlayList = 0;
    return st;
}

QVariantMap PlayerObject::GetMetadata()
{
    QVariantMap map;

    if (QFile::exists(m_core->metaData(Qmmp::URL)))
        map.insert("location", "file://" +m_core->metaData(Qmmp::URL));
    else
        map.insert("location", m_core->metaData(Qmmp::URL));
    map.insert("title", m_core->metaData(Qmmp::TITLE));
    map.insert("artist", m_core->metaData(Qmmp::ARTIST));
    map.insert("album", m_core->metaData(Qmmp::ALBUM));
    map.insert("tracknumber", m_core->metaData(Qmmp::TRACK));
    map.insert("time", m_core->length());
    map.insert("mtime", m_core->length() * 1000);
    map.insert("genre", m_core->metaData(Qmmp::GENRE));
    map.insert("comment", m_core->metaData(Qmmp::COMMENT));
    map.insert("audio-bitrate", m_core->bitrate());
    map.insert("audio-samplerate", m_core->frequency());
    map.insert("year", m_core->metaData(Qmmp::YEAR));
    return map;
}

void PlayerObject::VolumeSet(int volume)
{
    int balance = (m_core->rightVolume() - m_core->leftVolume()) * 100/VolumeGet();
    m_core->setVolume(volume - qMax(balance,0)*volume/100,
                      volume + qMin(balance,0)*volume/100);
}

int PlayerObject::VolumeGet()
{
    return qMax(m_core->leftVolume(), m_core->rightVolume());
}

void PlayerObject::PositionSet(int pos)
{
    m_core->seek(pos / 1000);
}

qint64 PlayerObject::PositionGet()
{
    return m_core->elapsed() * 1000;
}
