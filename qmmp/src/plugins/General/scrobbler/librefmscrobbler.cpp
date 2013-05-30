/***************************************************************************
 *   Copyright (C) 2008-2013 by Ilya Kotov                                 *
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
#include "librefmscrobbler.h"

#define PROTOCOL_VER "1.2.1"
#define CLIENT_ID "qmm"
#define CLIENT_VER "0.7"
#define SCROBBLER_LIBREFM_URL "http://turtle.libre.fm/"


LibrefmScrobbler::LibrefmScrobbler(const QString &login, const QString &passw, QObject *parent)
    : QObject(parent)
{
    m_failure_count = 0;
    m_handshake_count = 0;
    m_submitedSongs = 0;
    m_handshakeReply = 0;
    m_submitReply = 0;
    m_notificationReply = 0;
    m_ua = QString("iScrobbler/1.5.1qmmp-plugins/%1").arg(Qmmp::strVersion()).toAscii();
    m_login = login;
    m_passw = passw;
    m_passw = QString(QCryptographicHash::hash(m_passw.toAscii(), QCryptographicHash::Md5).toHex());
    m_disabled = login.isEmpty() || passw.isEmpty();
    m_core = SoundCore::instance();
    m_cache = new ScrobblerCache(QDir::homePath() +"/.qmmp/scrobbler_librefm.cache");
    m_http = new QNetworkAccessManager(this);
    m_time = new QTime();

    connect(m_http, SIGNAL(finished (QNetworkReply *)), SLOT(processResponse(QNetworkReply *)));
    connect(QmmpSettings::instance(), SIGNAL(networkSettingsChanged()), SLOT(setupProxy()));
    connect (m_core, SIGNAL(metaDataChanged()), SLOT(updateMetaData()));
    connect (m_core, SIGNAL(stateChanged (Qmmp::State)), SLOT(setState(Qmmp::State)));

    setupProxy();
    m_cachedSongs = m_cache->load();

    m_start_ts = QDateTime::currentDateTime().toTime_t();
    handshake();
}


LibrefmScrobbler::~LibrefmScrobbler()
{
    m_cache->save(m_cachedSongs);
    delete m_time;
    delete m_cache;
}

void LibrefmScrobbler::setState(Qmmp::State state)
{
    static Qmmp::State previousState = state;
    static int elapsed = 0;
    switch ((uint) state)
    {
    case Qmmp::Playing:
        if (previousState != Qmmp::Paused)
        {
            m_start_ts = QDateTime::currentDateTime().toTime_t();
            elapsed = 0;
        }
        else
        {
            qDebug("LibrefmScrobbler: resuming from %d seconds played", elapsed / 1000);
        }
        m_time->restart();
        if (!isReady() && !m_handshakeReply)
            handshake();
        break;
    case Qmmp::Stopped:
        elapsed += m_time->elapsed();
        if (!m_song.metaData().isEmpty()
            && ((elapsed/1000 > 240) || (elapsed/1000 > int(m_song.length()/2)))
            && (m_song.length() > MIN_SONG_LENGTH))
        {
            m_song.setTimeStamp(m_start_ts);
            m_cachedSongs << m_song;
            m_cache->save(m_cachedSongs);
        }
        m_song.clear();
        if (m_cachedSongs.isEmpty())
            break;

        if (isReady() && !m_submitReply)
            submit();
        break;
    case Qmmp::Paused:
        elapsed += m_time->restart();
        qDebug("LibrefmScrobbler: pausing after %d seconds played", elapsed / 1000);
        break;
    default:
        ;
    }
    previousState = state;
}

void LibrefmScrobbler::updateMetaData()
{
    QMap <Qmmp::MetaData, QString> metadata = m_core->metaData();
    if (m_core->state() == Qmmp::Playing
            && !metadata.value(Qmmp::TITLE).isEmpty()      //skip empty tags
            && !metadata.value(Qmmp::ARTIST).isEmpty()
            && m_core->totalTime() >= 0)                   //skip stream
    {
        metadata[Qmmp::ARTIST].replace("%", QUrl::toPercentEncoding("%")); //replace special symbols
        metadata[Qmmp::ALBUM].replace("%", QUrl::toPercentEncoding("%"));
        metadata[Qmmp::TITLE].replace("%", QUrl::toPercentEncoding("%"));
        metadata[Qmmp::ARTIST].replace("&", QUrl::toPercentEncoding("&"));
        metadata[Qmmp::ALBUM].replace("&", QUrl::toPercentEncoding("&"));
        metadata[Qmmp::TITLE].replace("&", QUrl::toPercentEncoding("&"));
        metadata[Qmmp::ARTIST].replace("=", QUrl::toPercentEncoding("="));
        metadata[Qmmp::ALBUM].replace("=", QUrl::toPercentEncoding("="));
        metadata[Qmmp::TITLE].replace("=", QUrl::toPercentEncoding("="));
        m_song = SongInfo(metadata, m_core->totalTime()/1000);
        if (isReady() && !m_notificationReply && !m_submitReply)
            sendNotification(m_song);
    }
}

void LibrefmScrobbler::processResponse(QNetworkReply *reply)
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
        m_handshakeReply = 0;
        m_submitUrl.clear();
        m_session.clear();
        m_nowPlayingUrl.clear();
        m_failure_count = 0;
        QStringList strlist = data.split("\n");
        if (!strlist[0].contains("OK") || strlist.size() < 4)
        {
            qWarning("LibrefmScrobbler: handshake phase error");
            m_disabled = true;
            if(strlist[0].contains("BANNED"))
                qWarning("LibrefmScrobbler: client has been banned");
            else if(strlist[0].contains("BADAUTH"))
                qWarning("LibrefmScrobbler: incorrect user/password");
            else if(strlist[0].contains("BADTIME"))
                qWarning("LibrefmScrobbler: incorrect system time");
            else
            {
                qWarning("LibrefmScrobbler: service error: %s", qPrintable(strlist[0]));
                m_disabled = false;
                m_handshake_count++;
                QTimer::singleShot (60000 * qMin(m_handshake_count^2, 120) , this, SLOT(handshake()));
                qWarning("LibrefmScrobbler: waiting %d minutes...", qMin(m_handshake_count^2, 120));
            }
        }
        else if (strlist.size() > 3) //process handshake response
        {
            qDebug("LibrefmScrobbler: reading handshake response");
            qDebug("LibrefmScrobbler: Session ID: %s", qPrintable(strlist[1]));
            qDebug("LibrefmScrobbler: Now-Playing URL: %s", qPrintable(strlist[2]));
            qDebug("LibrefmScrobbler: Submission URL: %s", qPrintable(strlist[3]));
            m_submitUrl = strlist[3];
            m_nowPlayingUrl = strlist[2];
            m_session = strlist[1];
            updateMetaData(); //send now-playing notification for already playing song
            if (!m_cachedSongs.isEmpty()) //submit recent songs
                submit();
        }
    }
    else if (reply == m_submitReply)
    {
        m_submitReply = 0;
        if (!data.startsWith("OK"))
        {
            qWarning("LibrefmScrobbler: submit error");
            if(data.contains("BADSESSION"))
            {
                qWarning("LibrefmScrobbler: invalid session ID");
                qWarning("LibrefmScrobbler: performing re-handshake");
                handshake();
            }
            else
            {
                qWarning("LibrefmScrobbler: %s", qPrintable(data));
                m_failure_count ++;
            }
        }
        else
        {
            qDebug("LibrefmScrobbler: submited %d song(s)", m_submitedSongs);
            while (m_submitedSongs)
            {
                m_submitedSongs--;
                m_cachedSongs.removeFirst ();
            }
            if (!m_cachedSongs.isEmpty()) //submit remaining songs
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
            qWarning("LibrefmScrobbler: notification error");
            if(data.contains("BADSESSION"))
            {
                qWarning("LibrefmScrobbler: invalid session ID");
                qWarning("LibrefmScrobbler: performing re-handshake");
                handshake();
            }
            else
            {
                qWarning("LibrefmScrobbler: %s",qPrintable(data));
                m_failure_count ++;
            }
        }
        else
            qDebug("LibrefmScrobbler: Now-Playing notification done");
    }
    if(m_failure_count >= 3)
    {
        qWarning("LibrefmScrobbler: performing re-handshake");
        handshake();
    }
    reply->deleteLater();
}

void LibrefmScrobbler::setupProxy()
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

void LibrefmScrobbler::handshake()
{
    if (m_disabled)
        return;
    qDebug("LibrefmScrobbler: handshake request");
    uint ts = QDateTime::currentDateTime().toTime_t();
    qDebug("LibrefmScrobbler: current time stamp %d",ts);
    QString auth_tmp = QString("%1%2").arg(m_passw).arg(ts);
    QByteArray auth = QCryptographicHash::hash(auth_tmp.toAscii (), QCryptographicHash::Md5);
    auth = auth.toHex();
    QUrl url(QString(SCROBBLER_LIBREFM_URL) + "?");
    url.addQueryItem("hs", "true");
    url.addQueryItem("p", PROTOCOL_VER);
    url.addQueryItem("c", CLIENT_ID);
    url.addQueryItem("v", CLIENT_VER);
    url.addQueryItem("u", m_login);
    url.addQueryItem("t", QString::number(ts));
    url.addQueryItem("a", QString(auth));
    url.setPort(80);
    qDebug("LibrefmScrobbler: request url: %s",qPrintable(url.toString()));
    QNetworkRequest request(url);
    request.setRawHeader("User-Agent",  m_ua);
    request.setRawHeader("Host",url.host().toAscii());
    request.setRawHeader("Accept", "*/*");
    m_handshakeReply = m_http->get(request);
}

