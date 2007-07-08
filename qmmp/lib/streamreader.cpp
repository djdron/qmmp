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
#include <QApplication>
#include <QHttp>
#include <QUrl>

#include "streamreader.h"

StreamReader::StreamReader(const QString &name, QObject *parent)
        : QIODevice(parent)
{

    m_name =name;
    qDebug(qPrintable(name));
    http = new QHttp(this);

    connect(http, SIGNAL(requestFinished(int, bool)),
            this, SLOT(httpRequestFinished(int, bool)));
    connect(http, SIGNAL(dataReadProgress(int, int)),
            this, SLOT(updateDataReadProgress(int, int)));
    connect(http, SIGNAL(responseHeaderReceived(const QHttpResponseHeader &)),
            this, SLOT(readResponseHeader(const QHttpResponseHeader &)));
}
StreamReader::~StreamReader()
{
    http->abort();
}


bool StreamReader::atEnd () const
{
    return FALSE;
}

qint64 StreamReader::bytesAvailable () const
{
    return http->bytesAvailable ();
}

qint64 StreamReader::bytesToWrite () const
{
    return -1;
}

bool StreamReader::canReadLine () const
{
    return FALSE;
}

void StreamReader::close ()
{
    httpRequestAborted = TRUE;
    http->close();
}

bool StreamReader::isSequential () const
{
    return TRUE;
}

bool StreamReader::open ( OpenMode mode )
{
    if (mode != QIODevice::ReadOnly)
        return FALSE;
    downloadFile();
    if (httpRequestAborted)
        return FALSE;
    setOpenMode(QIODevice::ReadOnly);
    return TRUE;
}

qint64 StreamReader::pos () const
{
    return 0;
}

bool StreamReader::reset ()
{
    return TRUE;
}

bool StreamReader::seek ( qint64 pos )
{
    return FALSE;
}

qint64 StreamReader::size () const
{
    return bytesAvailable ();
}

bool StreamReader::waitForBytesWritten ( int msecs )
{
    usleep(msecs*1000);
    return TRUE;
}

bool StreamReader::waitForReadyRead ( int msecs )
{
    usleep(msecs*1000);
    return TRUE;
}

qint64 StreamReader::readData(char* data, qint64 maxlen)
{
    return http->read (data, maxlen);
}

qint64 StreamReader::writeData(const char*, qint64)
{
    return 0;
}

void StreamReader::downloadFile()
{
    QUrl url(m_name);

    http->setHost(url.host(), url.port() != -1 ? url.port() : 80);
    if (!url.userName().isEmpty())
        http->setUser(url.userName(), url.password());

    httpRequestAborted = false;
    f = false;
    qDebug("StreamReader: connecting...");
    httpGetId = http->get(url.path(), 0);
    qDebug("StreamReader: buffering...");
    while (http->bytesAvailable () < BUFFER_SIZE*0.5 && !httpRequestAborted)
    {
        qApp->processEvents();
    }
    qDebug("StreamReader: ready");
}

void StreamReader::cancelDownload()
{
    httpRequestAborted = true;
    http->abort();
}

void StreamReader::httpRequestFinished(int requestId, bool error)
{
    if (httpRequestAborted)
    {
        return;
    }

    if (requestId != httpGetId)
        return;

    if (error)
    {
        qDebug(qPrintable(QString("StreamReader: %1").arg(http->errorString())));
    }
    else
    {
        qDebug("StreamReader: end of file");
    }

}

void StreamReader::readResponseHeader(const QHttpResponseHeader &responseHeader)
{
    if (responseHeader.statusCode() != 200)
    {
        qDebug(qPrintable(QString("Download failed: %1.") .arg(responseHeader.reasonPhrase())));
        httpRequestAborted = true;
        http->abort();
        return;
    }
}

void StreamReader::updateDataReadProgress(int bytesRead, int totalBytes)
{
    fillBuffer();
    //qDebug("%d, %d, %d",bytesRead,totalBytes,http->bytesAvailable ());
}

void StreamReader::fillBuffer()
{
    if (http->bytesAvailable () > BUFFER_SIZE && !httpRequestAborted)
    {
        while (http->bytesAvailable () > BUFFER_SIZE*0.75 && !httpRequestAborted)
        {
            qDebug("StreamReader: skipping data...");
            char *data = new char[BUFFER_SIZE/20];
            http->read (data, BUFFER_SIZE/20);
            qApp->processEvents();
            delete data;
        }
    }
}
