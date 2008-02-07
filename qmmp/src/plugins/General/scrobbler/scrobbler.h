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
#ifndef SCROBBLER_H
#define SCROBBLER_H

#include <QHttpResponseHeader>
#include <qmmpui/general.h>
#include <time.h>

class QHttp;
class QTime;



/**
	@author Ilya Kotov <forkotov02@hotmail.ru>
*/

class Scrobbler : public General
{
Q_OBJECT
public:
    Scrobbler(QObject *parent = 0);

    ~Scrobbler();

    void setState(const uint& state);
    void setSongInfo(const SongInfo &song);

private slots:
    void processResponse(int, bool);
    void processResponseHeader(const QHttpResponseHeader &);
    void readResponse(const QHttpResponseHeader&);

private:
    void handshake();
    void submit();
    bool isReady();
    time_t m_start_ts;
    SongInfo m_song;
    QHttp *m_http;
    uint m_state;
    QString m_login;
    QString m_passw;
    QString m_submitUrl;
    QString m_session;
    QList <time_t> m_timeStamps;
    QList <SongInfo> m_songCache;
    QTime* m_time;
    int m_submitedSongs;
    int m_handshakeid;
    int m_submitid;
    QByteArray m_array;
    bool m_disabled;

};

#endif
