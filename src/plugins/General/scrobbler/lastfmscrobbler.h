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
class QTime;
class SoundCore;

/**
    @author Ilya Kotov <forkotov02@hotmail.ru>
*/
struct LastfmResponse
{
    QString status;
    QString token;
    QString code;
    QString error;
    QString key;
    QString name;
    QString subscriber;
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
    void getToken();
    void getSession();
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
    QString m_token, m_session;
    QNetworkAccessManager *m_http;
    SoundCore *m_core;
    QNetworkReply *m_getTokenReply, *m_getSessionReply;
    QNetworkReply *m_submitReply, *m_notificationReply;
    QTime *m_time;
    ScrobblerCache *m_cache;
};

#endif
