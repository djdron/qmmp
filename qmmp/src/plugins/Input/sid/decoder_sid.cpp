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

#include <QFile>
#include <sidplayfp/sidplayfp.h>
#include <sidplayfp/SidTune.h>
#include <sidplayfp/sidbuilder.h>
#include <sidplayfp/SidConfig.h>
#include <sidplayfp/builders/residfp.h>
#include <sidplayfp/builders/resid.h>
#include <sidplayfp/SidInfo.h>
#include <sidplayfp/SidTuneInfo.h>
#include <sidplayfp/SidDatabase.h>
#include "decoder_sid.h"

// Decoder class
DecoderSID::DecoderSID(const QString &url) : Decoder()
{
    m_url = url;
    m_player = new sidplayfp();
}

DecoderSID::~DecoderSID()
{
    delete m_player;
}

bool DecoderSID::initialize()
{
    QString path = m_url;
    path.remove("sid://");
    path.remove(QRegExp("#\\d+$"));
    int track = m_url.section("#", -1).toInt();

    SidTune *tune = new SidTune(0);
    tune->load(qPrintable(path));
    if(!tune->getInfo())
    {
        qWarning("DecoderSID: unable to load tune, error: %s", tune->statusString());
        delete tune;
        return false;
    }
    int count = tune->getInfo()->songs();

    if(track > count || track < 1)
    {
        qWarning("DecoderSID: track number is out of range");
        delete tune;
        return false;
    }

    tune->selectSong(track);

    if(!tune->getStatus())
    {
        qWarning("DecoderSID: error: %s", tune->statusString());
        return false;
    }

    //send metadata for pseudo-protocol
    const SidTuneInfo *tune_info = tune->getInfo();
    QMap<Qmmp::MetaData, QString> metadata;
    metadata.insert(Qmmp::TITLE,  tune_info->infoString(0));
    metadata.insert(Qmmp::ARTIST, tune_info->infoString(1));
    metadata.insert(Qmmp::COMMENT, tune_info->commentString(0));
    metadata.insert(Qmmp::TRACK, QString("%1").arg(track));
    metadata.insert(Qmmp::URL, m_url);
    addMetaData(metadata);

    sidbuilder *rs = new ReSIDfpBuilder("ReSIDfp builder");
    rs->create(m_player->info().maxsids());

    SidConfig cfg = m_player->config();

    cfg.frequency    = 44100;
    cfg.samplingMethod = SidConfig::INTERPOLATE;
    cfg.playback     = SidConfig::STEREO;
    cfg.sidEmulation = rs;
    cfg.fastSampling = false;

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


    char md5[SidTune::MD5_LENGTH];
    tune->createMD5(md5);

    SidDatabase database;

    database.open("/home/user/.qmmp/Songlengths.txt");



    qDebug("length = %d", database.length(md5, track));
    m_length = database.length(md5, track);


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
    if(m_player->time() > m_length)
        return 0;
    return m_player->play((short *)data, size/2) * 2;
}
