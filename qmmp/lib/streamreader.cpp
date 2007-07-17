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
    qDebug(qPrintable(name));
    m_http = new QHttp(this);

    m_pos = 0;

    connect(m_http, SIGNAL(requestFinished(int, bool)),
            this, SLOT(httpRequestFinished(int, bool)));
    connect(m_http, SIGNAL(dataReadProgress(int, int)),
            this, SLOT(updateDataReadProgress(int, int)));
    connect(m_http, SIGNAL(responseHeaderReceived(const QHttpResponseHeader &)),
            this, SLOT(readResponseHeader(const QHttpResponseHeader &)));

    m_url = QUrl(name);

    m_http->setHost(m_url.host(), m_url.port() != -1 ? m_url.port() : 80);
    if (!m_url.userName().isEmpty())
        m_http->setUser(m_url.userName(), m_url.password());
}

StreamReader::~StreamReader()
{
    m_http->abort();
}

bool StreamReader::atEnd () const
{
    return FALSE;
}

qint64 StreamReader::bytesAvailable () const
{
    return m_http->bytesAvailable ();
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
    m_httpRequestAborted = TRUE;
    m_http->close();
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
    if (m_httpRequestAborted)
        return FALSE;
    setOpenMode(QIODevice::ReadOnly);
    return TRUE;
}

qint64 StreamReader::pos () const
{
    return m_pos;
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
    return m_http->read (data, maxlen);
}

qint64 StreamReader::writeData(const char*, qint64)
{
    return 0;
}

void StreamReader::downloadFile()
{
    m_httpRequestAborted = FALSE;
    qDebug("StreamReader: connecting...");
    m_httpGetId = m_http->get(m_url.path(), 0);
    qDebug("StreamReader: buffering...");
    while (m_http->bytesAvailable () < BUFFER_SIZE*0.5 && !m_httpRequestAborted)
    {
        qApp->processEvents();
    }
    qDebug("StreamReader: ready");
}

void StreamReader::cancelDownload()
{
    m_httpRequestAborted = true;
    m_http->abort();
}

void StreamReader::httpRequestFinished(int requestId, bool error)
{
    if (m_httpRequestAborted)
    {
        return;
    }

    if (requestId != m_httpGetId)
        return;

    if (error)
    {
        qDebug(qPrintable(QString("StreamReader: %1").arg(m_http->errorString())));
    }
    else
    {
        qDebug("StreamReader: end of file");
    }

}

void StreamReader::readResponseHeader(const QHttpResponseHeader &responseHeader)
{
    m_contentType = responseHeader.contentType ();
    if (responseHeader.statusCode() != 200)
    {
        qDebug(qPrintable(QString("Download failed: %1.") .arg(responseHeader.reasonPhrase())));
        m_httpRequestAborted = true;
        m_http->abort();
        return;
    }
}

void StreamReader::updateDataReadProgress(int bytesRead, int totalBytes)
{
    m_pos = bytesRead;
    m_size = totalBytes;
    fillBuffer();
}

void StreamReader::fillBuffer()
{
    if (m_http->bytesAvailable () > BUFFER_SIZE && !m_httpRequestAborted)
    {
        while (m_http->bytesAvailable () > BUFFER_SIZE*0.75 && !m_httpRequestAborted)
        {
            qDebug("StreamReader: skipping data...");
            char *data = new char[BUFFER_SIZE/20];
            m_http->read (data, BUFFER_SIZE/20);
            qApp->processEvents();
            delete data;
        }
    }
}

const QString &StreamReader::contentType()
{
    if(!m_contentType.isEmpty())
        return m_contentType;
    m_httpRequestAborted = FALSE;
    qDebug("StreamReader: reading content type");
    m_httpGetId = m_http->get(m_url.path(), 0);
    while (m_contentType.isEmpty() && !m_httpRequestAborted)
    {
        qApp->processEvents();
    }
    qDebug("StreamReader: content type:  %s", qPrintable(m_contentType));
    //m_http->abort();
    m_http->close();
    return m_contentType;
}
