/***************************************************************************
 *   Copyright (C) 2006 by Ilya Kotov                                      *
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

#include <curl/curl.h>

/**
	@author Ilya Kotov <forkotov02@hotmail.ru>
*/

struct Stream
{
    char *buf;
    int buf_fill;
    QString content_type;
    bool aborted;
    QMap <QString, QString> header;
    bool icy_meta_data;
    int icy_metaint;
};

class Downloader : public QThread
{
Q_OBJECT
public:
    Downloader(QObject *parent, const QString &url);

    ~Downloader();

    qint64 read(char* data, qint64 maxlen);
    Stream *stream();
    QMutex *mutex();
    QString contentType();
    void abort();
    int bytesAvailable();

private:
    qint64 readBuffer(char* data, qint64 maxlen);
    void readICYMetaData();
    CURL *m_handle;
    QMutex m_mutex;
    Stream m_stream;
    QString m_url;
    int m_metacount;

protected:
    void run();

};

#endif
