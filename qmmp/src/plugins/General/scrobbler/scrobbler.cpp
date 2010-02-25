/***************************************************************************
 *   Copyright (C) 2008-2010 by Ilya Kotov                                 *
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
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkProxy>
#include <QNetworkReply>
#include <QByteArray>
#include <QCryptographicHash>
#include <QUrl>
#include <QTime>
#include <QTimer>
#include <QDateTime>
#include <QDir>
#include <qmmp/soundcore.h>
#include <qmmp/qmmpsettings.h>
#include <qmmp/qmmp.h>
#include "scrobbler.h"

#define PROTOCOL_VER "1.2"
#define CLIENT_ID "qmm"
#define CLIENT_VER "0.3"


Scrobbler::Scrobbler(const QString &url,
                     const QString &login,
                     const QString &passw,
                     const QString &name,
                     QObject *parent) : QObject(parent)
{
    m_failure_count = 0;
    m_handshake_count = 0;
    m_http = new QNetworkAccessManager(this);
    m_state = Qmmp::Stopped;
    m_login = login;
    m_passw = passw;
    m_server = url;
    m_name = name;
    connect(QmmpSettings::instance(), SIGNAL(networkSettingsChanged()), SLOT(setupProxy()));
    setupProxy();
    m_disabled = m_login.isEmpty() || m_passw.isEmpty();
    m_passw = QString(QCryptographicHash::hash(m_passw.toAscii(), QCryptographicHash::Md5).toHex());
    connect(m_http, SIGNAL(finished (QNetworkReply *)), SLOT(processResponse(QNetworkReply *)));
    m_core = SoundCore::instance();
    connect (m_core, SIGNAL(metaDataChanged()), SLOT(updateMetaData()));
    connect (m_core, SIGNAL(stateChanged (Qmmp::State)), SLOT(setState(Qmmp::State)));
    m_time = new QTime();
    m_submitedSongs = 0;
    m_handshakeReply = 0;
    m_submitReply = 0;
    m_notificationReply = 0;
    m_ua = QString("iScrobbler/1.5.1qmmp-plugins/%1").arg(Qmmp::strVersion()).toAscii();

    QFile file(QDir::homePath() +"/.qmmp/scrobbler_" + m_name + ".cache");

    if (!m_disabled && file.open(QIODevice::ReadOnly))
    {
        int s;
        QString line, param, value;
        while (!file.atEnd())
        {
            line = QString::fromUtf8(file.readLine()).trimmed();
            if ((s = line.indexOf("=")) < 0)
                continue;

            param = line.left(s);
            value = line.right(line.size() - s - 1);

            if (param == "title")
            {
                m_songCache << SongInfo();
                m_songCache.last().setMetaData(Qmmp::TITLE, value);
            }
            else if (m_songCache.isEmpty())
                continue;
            else if (param == "artist")
                m_songCache.last().setMetaData(Qmmp::ARTIST, value);
            else if (param == "album")
                m_songCache.last().setMetaData(Qmmp::ALBUM, value);
            else if (param == "comment")
                m_songCache.last().setMetaData(Qmmp::COMMENT, value);
            else if (param == "genre")
                m_songCache.last().setMetaData(Qmmp::GENRE, value);
            else if (param == "year")
                m_songCache.last().setMetaData(Qmmp::YEAR, value);
            else if (param == "track")
                m_songCache.last().setMetaData(Qmmp::TRACK, value);
            else if (param == "length")
                m_songCache.last().setLength(value.toInt());
            else if (param == "time")
                m_songCache.last().setTimeStamp(value.toUInt());
        }
        file.close();
    }
    if (!m_disabled)
        handshake();
}


Scrobbler::~Scrobbler()
{
    delete m_time;
    syncCache();
}

void Scrobbler::setState(Qmmp::State state)
{
    m_state = state;
    switch ((uint) state)
    {
    case Qmmp::Playing:
        m_start_ts = QDateTime::currentDateTime().toTime_t();
        m_time->restart();
        if (!isReady() && !m_handshakeReply)
            handshake();
        break;
    case Qmmp::Stopped:
        if (!m_song.metaData().isEmpty()
            && ((m_time->elapsed ()/1000 > 240) || (m_time->elapsed ()/1000 > int(m_song.length()/2)))
            && (m_time->elapsed ()/1000 > 60))
        {
            m_song.setTimeStamp(m_start_ts);
            m_songCache << m_song;
            syncCache();
        }

        m_song.clear();
        if (m_songCache.isEmpty())
            break;

        if (isReady() && !m_submitReply)
            submit();
        break;
    default:
        ;
    }
}

void Scrobbler::updateMetaData()
{
    QMap <Qmmp::MetaData, QString> metadata = m_core->metaData();
    if (m_state == Qmmp::Playing
            && !metadata.value(Qmmp::TITLE).isEmpty()      //skip empty tags
            && !metadata.value(Qmmp::ARTIST).isEmpty()
            && m_core->totalTime()                         //skip stream
            && !metadata.value(Qmmp::ARTIST).contains("=") //skip tags with special symbols
            && !metadata.value(Qmmp::TITLE).contains("=")
            && !metadata.value(Qmmp::ALBUM).contains("="))
    {
        metadata[Qmmp::ARTIST].replace("%", QUrl::toPercentEncoding("%")); //replace special symbols
        metadata[Qmmp::ALBUM].replace("%", QUrl::toPercentEncoding("%"));
        metadata[Qmmp::TITLE].replace("%", QUrl::toPercentEncoding("%"));
        metadata[Qmmp::ARTIST].replace("&", QUrl::toPercentEncoding("&"));
        metadata[Qmmp::ALBUM].replace("&", QUrl::toPercentEncoding("&"));
        metadata[Qmmp::TITLE].replace("&", QUrl::toPercentEncoding("&"));
        m_song = SongInfo(metadata, m_core->totalTime()/1000);
        if (isReady() && !m_notificationReply && !m_submitReply)
            sendNotification(m_song);
    }
}

void Scrobbler::processResponse(QNetworkReply *reply)
{
    QString data;
    if (reply->error() == QNetworkReply::NoError)
        data = reply->readAll();
    else
        data = reply->errorString ();

    data = data.trimmed();
    if (data.startsWith("OK"))
    {
        m_failure_count = 0;
        m_handshake_count = 0;
    }

    if (reply == m_handshakeReply)
    {
        m_submitUrl.clear();
        m_session.clear();
        m_nowPlayingUrl.clear();
        m_failure_count = 0;
        QStringList strlist = data.split("\n");
        if (!strlist[0].contains("OK") || strlist.size() < 4)
        {
            qWarning("Scrobbler[%s]: handshake phase error", qPrintable(m_name));
            m_disabled = TRUE;
            if(strlist[0].contains("BANNED"))
                qWarning("Scrobbler[%s]: client has been banned", qPrintable(m_name));
            else if(strlist[0].contains("BADAUTH"))
                qWarning("Scrobbler[%s]: incorrect user/password", qPrintable(m_name));
            else if(strlist[0].contains("BADTIME"))
                qWarning("Scrobbler[%s]: incorrect system time", qPrintable(m_name));
            else
            {
                qWarning("Scrobbler[%s]: service error: %s", qPrintable(m_name), qPrintable(strlist[0]));
                m_disabled = FALSE;
                m_handshake_count++;
                QTimer::singleShot (60000 * qMin(m_handshake_count^2, 120) , this, SLOT(handshake()));
                qWarning("Scrobbler[%s]: waiting %d minutes...", qPrintable(m_name),
                         qMin(m_handshake_count^2, 120));
            }
        }
        else if (strlist.size() > 3) //process handshake response
        {
            qDebug("Scrobbler[%s]: reading handshake response",qPrintable(m_name));
            qDebug("Scrobbler[%s]: Session ID: %s",qPrintable(m_name),qPrintable(strlist[1]));
            qDebug("Scrobbler[%s]: Now-Playing URL: %s",qPrintable(m_name),qPrintable(strlist[2]));
            qDebug("Scrobbler[%s]: Submission URL: %s",qPrintable(m_name),qPrintable(strlist[3]));
            m_submitUrl = strlist[3];
            m_nowPlayingUrl = strlist[2];
            m_session = strlist[1];
            updateMetaData(); //send now-playing notification for already playing song
            if (!m_songCache.isEmpty()) //submit recent songs
                submit();
        }
    }
    else if (reply == m_submitReply)
    {
        m_submitReply = 0;
        if (!data.startsWith("OK"))
        {
            qWarning("Scrobbler[%s]: submit error",qPrintable(m_name));
            if(data.contains("BADSESSION"))
            {
                qWarning("Scrobbler[%s]: invalid session ID",qPrintable(m_name));
                qWarning("Scrobbler[%s]: performing re-handshake",qPrintable(m_name));
                handshake();
            }
            else
            {
                qWarning("Scrobbler[%s]: %s",qPrintable(m_name),qPrintable(data));
                m_failure_count ++;
            }
        }
        else
        {
            qDebug("Scrobbler[%s]: submited %d song(s)",qPrintable(m_name), m_submitedSongs);
            while (m_submitedSongs)
            {
                m_submitedSongs--;
                m_songCache.removeFirst ();
            }
            if (!m_songCache.isEmpty()) //submit remaining songs
                submit();
            else
                updateMetaData();
        }
    }
    else if (reply == m_notificationReply)
    {
        m_notificationReply = 0;
        if (!data.startsWith("OK"))
        {
            qWarning("Scrobbler[%s]: notification error",qPrintable(m_name));
            if(data.contains("BADSESSION"))
            {
                qWarning("Scrobbler[%s]: invalid session ID",qPrintable(m_name));
                qWarning("Scrobbler[%s]: performing re-handshake",qPrintable(m_name));
                handshake();
            }
            else
            {
                qWarning("Scrobbler[%s]: %s",qPrintable(m_name),qPrintable(data));
                m_failure_count ++;
            }
        }
        else
            qDebug("Scrobbler[%s]: Now-Playing notification done", qPrintable(m_name));
    }
    if(m_failure_count >= 3)
    {
        qWarning("Scrobbler[%s]: performing re-handshake",qPrintable(m_name));
        handshake();
    }
    reply->deleteLater();
}

void Scrobbler::setupProxy()
{
    QmmpSettings *gs = QmmpSettings::instance();
    if (gs->isProxyEnabled())
    {
        QNetworkProxy proxy(QNetworkProxy::HttpProxy, gs->proxy().host(),  gs->proxy().port());
        if(gs->useProxyAuth())
        {
            proxy.setUser(gs->proxy().userName());
            proxy.setPassword(gs->proxy().password());
        }
        m_http->setProxy(proxy);
    }
    else
        m_http->setProxy(QNetworkProxy::NoProxy);
}

void Scrobbler::handshake()
{
    if (m_disabled)
        return;
    qDebug("Scrobbler[%s] handshake request",qPrintable(m_name));
    uint ts = QDateTime::currentDateTime().toTime_t();
    qDebug("Scrobbler[%s]: current time stamp %d",qPrintable(m_name),ts);
    QString auth_tmp = QString("%1%2").arg(m_passw).arg(ts);
    QByteArray auth = QCryptographicHash::hash(auth_tmp.toAscii (), QCryptographicHash::Md5);
    auth = auth.toHex();
    QUrl url(QString("http://") + m_server + "/?");
    url.addQueryItem("hs", "true");
    url.addQueryItem("p", PROTOCOL_VER);
    url.addQueryItem("c", CLIENT_ID);
    url.addQueryItem("v", CLIENT_VER);
    url.addQueryItem("u", m_login);
    url.addQueryItem("t", QString::number(ts));
    url.addQueryItem("a", QString(auth));
    url.setPort(80);
    qDebug("Scrobbler[%s]: request url: %s",qPrintable(m_name),qPrintable(url.toString()));
    QNetworkRequest request(url);
    request.setRawHeader("User-Agent",  m_ua);
    request.setRawHeader("Host",url.host().toAscii());
    request.setRawHeader("Accept", "*/*");
    m_handshakeReply = m_http->get(request);
}

