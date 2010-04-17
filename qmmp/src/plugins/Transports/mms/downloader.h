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
#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QThread>
#include <QMutex>
#include <QByteArray>
#include <QMap>
#define this var
#include <libmms/mmsx.h>
#undef this



/*! @internal
 *  @author Ilya Kotov <forkotov02@hotmail.ru>
 */
class Downloader : public QThread
{
    Q_OBJECT
public:
    Downloader(QObject *parent, const QString &url);

    ~Downloader();

    qint64 read(char* data, qint64 maxlen);
    QMutex *mutex();
    QString contentType();
    void abort();
    qint64 bytesAvailable();
    void checkBuffer();
    bool isReady();

signals:
    void readyRead();

private:
    QMutex m_mutex;
    QString m_url;
    mmsx_t *m_handle;
    bool m_aborted;
    qint64 m_buffer_size, m_prebuf_size;
    char *m_buffer;
    qint64 m_buffer_at;
    bool m_ready;

protected:
    void run();

};

#endif
