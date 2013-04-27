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


#include "decoder_sid.h"

// Decoder class
DecoderSID::DecoderSID(QIODevice *input) : Decoder(input)
{
    m_player = new sidplayfp();
}

DecoderSID::~DecoderSID()
{
    delete m_player;
}

bool DecoderSID::initialize()
{
    QByteArray data = input()->readAll();

    if(data.isEmpty())
    {
        qWarning("DecoderSID: error: %s", qPrintable(input()->errorString()));
        return false;
    }

    SidTune *tune = new SidTune(0);
    tune->read((const unsigned char*)data.constData(), data.size());
    tune->selectSong(0);

    if(!tune->getStatus())
    {
        qWarning("DecoderSID: error: %s", tune->statusString());
        return false;
    }

    sidbuilder *rs = new ReSIDfpBuilder("ReSIDfp builder");
    rs->create(m_player->info().maxsids());

    SidConfig cfg = m_player->config();

    cfg.frequency    = 44100;
    cfg.samplingMethod = SidConfig::INTERPOLATE;
    cfg.playback     = SidConfig::STEREO;
    cfg.sidEmulation = rs;

    if(!m_player->config(cfg))
    {
        qWarning("DecoderSID: unable to load config, error: %s", m_player->error());
        return false;
    }

    if(!m_player->load(tune))
    {
        qWarning("DecoderSID: unable to load tune, error: %s", m_player->error());
        return false;
    }

    configure(44100, 2);
    qDebug("DecoderSID: initialize succes");
    return true;
}

qint64 DecoderSID::totalTime()
{
    return 0;
}

void DecoderSID::seek(qint64 pos)
{
    Q_UNUSED(pos);
}

int DecoderSID::bitrate()
{
    return 8;
}

qint64 DecoderSID::read(char *data, qint64 size)
{
    return m_player->play((short *)data, size/2) * 2;
}
