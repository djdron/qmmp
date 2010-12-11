/***************************************************************************
 *   Copyright (C) 2010 by Ilya Kotov                                      *
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
#ifndef SCROBBLER2_H
#define SCROBBLER2_H

#include <QMap>
#include <qmmp/qmmp.h>
#include "scrobbler.h"

class QNetworkAccessManager;
class QNetworkReply;
class QTime;
class SoundCore;

/**
    @author Ilya Kotov <forkotov02@hotmail.ru>
*/

class Scrobbler2 : public QObject
{
    Q_OBJECT
public:
    Scrobbler2(const QString &url, const QString &name, QObject *parent = 0);
    ~Scrobbler2();

private slots:
    void setState(Qmmp::State state);
    void updateMetaData();
    void processResponse(QNetworkReply *reply);
    void setupProxy();
    void getToken();
    void getSession();
    void submit();

private:
    void sendNotification(const SongInfo &info);
    void syncCache();
    uint m_start_ts;
    SongInfo m_song;
    QNetworkAccessManager *m_http;
    Qmmp::State m_state;
    SoundCore *m_core;
    QList <SongInfo> m_songCache;
    QByteArray m_ua;
    QTime* m_time;
    int m_submitedSongs;
    QNetworkReply *m_getTokenReply, *m_getSessionReply;
    QNetworkReply *m_submitReply, *m_notificationReply;
    QString m_server, m_name;
    QString m_token, m_session;
};

#endif
