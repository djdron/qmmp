/***************************************************************************
 *   Copyright (C) 2006-2010 by Ilya Kotov                                 *
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

//curl callbacks
static size_t curl_write_data(void *data, size_t size, size_t nmemb,
                              void *pointer)
{
    Downloader *dl = (Downloader *)pointer;
    dl->mutex()->lock ();
    size_t buf_start = dl->stream()->buf_fill;
    size_t data_size = size * nmemb;
    dl->stream()->buf_fill += data_size;

    dl->stream()->buf = (char *)realloc (dl->stream()->buf, dl->stream()->buf_fill);
    memcpy (dl->stream()->buf + buf_start, data, data_size);
    dl->mutex()->unlock();
    dl->checkBuffer();
    return data_size;
}

static size_t curl_header(void *data, size_t size, size_t nmemb,
                          void *pointer)
{
    Downloader *dl = (Downloader *)pointer;
    dl->mutex()->lock ();
    size_t data_size = size * nmemb;
    if (data_size < 3)
    {
        dl->mutex()->unlock();
        return data_size;
    }

    //qDebug("header received: %s", (char*) data);
    QString str = QString::fromAscii((char *) data, data_size);
    str = str.trimmed ();
    if (str.left(4).contains("HTTP"))
    {
        qDebug("Downloader: header received");
        //TODO open metadata socket
    }
    else if (str.left(4).contains("ICY"))
    {
        qDebug("Downloader: shoutcast header received");
        //dl->stream()->icy_meta_data = true;
    }
    else
    {
        QString key = str.left(str.indexOf(":")).trimmed().toLower();
        QString value = str.right(str.size() - str.indexOf(":") - 1).trimmed();
        dl->stream()->header.insert(key, value);
        qDebug("Downloader: key=%s, value=%s",qPrintable(key),qPrintable(value));

        if (key == "icy-metaint")
        {
            dl->stream()->icy_metaint = value.toInt();
            dl->stream()->icy_meta_data = true;
        }
    }
    dl->mutex()->unlock();
    return data_size;
}

int curl_progress(void *pointer, double dltotal, double dlnow, double ultotal, double ulnow)
{
    Q_UNUSED(dltotal);
    Q_UNUSED(dlnow);
    Q_UNUSED(ultotal);
    Q_UNUSED(ulnow);
    Downloader *dl = (Downloader *)pointer;
    dl->mutex()->lock ();
    bool aborted = dl->stream()->aborted;
    dl->mutex()->unlock();
    if (aborted)
        return -1;
    return 0;
}

Downloader::Downloader(QObject *parent, const QString &url)
        : QThread(parent)
{
    m_url = url;
    curl_global_init(CURL_GLOBAL_ALL);
    m_stream.buf_fill = 0;
    m_stream.buf = 0;
    m_stream.icy_meta_data = false;
    m_stream.aborted = true;
    m_stream.icy_metaint = 0;
    m_handle = 0;
    m_metacount = 0;
    m_meta_sent = false;
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("HTTP");
    m_codec = QTextCodec::codecForName(settings.value("icy_encoding","windows-1252").toByteArray ());
    m_buffer_size = settings.value("buffer_size",128).toInt() * 1000;
    if (!m_codec)
        m_codec = QTextCodec::codecForName ("UTF-8");
#ifdef WITH_ENCA
    m_analyser = 0;
    if(settings.value("use_enca", false).toBool())
        m_analyser = enca_analyser_alloc(settings.value("enca_lang").toByteArray ().constData());
#endif
    settings.endGroup();
}


Downloader::~Downloader()
{
    abort();
    curl_global_cleanup();
    m_stream.aborted = true;
    m_stream.buf_fill = 0;
    if (m_stream.buf)
        free(m_stream.buf);

    m_stream.buf = 0;
#ifdef WITH_ENCA
    if(m_analyser)
        enca_analyser_free(m_analyser);
#endif
}


qint64 Downloader::read(char* data, qint64 maxlen)
{

    qint64 len = 0;
    m_mutex.lock();
    if (!m_stream.icy_meta_data || m_stream.icy_metaint == 0)
        len = readBuffer(data, maxlen);
    else
    {
        qint64 nread = 0;
        qint64 to_read;
        while (maxlen > nread && m_stream.buf_fill > nread)
        {
            to_read = qMin<qint64>(m_stream.icy_metaint - m_metacount, maxlen - nread);
            //to_read = (maxlen - nread);
            qint64 res = readBuffer(data + nread, to_read);
            nread += res;
            m_metacount += res;
            if (m_metacount == m_stream.icy_metaint)
            {
                m_metacount = 0;
                m_mutex.unlock();
                readICYMetaData();
                m_mutex.lock();
            }

        }
        len = nread;

    }
    m_mutex.unlock();
    return len;
}

Stream *Downloader::stream()
{
    return &m_stream;
}

QMutex *Downloader::mutex()
{
    return &m_mutex;
}

QString Downloader::contentType()
{
    if (m_stream.header.contains("content-type"))
        return m_stream.header.value("content-type").toLower();
    return QString();
}

void Downloader::abort()
{
    m_mutex.lock();

    if (m_stream.aborted)
    {
        m_mutex.unlock();
        return;
    }
    m_stream.aborted = true;
    m_mutex.unlock();
    wait();
    if (m_handle)
    {
        curl_easy_cleanup(m_handle);
        m_handle = 0;
    }
}

int Downloader::bytesAvailable()
{
    m_mutex.lock();
    int b = m_stream.buf_fill;
    m_mutex.unlock();
    return b;
}

void Downloader::run()
{
    qDebug("Downloader: starting download thread");
    m_handle = curl_easy_init();
    //proxy
    if (QmmpSettings::instance()->isProxyEnabled())
        curl_easy_setopt(m_handle, CURLOPT_PROXY,
                         strdup((QmmpSettings::instance()->proxy().host() + ":" +
                                 QString("%1").arg(QmmpSettings::instance()->proxy().port())).
                                toLatin1 ().constData ()));
    else
        curl_easy_setopt(m_handle, CURLOPT_NOPROXY, "*");
    if (QmmpSettings::instance()->useProxyAuth())
        curl_easy_setopt(m_handle, CURLOPT_PROXYUSERPWD,
                         strdup((QmmpSettings::instance()->proxy().userName() + ":" +
                                 QmmpSettings::instance()->proxy().password()).
                                toLatin1 ().constData ()));

    // Set url to download
    curl_easy_setopt(m_handle, CURLOPT_URL, strdup(m_url.toAscii().constData()));
    // callback for wrting
    curl_easy_setopt(m_handle, CURLOPT_WRITEFUNCTION, curl_write_data);
    // Set destination file
    curl_easy_setopt(m_handle, CURLOPT_WRITEDATA, this);
    curl_easy_setopt(m_handle, CURLOPT_HEADERDATA, this);
    curl_easy_setopt(m_handle, CURLOPT_HEADERFUNCTION, curl_header);
    // Disable SSL
    curl_easy_setopt(m_handle, CURLOPT_SSL_VERIFYPEER, false);
    curl_easy_setopt(m_handle, CURLOPT_SSL_VERIFYHOST, 0);
    // Enable progress meter
    curl_easy_setopt(m_handle, CURLOPT_NOPROGRESS, 0);
    curl_easy_setopt(m_handle, CURLOPT_PROGRESSDATA, this);
    curl_easy_setopt(m_handle, CURLOPT_PROGRESSFUNCTION, curl_progress);
    // Any kind of authentication
    curl_easy_setopt(m_handle, CURLOPT_HTTPAUTH, CURLAUTH_ANY);
    curl_easy_setopt(m_handle, CURLOPT_VERBOSE, 1);
    // Auto referrer
    curl_easy_setopt(m_handle, CURLOPT_AUTOREFERER, 1);
    // Follow redirections
    curl_easy_setopt(m_handle, CURLOPT_FOLLOWLOCATION, 1);
    curl_easy_setopt(m_handle, CURLOPT_FAILONERROR, 1);
    curl_easy_setopt(m_handle, CURLOPT_MAXREDIRS, 15);
    // user agent
    QString user_agent = QString("qmmp/%1").arg(Qmmp::strVersion());
    curl_easy_setopt(m_handle, CURLOPT_USERAGENT, qPrintable(user_agent));
    curl_easy_setopt(m_handle, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_0);

    struct curl_slist *http200_aliases = curl_slist_append(NULL, "ICY");
    struct curl_slist *http_headers = curl_slist_append(NULL, "Icy-MetaData: 1");
    curl_easy_setopt(m_handle, CURLOPT_HTTP200ALIASES, http200_aliases);
    curl_easy_setopt(m_handle, CURLOPT_HTTPHEADER, http_headers);
    m_mutex.lock();
    m_stream.buf_fill = 0;
    m_stream.buf = 0;
    m_stream.aborted = false;
    m_stream.header.clear ();
    m_ready  = false;
    int return_code;
    qDebug("Downloader: starting libcurl");
    m_mutex.unlock();
    return_code = curl_easy_perform(m_handle);
    qDebug("curl_easy_perform %d", return_code);
    qDebug("Downloader: thread finished");
}

qint64 Downloader::readBuffer(char* data, qint64 maxlen)
{
    if (m_stream.buf_fill > 0 && !m_stream.aborted)
    {
        int len = qMin<qint64>(m_stream.buf_fill, maxlen);
        memcpy(data, m_stream.buf, len);
        m_stream.buf_fill -= len;
        memmove(m_stream.buf, m_stream.buf + len, m_stream.buf_fill);
        return len;
    }
    return 0;
}

const QString &Downloader::title() const
{
    return m_title;
}

void Downloader::checkBuffer()
{
    if (m_stream.buf_fill > m_buffer_size && !m_ready)
    {
        m_ready  = true;
        qDebug("Downloader: ready");
        if(!m_meta_sent && stream()->icy_meta_data)
        {
            QMap<Qmmp::MetaData, QString> metaData;
            metaData.insert(Qmmp::TITLE, m_stream.header.value("icy-name"));
            metaData.insert(Qmmp::GENRE, m_stream.header.value("icy-genre"));
            metaData.insert(Qmmp::URL, m_url);
            StateHandler::instance()->dispatch(metaData);
        }
        emit readyRead();
    }
    else if (!m_ready)
    {
        StateHandler::instance()->dispatchBuffer(100 * m_stream.buf_fill / m_buffer_size);
        qApp->processEvents();
    }
}

bool Downloader::isReady()
{
    return m_ready;
}

void Downloader::readICYMetaData()
{
    uint8_t packet_size;
    m_metacount = 0;
    m_mutex.lock();
    readBuffer((char *)&packet_size, sizeof(packet_size));
    if (packet_size != 0)
    {
        int size = packet_size * 16;
        char packet[size];
        while (m_stream.buf_fill < size && isRunning())
        {
            m_mutex.unlock();
            qApp->processEvents();
            m_mutex.lock();
        }
        qint64 l = readBuffer(packet, size);
        qDebug("Downloader: ICY metadata: %s", packet);
        parseICYMetaData(packet, l);
    }
    m_mutex.unlock();

}

void Downloader::parseICYMetaData(char *data, qint64 size)
{
    if(!size)
        return;
    QTextCodec *codec = m_codec;
#ifdef WITH_ENCA
    if(m_analyser)
    {
        EncaEncoding encoding = enca_analyse(m_analyser, (uchar *) data, size);
        if(encoding.charset != ENCA_CS_UNKNOWN)
        {
            codec = QTextCodec::codecForName(enca_charset_name(encoding.charset,ENCA_NAME_STYLE_ENCA));
            qDebug("Downloader: detected charset: %s",
                   enca_charset_name(encoding.charset,ENCA_NAME_STYLE_ENCA));
            if(!codec)
                codec = m_codec;
        }
    }
#endif
    QString str = codec->toUnicode(data).trimmed();
    QStringList list(str.split(";", QString::SkipEmptyParts));
    foreach(QString line, list)
    {
        if (line.contains("StreamTitle="))
        {
            line = line.right(line.size() - line.indexOf("=") - 1).trimmed();
            m_title = line.remove("'");
            QMap<Qmmp::MetaData, QString> metaData;
            if (!m_title.isEmpty())
                metaData.insert(Qmmp::TITLE, m_title);
            else
                metaData.insert(Qmmp::TITLE, m_stream.header.value("icy-name"));
            metaData.insert(Qmmp::GENRE, m_stream.header.value("icy-genre"));
            metaData.insert(Qmmp::URL, m_url);
            StateHandler::instance()->dispatch(metaData);
            m_meta_sent = true;
            break;
        }
    }
}
