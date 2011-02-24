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
    connect(m_downloader, SIGNAL(ready()), SIGNAL(ready()));
}

StreamReader::~StreamReader()
{
    m_downloader->abort();
    qDebug("%s", Q_FUNC_INFO);
}

bool StreamReader::atEnd () const
{
    return false;
}

qint64 StreamReader::bytesAvailable () const
{
    return m_downloader->bytesAvailable ();
}

qint64 StreamReader::bytesToWrite () const
{
    return -1;
}

void StreamReader::close ()
{
    m_downloader->abort();
    QIODevice::close();
}

bool StreamReader::isSequential () const
{
    return true;
}

bool StreamReader::open (OpenMode mode)
{
    if (mode != QIODevice::ReadOnly)
        return false;
    QIODevice::open(mode);
    return m_downloader->isReady();
}

bool StreamReader::seek (qint64 pos)
{
    Q_UNUSED(pos);
    return false;
}

qint64 StreamReader::readData(char* data, qint64 maxlen)
{
    return m_downloader->read (data, maxlen);
}

qint64 StreamReader::writeData(const char*, qint64)
{
    return -1;
}

void StreamReader::downloadFile()
{
    m_downloader->start();
}

QString StreamReader::contentType()
{
    m_downloader->mutex()->lock ();
    m_contentType = m_downloader->contentType();
    m_downloader->mutex()->unlock();
    qDebug("StreamReader: content type: %s", qPrintable(m_contentType));
    return m_contentType;
}
