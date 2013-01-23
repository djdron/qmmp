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
#ifndef LIBREFMSCROBBLER_H
#define LIBREFMSCROBBLER_H

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
class LibrefmScrobbler : public QObject
{
    Q_OBJECT
public:
    LibrefmScrobbler(const QString &login, const QString &passw, QObject *parent = 0);

    ~LibrefmScrobbler();

private slots:
    void setState(Qmmp::State state);
    void updateMetaData();
    void processResponse(QNetworkReply *reply);
    void setupProxy();
    void handshake();

private:
    enum { MIN_SONG_LENGTH = 30 };

    void submit();
    void sendNotification(const SongInfo &info);
    bool isReady();

    uint m_start_ts;
    SongInfo m_song;
    QString m_login;
    QString m_passw;
    QString m_submitUrl;
    QString m_nowPlayingUrl;
    QString m_session;
    QList <SongInfo> m_cachedSongs;
    QByteArray m_ua;
    int m_submitedSongs;
    int m_failure_count;
    int m_handshake_count;
    bool m_disabled;
    QNetworkAccessManager *m_http;
    SoundCore *m_core;
    QNetworkReply *m_handshakeReply;
    QNetworkReply *m_submitReply;
    QNetworkReply *m_notificationReply;
    QTime* m_time;
    ScrobblerCache *m_cache;

};

#endif
