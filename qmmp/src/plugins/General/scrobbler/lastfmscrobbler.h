/***************************************************************************
 *   Copyright (C) 2010-2013 by Ilya Kotov                                 *
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
#ifndef LASTFMSCROBBLER_H
#define LASTFMSCROBBLER_H

#include <QMap>
#include <QObject>
#include <qmmp/qmmp.h>
#include "scrobblercache.h"

class QNetworkAccessManager;
class QNetworkReply;
class QIODevice;
class QTime;
class SoundCore;


/**
    @author Ilya Kotov <forkotov02@hotmail.ru>
*/
class LastfmResponse
{
public:
    QString status;
    QString token;
    QString code;
    QString error;
    QString key;
    QString name;
    QString subscriber;

    void parse(QIODevice *device);
};

/**
    @author Ilya Kotov <forkotov02@hotmail.ru>
*/
class LastfmScrobbler : public QObject
{
    Q_OBJECT
public:
    LastfmScrobbler(QObject *parent = 0);
    ~LastfmScrobbler();

private slots:
    void setState(Qmmp::State state);
    void updateMetaData();
    void processResponse(QNetworkReply *reply);
    void setupProxy();
    void submit();

private:
    enum { MIN_SONG_LENGTH = 30 };

    void sendNotification(const SongInfo &info);
    uint m_start_ts;
    SongInfo m_song;
    Qmmp::State m_state;
    QList <SongInfo> m_cachedSongs;
    QByteArray m_ua;
    int m_submitedSongs;
    QString m_session;
    QNetworkAccessManager *m_http;
    SoundCore *m_core;
    QNetworkReply *m_submitReply, *m_notificationReply;
    QTime *m_time;
    ScrobblerCache *m_cache;
};

/**
    @author Ilya Kotov <forkotov02@hotmail.ru>
*/
class LastfmAuth : public QObject
{
    Q_OBJECT
public:
    explicit LastfmAuth(QObject *parent = 0);
    void getToken();
    void getSession();
    QString session() const;

    enum ErrorType
    {
        NO_ERROR = 0,
        NETWORK_ERROR,
        LASTFM_ERROR
    };

signals:
    void tokenRequestFinished(int error);
    void sessionRequestFinished(int error);

private slots:
    void processResponse(QNetworkReply *reply);

private:
    QString m_token, m_session;
    QByteArray m_ua;
    QNetworkAccessManager *m_http;
    QNetworkReply *m_getTokenReply, *m_getSessionReply;
};

#endif
