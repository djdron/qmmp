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

#include <QMenu>
#include <QHttp>
#include <QByteArray>
#include <QCryptographicHash>
#include <QUrl>
#include <QTime>
#include <QSettings>
#include <QDir>
#include <qmmp/soundcore.h>
#include <qmmp/qmmp.h>

#include "scrobbler.h"

#define SCROBBLER_HS_URL "post.audioscrobbler.com"
#define PROTOCOL_VER "1.2"
#define CLIENT_ID "qmm"
#define CLIENT_VER "0.2"


Scrobbler::Scrobbler(QObject *parent)
        : General(parent)
{
    m_http = new QHttp(this);
    m_http->setHost(SCROBBLER_HS_URL, 80);
    m_state = Qmmp::Stopped;
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("Scrobbler");
    m_login = settings.value("login").toString();
    m_passw = settings.value("password").toString();
    settings.endGroup();
    //load global proxy settings
    if (Qmmp::useProxy())
        m_http->setProxy(Qmmp::proxy().host(),
                         Qmmp::proxy().port(),
                         Qmmp::useProxyAuth() ? Qmmp::proxy().userName() : QString(),
                         Qmmp::useProxyAuth() ? Qmmp::proxy().password() : QString());

    m_disabled = m_login.isEmpty() || m_passw.isEmpty();
    m_passw = QString(QCryptographicHash::hash(m_passw.toAscii(), QCryptographicHash::Md5).toHex());
    connect(m_http, SIGNAL(requestFinished (int, bool)), SLOT(processResponse(int, bool)));
    connect(m_http, SIGNAL(readyRead (const QHttpResponseHeader&)),
            SLOT(readResponse(const QHttpResponseHeader&)));

    m_core = SoundCore::instance();
    connect (m_core, SIGNAL(metaDataChanged()), SLOT(updateMetaData()));
    connect (m_core, SIGNAL(stateChanged (Qmmp::State)), SLOT(setState(Qmmp::State)));

    m_time = new QTime();
    m_submitedSongs = 0;
    m_handshakeid = 0;
    m_submitid = 0;
    m_notificationid = 0;
    if (!m_disabled)
        handshake();
}


Scrobbler::~Scrobbler()
{
    delete m_time;
}

void Scrobbler::setState(Qmmp::State state)
{
    m_state = state;
    if (m_disabled)
        return;
    switch ((uint) state)
    {
    case Qmmp::Playing:
    {
        m_start_ts = time(NULL);
        m_time->restart();
        if (!isReady() && m_handshakeid == 0)
            handshake();
        break;
    }
    case Qmmp::Paused:
    {
        break;
    }
    case Qmmp::Stopped:
    {
        if (!m_song.metaData().isEmpty()
                && ((m_time->elapsed ()/1000 > 240)
                    || (m_time->elapsed ()/1000 > int(m_song.length()/2)))
                && (m_time->elapsed ()/1000 > 60))
        {
            m_songCache << m_song;
            m_timeStamps << m_start_ts;
        }

        m_song.clear();
        if (m_songCache.isEmpty())
            break;

        if (m_http->error() != QHttp::NoError)
            m_http->clearPendingRequests ();

        if (isReady() && m_submitid == 0)
        {
            submit();
        }
        break;
    }
    }
}

void Scrobbler::updateMetaData()
{
    QMap <Qmmp::MetaData, QString> metadata = m_core->metaData();
    if (m_state == Qmmp::Playing
            && !metadata.value(Qmmp::TITLE).isEmpty()      //skip empty tags
            && !metadata.value(Qmmp::ARTIST).isEmpty()
            && m_core->totalTime()                         //skip stream
            && !metadata.value(Qmmp::ARTIST).contains("&") //skip tags with special symbols
            && !metadata.value(Qmmp::TITLE).contains("&")
            && !metadata.value(Qmmp::ALBUM).contains("&")
            && !metadata.value(Qmmp::ARTIST).contains("=")
            && !metadata.value(Qmmp::TITLE).contains("=")
            && !metadata.value(Qmmp::ALBUM).contains("="))
    {
        m_song = SongInfo(metadata, m_core->totalTime()/1000);
        if (isReady() && m_notificationid == 0)
            sendNotification(m_song);
    }
}

