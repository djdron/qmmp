/***************************************************************************
 *   Copyright (C) 2006-2008 by Ilya Kotov                                 *
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
    connect(m_downloader, SIGNAL(titleChanged()),SLOT(updateTitle()));
    connect(m_downloader, SIGNAL(readyRead()), SIGNAL(readyRead()));
    connect(m_downloader, SIGNAL(bufferingProgress(int)), SIGNAL(bufferingProgress(int)));
}

StreamReader::~StreamReader()
{
    m_downloader->abort();
    qDebug("StreamReader::~StreamReader()");
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
    //downloadFile();
    setOpenMode(QIODevice::ReadOnly);
    if (m_downloader->isRunning() && m_downloader->isReady())
        return TRUE;
    else
        return FALSE;
}

bool StreamReader::reset ()
{
    QIODevice::reset();
    return TRUE;
}

bool StreamReader::seek ( qint64 pos )
{
    QIODevice::seek(pos);
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
    m_downloader->start();
}

void StreamReader::updateTitle()
{
    emit titleChanged(m_downloader->title());
}

const QString &StreamReader::contentType()
{
    m_downloader->mutex()->lock ();
    m_contentType = m_downloader->contentType();
    m_downloader->mutex()->unlock();
    qApp->processEvents();
    qDebug("StreamReader: content type: %s", qPrintable(m_contentType));
    return m_contentType;
}
