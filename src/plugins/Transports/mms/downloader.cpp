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

#include <QApplication>
#include <QStringList>
#include <QDir>
#include <QMap>
#include <QSettings>
#include <QTextCodec>
#include <stdint.h>
#include <stdlib.h>
#include <qmmp/qmmpsettings.h>
#include <qmmp/qmmp.h>
#include <qmmp/statehandler.h>
#include "downloader.h"

Downloader::Downloader(QObject *parent, const QString &url)
        : QThread(parent)
{
    m_url = url;
    m_handle = 0;
    m_aborted = false;
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    m_prebuf_size = settings.value("MMS/buffer_size",128).toInt() * 1024;
    m_buffer_size = m_prebuf_size;
    m_buffer = (char *)malloc(m_buffer_size);
    m_ready = false;
    m_buffer_at = 0;
}


Downloader::~Downloader()
{
    abort();
    free(m_buffer);
    m_buffer = 0;
    m_buffer_at = 0;
    m_buffer_size = 0;
}

qint64 Downloader::read(char* data, qint64 maxlen)
{
    m_mutex.lock();
    qint64 len = qMin<qint64>(m_buffer_at, maxlen);
    memmove(data, m_buffer, len);
    m_buffer_at -= len;
    memmove(m_buffer, m_buffer + len, m_buffer_at);
    m_mutex.unlock();
    return len;
}

QMutex *Downloader::mutex()
{
    return &m_mutex;
}

QString Downloader::contentType()
{
    return "audio/x-ms-wma";
}

void Downloader::abort()
{
    m_mutex.lock();
    if (m_aborted)
    {
        m_mutex.unlock();
        return;
    }
    m_aborted = true;
    m_ready = false;
    m_mutex.unlock();
    wait();
    if (m_handle)
        mms_close(m_handle);
    m_handle = 0;
}

qint64 Downloader::bytesAvailable()
{
    m_mutex.lock();
    qint64 b = m_buffer_at;
    m_mutex.unlock();
    return b;
}

void Downloader::run()
{
    m_handle = mms_connect (0, 0, m_url.toLocal8Bit().constData(), 128 * 1024);
    if(!m_handle)
    {
        qWarning("Downloader: connection failed");
        return;
    }
    m_mutex.lock();
    if(m_aborted)
    {
        m_mutex.unlock();
        qDebug("Downloader: aborted");
        return;
    }
    m_mutex.unlock();
    qint64 len = 0;
    forever
    {
        m_mutex.lock();
        int to_read = 1024;
        if(m_buffer_at + to_read > m_buffer_size)
        {
            m_buffer_size = m_buffer_at + to_read;
            m_buffer = (char *)realloc(m_buffer, m_buffer_size);
        }
        len = mms_read (0, m_handle, m_buffer + m_buffer_at, to_read);
        if(len < 0)
        {
            qWarning("Downloader: error: %s", strerror(len));
            m_mutex.unlock();
            break;
        }
        m_buffer_at += len;
        if(!m_ready)
            checkBuffer();
        m_mutex.unlock();
        if(m_aborted)
            break;
        usleep(5000);
    }
}

void Downloader::checkBuffer()
{
    if (m_buffer_at > m_prebuf_size && !m_ready)
    {
        m_ready  = true;
        qDebug("Downloader: ready");
        QMap<Qmmp::MetaData, QString> metaData;
        metaData.insert(Qmmp::URL, m_url);
        StateHandler::instance()->dispatch(metaData);
        emit readyRead();
    }
    else if (!m_ready)
    {
        StateHandler::instance()->dispatchBuffer(100 * m_buffer_at / m_buffer_size);
        qApp->processEvents();
    }
}

bool Downloader::isReady()
{
    return m_ready;
}
