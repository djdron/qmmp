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
#ifndef SCROBBLER_H
#define SCROBBLER_H

#include <QMap>
#include <qmmp/qmmp.h>

class QNetworkAccessManager;
class QNetworkReply;
class QTime;
class SoundCore;

/**
    @author Ilya Kotov <forkotov02@hotmail.ru>
*/

class SongInfo
{
public:
    SongInfo();
    SongInfo(const QMap <Qmmp::MetaData, QString> metadata, qint64 length = 0);
    SongInfo(const SongInfo &other);

    ~SongInfo();

    void operator=(const SongInfo &info);
    bool operator==(const SongInfo &info);
    bool operator!=(const SongInfo &info);

    void setMetaData(const QMap <Qmmp::MetaData, QString> metadata);
    void setMetaData(Qmmp::MetaData key, const QString &value);
    void setLength(qint64 l);
    const QMap <Qmmp::MetaData, QString> metaData() const;
    const QString metaData(Qmmp::MetaData) const;
    qint64 length () const;
    void clear();
    void setTimeStamp(uint ts);
    uint timeStamp() const;

private:
    QMap <Qmmp::MetaData, QString> m_metadata;
    qint64 m_length;
    uint m_start_ts;

};

class Scrobbler : public QObject
{
    Q_OBJECT
public:
    Scrobbler(const QString &url,
              const QString &login,
              const QString &passw,
              const QString &name,
              QObject *parent = 0);

    ~Scrobbler();

private slots:
    void setState(Qmmp::State state);
    void updateMetaData();
    void processResponse(QNetworkReply *reply);

private:
    void handshake();
    void submit();
    void sendNotification(const SongInfo &info);
    bool isReady();
    uint m_start_ts;
    SongInfo m_song;
    QNetworkAccessManager *m_http;
    Qmmp::State m_state;
    SoundCore *m_core;
    QString m_login;
    QString m_passw;
    QString m_submitUrl;
    QString m_nowPlayingUrl;
    QString m_session;
    QList <SongInfo> m_songCache;
    QByteArray m_ua;
    QTime* m_time;
    int m_submitedSongs;
    QNetworkReply *m_handshakeReply;
    QNetworkReply *m_submitReply;
    QNetworkReply *m_notificationReply;
    bool m_disabled;
    QString m_server, m_name;
};

#endif
