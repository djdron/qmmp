/***************************************************************************
 *   Copyright (C) 2008-2009 by Ilya Kotov                                 *
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

#include <QHttpResponseHeader>
#include <QMap>
#include <qmmpui/general.h>
#include <qmmp/qmmp.h>
#include <time.h>

class QHttp;
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
    void setTimeStamp(time_t ts);
    time_t timeStamp() const;

private:
    QMap <Qmmp::MetaData, QString> m_metadata;
    qint64 m_length;
    time_t m_start_ts;

};

class Scrobbler : public General
{
    Q_OBJECT
public:
    Scrobbler(QObject *parent = 0);

    ~Scrobbler();

private slots:
    void setState(Qmmp::State state);
    void updateMetaData();
    void processResponse(int, bool);
    void readResponse(const QHttpResponseHeader&);

private:
    void handshake();
    void submit();
    void sendNotification(const SongInfo &info);
    bool isReady();
    time_t m_start_ts;
    SongInfo m_song;
    QHttp *m_http;
    Qmmp::State m_state;
    SoundCore *m_core;
    QString m_login;
    QString m_passw;
    QString m_submitUrl;
    QString m_nowPlayingUrl;
    QString m_session;
    //QList <time_t> m_timeStamps;
    QList <SongInfo> m_songCache;
    QTime* m_time;
    int m_submitedSongs;
    int m_handshakeid;
    int m_submitid;
    int m_notificationid;
    QByteArray m_array;
    bool m_disabled;

};

#endif
