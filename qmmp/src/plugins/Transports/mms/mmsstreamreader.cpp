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
#include <QSettings>
#include <stdint.h>
#include <stdlib.h>
#include <qmmp/qmmp.h>
#include <qmmp/statehandler.h>
#include "mmsstreamreader.h"

MMSStreamReader::MMSStreamReader(const QString &url, QObject *parent)
        : QIODevice(parent)
{
    qDebug("%s", Q_FUNC_INFO);
    m_url = url;
    m_handle = 0;
    m_aborted = false;
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    m_prebuf_size = settings.value("MMS/buffer_size",128).toInt() * 1024;
    m_buffer_size = m_prebuf_size;
    m_buffer = (char *)malloc(m_buffer_size);
    m_ready = false;
    m_buffer_at = 0;
    m_thread = new DownloadThread(this);
}

MMSStreamReader::~MMSStreamReader()
{
    qDebug("%s", Q_FUNC_INFO);
    abort();
    free(m_buffer);
    m_buffer = 0;
    m_buffer_at = 0;
    m_buffer_size = 0;
}

bool MMSStreamReader::atEnd () const
{
    qDebug("%s", Q_FUNC_INFO);
    return false;
}

qint64 MMSStreamReader::bytesToWrite () const
{
    qDebug("%s", Q_FUNC_INFO);
    return -1;
}

void MMSStreamReader::close ()
{
     qDebug("%s", Q_FUNC_INFO);
    abort();
    QIODevice::close();
}

bool MMSStreamReader::isSequential () const
{
     qDebug("%s", Q_FUNC_INFO);
    return true;
}

bool MMSStreamReader::open (OpenMode mode)
{
     qDebug("%s", Q_FUNC_INFO);
    if (mode != QIODevice::ReadOnly)
        return false;
    QIODevice::open(mode);
    return m_ready;
}

bool MMSStreamReader::seek (qint64 pos)
{
     qDebug("%s", Q_FUNC_INFO);
    Q_UNUSED(pos);
    return false;
}

qint64 MMSStreamReader::writeData(const char*, qint64)
{
     qDebug("%s", Q_FUNC_INFO);
    return -1;
}

qint64 MMSStreamReader::readData(char* data, qint64 maxlen)
{
     qDebug("%s", Q_FUNC_INFO);
    m_mutex.lock();
    qint64 len = qMin<qint64>(m_buffer_at, maxlen);
    memmove(data, m_buffer, len);
    m_buffer_at -= len;
    memmove(m_buffer, m_buffer + len, m_buffer_at);
    m_mutex.unlock();
    qDebug("read=%lld", len);
    return len;
}

void MMSStreamReader::downloadFile()
{
     qDebug("%s", Q_FUNC_INFO);
     m_thread->start();
}

QMutex *MMSStreamReader::mutex()
{
    return &m_mutex;
}

void MMSStreamReader::abort()
{
     qDebug("%s", Q_FUNC_INFO);
    m_mutex.lock();
    if (m_aborted)
    {
        m_mutex.unlock();
        return;
    }
    m_aborted = true;
    m_mutex.unlock();
    m_thread->wait();
    m_ready = false;
    if (m_handle)
        mmsx_close(m_handle);
    m_handle = 0;
}

qint64 MMSStreamReader::bytesAvailable() const
{
    qDebug("+++%lld",QIODevice::bytesAvailable() + m_buffer_at);
    return QIODevice::bytesAvailable() + m_buffer_at;
}

void MMSStreamReader::run()
{
     qDebug("%s", Q_FUNC_INFO);
    m_handle = mmsx_connect (0, 0, m_url.toLocal8Bit().constData(), 128 * 1024);
    if(!m_handle)
    {
        qWarning("MMSStreamReader: connection failed");
        return;
    }
    m_mutex.lock();
    if(m_aborted)
    {
        m_mutex.unlock();
        qDebug("MMSStreamReader: aborted");
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
        len = mmsx_read (0, m_handle, m_buffer + m_buffer_at, to_read);
        if(len < 0)
        {
            qWarning("MMSStreamReader: error: %s", strerror(len));
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

void MMSStreamReader::checkBuffer()
{
     qDebug("%s", Q_FUNC_INFO);
    if (m_buffer_at > m_prebuf_size && !m_ready)
    {
        m_ready = true;
        qDebug("MMSStreamReader: ready");
        QMap<Qmmp::MetaData, QString> metaData;
        metaData.insert(Qmmp::URL, m_url);
        StateHandler::instance()->dispatch(metaData);
        emit ready();
    }
    else if (!m_ready)
    {
        StateHandler::instance()->dispatchBuffer(100 * m_buffer_at / m_buffer_size);
        qApp->processEvents();
    }
}

DownloadThread::DownloadThread(MMSStreamReader *parent) : QThread(parent)
{
    m_parent = parent;
}

DownloadThread::~DownloadThread (){}

void DownloadThread::run()
{
    m_parent->run();
}