void Scrobbler::submit()
{
    qDebug("Scrobbler[%s]: submit request", qPrintable(m_name));
    if (m_songCache.isEmpty())
        return;
    m_submitedSongs = qMin(m_songCache.size(),25);
    QString body = QString("s=%1").arg(m_session);
    for (int i = 0; i < m_submitedSongs; ++i)
    {
        SongInfo info = m_songCache[i];
        body += QString("&a[%9]=%1&t[%9]=%2&i[%9]=%3&o[%9]=%4&r[%9]=%5&l[%9]=%6&b[%9]=%7&n[%9]=%8&m[%9]=")
                .arg(info.metaData(Qmmp::ARTIST))
                .arg(info.metaData(Qmmp::TITLE))
                .arg(info.timeStamp())
                .arg("P")
                .arg("")
                .arg(info.length())
                .arg(info.metaData(Qmmp::ALBUM))
                .arg(info.metaData(Qmmp::TRACK))
                .arg(i);
    }
    //qDebug("%s",qPrintable(body));
    QUrl url(m_submitUrl);
    url.setPort(80);
    QNetworkRequest request(url);
    request.setRawHeader("User-Agent", m_ua);
    request.setRawHeader("Host",url.host().toAscii());
    request.setRawHeader("Accept", "*/*");
    request.setHeader(QNetworkRequest::ContentLengthHeader,
                      QUrl::toPercentEncoding(body,":/[]&=%").size());
    m_submitReply = m_http->post(request, QUrl::toPercentEncoding(body,":/[]&=%"));
}

