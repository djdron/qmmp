/***************************************************************************
 *   Copyright (C) 2009 by Ilya Kotov                                      *
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
#include <QRegExp>
#include <QSettings>

#include <cdio/cdtext.h>
#include <cdio/track.h>
#include <cdio/cdda.h>
#include <cdio/audio.h>
#include <cdio/sector.h>
#include <cdio/cd_types.h>
#include <cdio/logging.h>

#include <qmmp/buffer.h>
#include <qmmp/output.h>
#include <qmmp/recycler.h>

#define CDDA_SECTORS 8

#include "decoder_cdaudio.h"

static void log_handler (cdio_log_level_t level, const char message[])
{
    QString str = QString::fromLocal8Bit(message).trimmed();
    switch (level)
    {
    case CDIO_LOG_DEBUG:
        qDebug("DecoderCDAudio: cdio message: %s (level=debug)", qPrintable(str));
    case CDIO_LOG_INFO:
        qDebug("DecoderCDAudio: cdio message: %s (level=info)", qPrintable(str));
        return;
    default:
        qWarning("DecoderCDAudio: cdio message: %s (level=error)", qPrintable(str));
    }
}

// Decoder class

DecoderCDAudio::DecoderCDAudio(const QString &url) : Decoder()
{
    m_bitrate = 0;
    m_totalTime = 0;
    m_first_sector = -1;
    m_last_sector  = -1;
    m_current_sector  = -1;
    m_url = url;
    m_cdio = 0;
}


DecoderCDAudio::~DecoderCDAudio()
{
    m_bitrate = 0;
    if (m_cdio)
    {
        cdio_destroy(m_cdio);
        m_cdio = 0;
    }
}

QList <CDATrack> DecoderCDAudio::generateTrackList(const QString &device)
{
    //read settings
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    int cd_speed = settings.value("cdaudio/speed", 0).toInt();
    bool use_cd_text = settings.value("cdaudio/cdtext", TRUE).toBool();
    QList <CDATrack> tracks;
    cdio_log_set_handler(log_handler); //setup cdio log handler
    CdIo_t *cdio = 0;
    QString device_path = device;
    if (device_path.isEmpty() || device_path == "/")
        device_path = settings.value("device").toString();
    if (device_path.isEmpty() || device_path == "/")
    {
        char **cd_drives = cdio_get_devices_with_cap(0, CDIO_FS_AUDIO, TRUE); //get drive list with CDA disks
        // open first audio capable cd drive
        if (cd_drives && *cd_drives)
        {
            cdio = cdio_open_cd(*cd_drives);
            if (!cdio)
            {
                qWarning("DecoderCDAudio: failed to open CD.");
                cdio_free_device_list(cd_drives);
                return tracks;
            }
            qDebug("DecoderCDAudio: found cd audio capable drive \"%s\"", *cd_drives);
        }
        else
        {
            qWarning("DecoderCDAudio: unable to find cd audio drive.");
            cdio_free_device_list(cd_drives);
            return tracks;
        }
        device_path = QString(*cd_drives);
        if (cd_drives && *cd_drives) //free device list
            cdio_free_device_list(cd_drives);
    }
    else
    {
        cdio = cdio_open_cd(device_path.toAscii().constData());
        if (!cdio)
        {
            qWarning("DecoderCDAudio: failed to open CD.");
            return tracks;
        }
        qDebug("DecoderCDAudio: using cd audio capable drive \"%s\"", qPrintable(device_path));
    }

    if (cd_speed)
    {
        qDebug("DecoderCDAudio: setting drive speed to %dX.", cd_speed);
        if (cdio_set_speed(cdio, 1) != DRIVER_OP_SUCCESS)
            qWarning("DecoderCDAudio: unable to set drive speed to %dX.", cd_speed);
    }

    cdrom_drive_t *pcdrom_drive = cdio_cddap_identify_cdio(cdio, 1, 0); //create paranoya CD-ROM object
    //get first and last track numbers
    int first_track_number = cdio_get_first_track_num(pcdrom_drive->p_cdio);
    int last_track_number = cdio_get_last_track_num(pcdrom_drive->p_cdio);

    if ((first_track_number == CDIO_INVALID_TRACK) || (last_track_number == CDIO_INVALID_TRACK))
    {
        qWarning("DecoderCDAudio: invalid first (last) track number.");
        cdio_destroy(cdio);
        cdio = 0;
        return tracks;
    }
    //fill track list
    for (int i = first_track_number; i <= last_track_number; ++i)
    {
        CDATrack t;
        t.first_sector = cdio_get_track_lsn(pcdrom_drive->p_cdio, i);
        t.last_sector = cdio_get_track_last_lsn(pcdrom_drive->p_cdio, i);
        t.info.setLength((t.last_sector - t.first_sector +1) / 75);
        t.info.setMetaData(Qmmp::TRACK, i);
        t.info.setPath(QString("cdda://%1#%2").arg(device_path).arg(i));
        if ((t.first_sector == CDIO_INVALID_LSN) || (t.last_sector== CDIO_INVALID_LSN))
        {
            qWarning("DecoderCDAudio: invalid stard(end) lsn for the track %d.", i);
            tracks.clear();
            cdio_destroy(cdio);
            cdio = 0;
            return tracks;
        }
        //cd text
        cdtext_t *cdtext = use_cd_text ? cdio_get_cdtext(pcdrom_drive->p_cdio, i) : 0;
        if (cdtext && cdtext->field[CDTEXT_TITLE])
        {
            t.info.setMetaData(Qmmp::TITLE, QString::fromLocal8Bit(cdtext->field[CDTEXT_TITLE]));
            t.info.setMetaData(Qmmp::ARTIST, QString::fromLocal8Bit(cdtext->field[CDTEXT_PERFORMER]));
            t.info.setMetaData(Qmmp::GENRE, QString::fromLocal8Bit(cdtext->field[CDTEXT_GENRE]));
        }
        else
            t.info.setMetaData(Qmmp::TITLE, QString("CDA Track %1").arg(i, 2, 10, QChar('0')));
        tracks  << t;
    }
    qDebug("DecoderCDAudio: found %d audio tracks", tracks.size());
    cdio_destroy(cdio);
    cdio = 0;
    return tracks;
}

qint64 DecoderCDAudio::calculateTrackLength(lsn_t startlsn, lsn_t endlsn)
{
    return ((endlsn - startlsn + 1) * 1000) / 75;
}

bool DecoderCDAudio::initialize()
{
    m_bitrate = 0;
    m_totalTime = 0;
    //extract track from url
    int track_number = m_url.section("#", -1).toInt();
    track_number = qMax(track_number, 1);
    QList <CDATrack> tracks = DecoderCDAudio::generateTrackList(QUrl(m_url).path()); //generate track list
    if (tracks.isEmpty())
    {
        qWarning("DecoderCDAudio: initialize failed");
        return FALSE;
    }
    //find track by number
    int track_at = -1;
    for (int i = 0; i < tracks.size(); ++i)
        if (tracks[i].info.metaData(Qmmp::TRACK).toInt() == track_number)
        {
            track_at = i;
            break;
        }
    if (track_at < 0)
    {
        qWarning("DecoderCDAudio: invalid track number");
        return FALSE;
    }

    if (QUrl(m_url).path().isEmpty() || QUrl(m_url).path() == "/") //try default path from config
    {
        QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
        m_url = QString("cdda://%1#%2").arg(settings.value("device").toString()).arg(track_number);
    }

    if (QUrl(m_url).path() == "/")
    {
        char **cd_drives = cdio_get_devices_with_cap(0, CDIO_FS_AUDIO, TRUE); //get drive list with CDA disks
        // open first audio capable cd drive
        if (cd_drives && *cd_drives)
        {
            m_cdio = cdio_open_cd(*cd_drives);
            if (!m_cdio)
            {
                qWarning("DecoderCDAudio: failed to open CD.");
                cdio_free_device_list(cd_drives);
                return FALSE;
            }
            qDebug("DecoderCDAudio: found cd audio capable drive \"%s\"", *cd_drives);
        }
        else
        {
            qWarning("DecoderCDAudio: unable to find cd audio drive.");
            return FALSE;
        }
        if (cd_drives && *cd_drives) //free device list
            cdio_free_device_list(cd_drives);
    }
    else
    {
        m_cdio = cdio_open_cd(QUrl(m_url).path().toAscii().constData());
        if (!m_cdio)
        {
            qWarning("DecoderCDAudio: failed to open CD.");
            return FALSE;
        }
        qDebug("DecoderCDAudio: using cd audio capable drive \"%s\"", QUrl(m_url).path().toAscii().constData());
    }
    configure(44100, 2, 16);
    m_bitrate = 1411;
    m_totalTime = tracks[track_at].info.length() * 1000;
    m_first_sector = tracks[track_at].first_sector;
    m_current_sector = tracks[track_at].first_sector;
    m_last_sector = tracks[track_at].last_sector;
    StateHandler::instance()->dispatch(tracks[track_at].info.metaData()); //send metadata
    qDebug("DecoderCDAudio: initialize succes");
    return TRUE;
}


qint64 DecoderCDAudio::totalTime()
{
    return m_totalTime;
}

int DecoderCDAudio::bitrate()
{
    return m_bitrate;
}

qint64 DecoderCDAudio::read(char *audio, qint64 maxSize)
{
    long len = 0;
    lsn_t secorts_to_read = qMin(CDDA_SECTORS, (m_last_sector - m_current_sector + 1));
    if(secorts_to_read * CDIO_CD_FRAMESIZE_RAW > maxSize)
    {
        qWarning("DecoderCDAudio: buffer is too small");
        return 0;
    }
    if (secorts_to_read <= 0)
        len = 0;
    else
    {
        if (cdio_read_audio_sectors(m_cdio, audio,
                                    m_current_sector, secorts_to_read) != DRIVER_OP_SUCCESS)
            len = -1;
        else
        {
            len = secorts_to_read * CDIO_CD_FRAMESIZE_RAW;
            m_current_sector += secorts_to_read;
        }
    }
    return len;
}

void DecoderCDAudio::seek(qint64 pos)
{
    m_current_sector = m_first_sector + pos * 75 / 1000;
}