void Scrobbler::processResponse(int id, bool error)
{
    if (error)
    {
        qWarning("Scrobbler: %s", qPrintable(m_http->errorString ()));
        //TODO hard failure handling

        if (id == m_submitid)
            m_submitid = 0;
        else if (id == m_handshakeid)
            m_handshakeid = 0;
        else if (id == m_notificationid)
            m_notificationid = 0;
        return;
    }
    QString str(m_array);
    QStringList strlist = str.split("\n");

    if (id == m_handshakeid)
    {
        m_handshakeid = 0;
        if (!strlist[0].contains("OK") || strlist.size() < 4)
        {
            qWarning("Scrobbler: handshake phase error: %s", qPrintable(strlist[0]));
            //TODO badtime handling
            return;
        }
        if (strlist.size() > 3) //process handshake response
        {
            qDebug("Scrobbler: reading handshake response");
            qDebug("Scrobbler: Session ID: %s",qPrintable(strlist[1]));
            qDebug("Scrobbler: Now-Playing URL: %s",qPrintable(strlist[2]));
            qDebug("Scrobbler: Submission URL: %s",qPrintable(strlist[3]));
            m_submitUrl = strlist[3];
            m_nowPlayingUrl = strlist[2];
            m_session = strlist[1];
            updateMetaData(); //send now-playing notification for already playing song
            return;
        }
    }
    else if (id == m_submitid)
    {
        m_submitid = 0;
        if (!strlist[0].contains("OK"))
        {
            qWarning("Scrobbler: submit error: %s", qPrintable(strlist[0]));
            //TODO badsession handling
            return;
        }
        qWarning("Scrobbler: submited %d song(s)", m_submitedSongs);
        while (m_submitedSongs)
        {
            m_submitedSongs--;
            m_timeStamps.removeFirst ();
            m_songCache.removeFirst ();
        }
    }
    else if (id == m_notificationid)
    {
        m_notificationid = 0;
        if (!strlist[0].contains("OK"))
        {
            qWarning("Scrobbler: notification error: %s", qPrintable(strlist[0]));
            //TODO badsession handling
            return;
        }
        qDebug("Scrobbler: Now-Playing notification done");
    }
    m_array.clear();
}

void Scrobbler::readResponse(const QHttpResponseHeader &header)
{
    if (header.statusCode () != 200)
    {
        qWarning("Scrobbler: error: %s",qPrintable(header.reasonPhrase ()));
        //TODO Failure Handling
        return;
    }
    m_array = m_http->readAll();
}

void Scrobbler::handshake()
{
    qDebug("Scrobbler::handshake()");
    time_t ts = time(NULL);
    qDebug("Scrobbler: current time stamp %ld",ts);
    QString auth_tmp = QString("%1%2").arg(m_passw).arg(ts);
    QByteArray auth = QCryptographicHash::hash(auth_tmp.toAscii (), QCryptographicHash::Md5);
    auth = auth.toHex();

    QString url = QString("%1?hs=true&p=%2&c=%3&v=%4&u=%5&t=%6&a=%7")
                  .arg("/")
                  .arg(PROTOCOL_VER)
                  .arg(CLIENT_ID)
                  .arg(CLIENT_VER)
                  .arg(m_login)
                  .arg(ts)
                  .arg(QString(auth));

    qDebug("Scrobbler: request url: %s",qPrintable(url));
    m_http->setHost(SCROBBLER_HS_URL, 80);
    m_handshakeid = m_http->get(url);
}