void LibrefmScrobbler::submit()
{
    qDebug("LibrefmScrobbler: submit request");
    if (m_cachedSongs.isEmpty())
        return;
    m_submitedSongs = qMin(m_cachedSongs.size(),25);
    QString body = QString("s=%1").arg(m_session);
    for (int i = 0; i < m_submitedSongs; ++i)
    {
        SongInfo info = m_cachedSongs[i];
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
    request.setRawHeader("Content-Type", "application/x-www-form-urlencoded");
    request.setHeader(QNetworkRequest::ContentLengthHeader,
                      QUrl::toPercentEncoding(body,":/[]&=%").size());
    m_submitReply = m_http->post(request, QUrl::toPercentEncoding(body,":/[]&=%"));
}

void LibrefmScrobbler::sendNotification(const SongInfo &info)
{
    qDebug("LibrefmScrobbler: sending notification");
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
    request.setRawHeader("Content-Type", "application/x-www-form-urlencoded");
    request.setHeader(QNetworkRequest::ContentLengthHeader,
                      QUrl::toPercentEncoding(body,":/[]&=%").size());
    m_notificationReply = m_http->post(request, QUrl::toPercentEncoding(body,":/[]&=%"));
}

bool LibrefmScrobbler::isReady()
{
    return !m_submitUrl.isEmpty() && !m_session.isEmpty();
}