void Scrobbler::sendNotification(const SongInfo &info)
{
    qDebug("Scrobbler[%s] sending notification", qPrintable(m_name));
    QString body = QString("s=%1").arg(m_session);
    body += QString("&a=%1&t=%2&b=%3&l=%4&n=%5&m=")
            .arg(info.metaData(Qmmp::ARTIST))
            .arg(info.metaData(Qmmp::TITLE))
            .arg(info.metaData(Qmmp::ALBUM))
            .arg(info.length())
            .arg(info.metaData(Qmmp::TRACK));
    QUrl url(m_nowPlayingUrl);
    url.setPort(80);
    QNetworkRequest request(url);
    request.setRawHeader("User-Agent", m_ua);
    request.setRawHeader("Host", url.host().toAscii());
    request.setRawHeader("Accept", "*/*");
    request.setHeader(QNetworkRequest::ContentLengthHeader,
                      QUrl::toPercentEncoding(body,":/[]&=%").size());
    m_notificationReply = m_http->post(request, QUrl::toPercentEncoding(body,":/[]&=%"));
}

bool Scrobbler::isReady()
{
    return !m_submitUrl.isEmpty() && !m_session.isEmpty();
}

void Scrobbler::syncCache()
{
    QFile file(QDir::homePath() +"/.qmmp/scrobbler_" + m_name + ".cache");
    if (m_songCache.isEmpty())
    {
        file.remove();
        return;
    }
    file.open(QIODevice::WriteOnly);
    foreach(SongInfo m, m_songCache)
    {
        file.write(QString("title=%1").arg(m.metaData(Qmmp::TITLE)).toUtf8() +"\n");
        file.write(QString("artist=%1").arg(m.metaData(Qmmp::ARTIST)).toUtf8() +"\n");
        file.write(QString("album=%1").arg(m.metaData(Qmmp::ALBUM)).toUtf8() +"\n");
        file.write(QString("comment=%1").arg(m.metaData(Qmmp::COMMENT)).toUtf8() +"\n");
        file.write(QString("genre=%1").arg(m.metaData(Qmmp::GENRE)).toUtf8() +"\n");
        file.write(QString("year=%1").arg(m.metaData(Qmmp::YEAR)).toUtf8() +"\n");
        file.write(QString("track=%1").arg(m.metaData(Qmmp::TRACK)).toUtf8() +"\n");
        file.write(QString("length=%1").arg(m.length()).toUtf8() +"\n");
        file.write(QString("time=%1").arg(m.timeStamp()).toUtf8() +"\n");
    }
    file.close();
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
    m_start_ts = other.timeStamp();
}

SongInfo::~SongInfo()
{}

void SongInfo::operator=(const SongInfo &info)
{
    m_metadata = info.metaData();
    m_length = info.length();
    m_start_ts = info.timeStamp();
}

bool SongInfo::operator==(const SongInfo &info)
{
    return (m_metadata == info.metaData()) && (m_length == info.length()) && (m_start_ts == info.timeStamp());
}

bool SongInfo::operator!=(const SongInfo &info)
{
    return !operator==(info);
}

void SongInfo::setMetaData(const QMap <Qmmp::MetaData, QString> metadata)
{
    m_metadata = metadata;
}

void SongInfo::setMetaData(Qmmp::MetaData key, const QString &value)
{
    m_metadata.insert(key, value);
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

void SongInfo::setTimeStamp(uint ts)
{
    m_start_ts = ts;
}

uint SongInfo::timeStamp() const
{
    return m_start_ts;
}