void Scrobbler::submit()
{
    qDebug("Scrobbler::submit()");
    if (m_songCache.isEmpty())
        return;
    m_submitedSongs = m_songCache.size();
    QString body = QString("s=%1").arg(m_session);
    for (int i = 0; i < qMin(m_songCache.size(), 25); ++i)
    {
        SongInfo info = m_songCache[i];
        body += QString("&a[%9]=%1&t[%9]=%2&i[%9]=%3&o[%9]=%4&r[%9]=%5&l[%9]=%6&b[%9]=%7&n[%9]=%8&m[%9]=")
                .arg(info.metaData(Qmmp::ARTIST))
                .arg(info.metaData(Qmmp::TITLE))
                .arg( m_timeStamps[i])
                .arg("P")
                .arg("")
                .arg(info.length())
                .arg(info.metaData(Qmmp::ALBUM))
                .arg(info.metaData(Qmmp::TRACK))
                .arg(i);
    }
    QUrl url(m_submitUrl);
    m_http->setHost(url.host(), url.port());
    QHttpRequestHeader header("POST", url.path());
    header.setContentType("application/x-www-form-urlencoded");
    header.setValue("User-Agent","iScrobbler/1.5.1qmmp-plugins/" + Qmmp::strVersion());
    header.setValue("Host",url.host());
    header.setValue("Accept", "*/*");
    header.setContentLength(QUrl::toPercentEncoding(body,":/[]&=").size());
    qDebug("Scrobbler: submit request header");
    qDebug(qPrintable(header.toString().trimmed()));
    qDebug("*****************************");
    m_submitid = m_http->request(header, QUrl::toPercentEncoding(body,":/[]&="));
}

void Scrobbler::sendNotification(const SongInfo &info)
{
    qDebug("Scrobbler::sendNotification()");
    QString body = QString("s=%1").arg(m_session);
    body += QString("&a=%1&t=%2&b=%3&l=%4&n=%5&m=")
            .arg(info.metaData(Qmmp::ARTIST))
            .arg(info.metaData(Qmmp::TITLE))
            .arg(info.metaData(Qmmp::ALBUM))
            .arg(info.length())
            .arg(info.metaData(Qmmp::TRACK));
    QUrl url(m_nowPlayingUrl);
    m_http->setHost(url.host(), url.port());
    QHttpRequestHeader header("POST", url.path());
    header.setContentType("application/x-www-form-urlencoded");
    header.setValue("User-Agent","iScrobbler/1.5.1qmmp-plugins/" + Qmmp::strVersion());
    header.setValue("Host",url.host());
    header.setValue("Accept", "*/*");
    header.setContentLength(QUrl::toPercentEncoding(body,":/[]&=").size());
    qDebug("Scrobbler: Now-Playing notification request header");
    qDebug(qPrintable(header.toString().trimmed()));
    qDebug("*****************************");
    m_notificationid = m_http->request(header, QUrl::toPercentEncoding(body,":/[]&="));
}

bool Scrobbler::isReady()
{
    return !m_submitUrl.isEmpty() && !m_session.isEmpty();
}

SongInfo::SongInfo()
{
    m_length = 0;
}

SongInfo::SongInfo(const QMap <Qmmp::MetaData, QString> metadata, qint64 length)
{
    m_metadata = metadata;
    m_length = length;
}

SongInfo::SongInfo(const SongInfo &other)
{
    m_metadata = other.metaData();
    m_length  = other.length();
}

SongInfo::~SongInfo()
{}

void SongInfo::operator=(const SongInfo &info)
{
    m_metadata = info.metaData();
    m_length = info.length();
}

bool SongInfo::operator==(const SongInfo &info)
{
    return (m_metadata == info.metaData()) && (m_length == info.length());
}

bool SongInfo::operator!=(const SongInfo &info)
{
    return !operator==(info);
}

void SongInfo::setMetaData(const QMap <Qmmp::MetaData, QString> metadata)
{
    m_metadata = metadata;
}

void SongInfo::setLength(qint64 l)
{
    m_length = l;
}

const QMap <Qmmp::MetaData, QString> SongInfo::metaData() const
{
    return m_metadata;
}

const QString SongInfo::metaData(Qmmp::MetaData key) const
{
    return m_metadata.value(key);
}

qint64 SongInfo::length () const
{
    return m_length;
}

void SongInfo::clear()
{
    m_metadata.clear();
    m_length = 0;
}
