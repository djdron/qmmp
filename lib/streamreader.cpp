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
#include <QUrl>

#include "downloader.h"
#include "streamreader.h"

StreamReader::StreamReader(const QString &name, QObject *parent)
        : QIODevice(parent)
{
    m_downloader = new Downloader(this, name);
}

StreamReader::~StreamReader()
{
    m_downloader->abort();
}

bool StreamReader::atEnd () const
{
    return FALSE;
}

qint64 StreamReader::bytesAvailable () const
{
    return m_downloader->bytesAvailable ();
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
    //m_httpRequestAborted = TRUE;
    //m_http->close();
    m_downloader->abort();
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
    //if (m_httpRequestAborted)
    //return TRUE;
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
    return m_downloader->read (data, maxlen);
}

qint64 StreamReader::writeData(const char*, qint64)
{
    return 0;
}

void StreamReader::downloadFile()
{
    m_httpRequestAborted = FALSE;
    //qDebug("StreamReader: connecting...");
    //m_httpGetId = m_http->get(m_url.path(), 0);
    qDebug("StreamReader: buffering...");
    /*while (m_http->bytesAvailable () < BUFFER_SIZE*0.5 && !m_httpRequestAborted)
    {
        qApp->processEvents();
    }*/
    m_downloader->start();
    while (m_downloader->bytesAvailable () < BUFFER_SIZE*0.5 &&
           m_downloader->isRunning())
    {
        qApp->processEvents();
        //qDebug("StreamReader: bytes: %d",m_downloader->bytesAvailable ());
        //sleep(1);
    }
    qDebug("StreamReader: ready");
}

void StreamReader::cancelDownload()
{
    m_httpRequestAborted = true;
    //&& !m_httpRequestAborted->abort();
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
        //qDebug(qPrintable(QString("StreamReader: %1").arg(m_http->errorString())));
        m_httpRequestAborted = TRUE;
    }
    else
    {
        qDebug("StreamReader: end of file");
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
    /*if (m_http->bytesAvailable () > BUFFER_SIZE && !m_httpRequestAborted)
    {
        while (m_http->bytesAvailable () > BUFFER_SIZE*0.75 && !m_httpRequestAborted)
        {
            qDebug("StreamReader: skipping data...");
            char *data = new char[BUFFER_SIZE/20];
            m_http->read (data, BUFFER_SIZE/20);
            qApp->processEvents();
            delete data;
        }
    }*/
}

const QString &StreamReader::contentType()
{
    //Downloader* dw = new Downloader(this, "http://127.0.0.1:8000/");
    m_downloader->start();
    while (m_contentType.isEmpty() && m_downloader->isRunning())
    {
        m_downloader->mutex()->lock ();
        m_contentType = m_downloader->contentType();
        m_downloader->mutex()->unlock();
        qApp->processEvents();
    }
    //m_contentType = dw->contentType();
    m_downloader->abort();
    qDebug("StreamReader: content type: %s", qPrintable(m_contentType));
    return m_contentType;
}
