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
#include "downloader.h"

static int stop;

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
    return data_size;
}

static size_t curl_header(void *data, size_t size, size_t nmemb,
                          void *pointer)
{
    Downloader *dl = (Downloader *)pointer;
    dl->mutex()->lock ();
    size_t data_size = size * nmemb;
    if (data_size < 2)
        return data_size;

    //qDebug("header received: %s", (char*) data);
    QString str = QString::fromAscii((char *) data, data_size);
    str = str.trimmed ();
    if (str.contains("Content-Type"))
    {
        str = str.right(str.size() - str.indexOf(":") - 1);
        qDebug(qPrintable(QString("content-type: ")+str.trimmed()));
        dl->stream()->content_type = str.trimmed();
    }
    if (str.contains("content-type"))
    {
        str = str.right(str.size() - str.indexOf(":") - 1);
        qDebug(qPrintable(QString("content-type: ")+str.trimmed()));
        dl->stream()->content_type = str.trimmed();
    }
    dl->mutex()->unlock();
    return size * nmemb;
}

int curl_progress(void *pointer, double dltotal, double dlnow, double ultotal, double ulnow)
{
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

    m_stream.aborted = TRUE;
}


Downloader::~Downloader()
{
    abort();
}


qint64 Downloader::read(char* data, qint64 maxlen)
{
    m_mutex.lock();
    if (m_stream.buf_fill>0 && !m_stream.aborted)
    {
        int len = qMin<qint64>(m_stream.buf_fill, maxlen);
        memcpy(data, m_stream.buf, len);
        m_stream.buf_fill -= len;
        memmove(m_stream.buf, m_stream.buf + len, m_stream.buf_fill);
        m_mutex.unlock();
        return len;
    }
    m_mutex.unlock();
    return 0;
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
    return m_stream.content_type;
}

void Downloader::abort()
{
    m_mutex.lock();

    if (m_stream.aborted)
    {
        m_mutex.unlock();
        return;
    }
    m_stream.aborted = TRUE;
    m_mutex.unlock();
    wait();
    curl_easy_cleanup(m_handle);
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
    // Set url to download
    curl_easy_setopt(m_handle, CURLOPT_URL, m_url.toAscii().constData());
    //qDebug("Downloader: url: %s", qPrintable(url));
    // callback for wrting
    curl_easy_setopt(m_handle, CURLOPT_WRITEFUNCTION, curl_write_data);
    // Set destination file
    curl_easy_setopt(m_handle, CURLOPT_WRITEDATA, this);
    curl_easy_setopt(m_handle, CURLOPT_HEADERDATA, this);
    curl_easy_setopt(m_handle, CURLOPT_HEADERFUNCTION, curl_header);
    // Some SSL mambo jambo
    curl_easy_setopt(m_handle, CURLOPT_SSL_VERIFYPEER, FALSE);
    curl_easy_setopt(m_handle, CURLOPT_SSL_VERIFYHOST, 0);
    // Disable progress meter
    curl_easy_setopt(m_handle, CURLOPT_NOPROGRESS, 0);

    curl_easy_setopt(m_handle, CURLOPT_PROGRESSDATA, this);
    curl_easy_setopt(m_handle, CURLOPT_PROGRESSFUNCTION, curl_progress);
    // Any kind of authentication
    curl_easy_setopt(m_handle, CURLOPT_HTTPAUTH, CURLAUTH_ANY);
    //curl_easy_setopt(m_handle, CURLOPT_VERBOSE, 1);
    // Auto referrer
    curl_easy_setopt(m_handle, CURLOPT_AUTOREFERER, 1);
    // Follow redirections
    curl_easy_setopt(m_handle, CURLOPT_FOLLOWLOCATION, 1);
    // user agent
    curl_easy_setopt(m_handle, CURLOPT_USERAGENT, "qmmp/0.2");
    curl_easy_setopt(m_handle, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_0);

    struct curl_slist *http200_aliases = curl_slist_append(NULL, "ICY");
    struct curl_slist *http_headers = curl_slist_append(NULL, "Icy-MetaData: 1");
    curl_easy_setopt(m_handle, CURLOPT_HTTP200ALIASES, http200_aliases);
    curl_easy_setopt(m_handle, CURLOPT_HTTPHEADER, http_headers);
    m_mutex.lock();
    m_stream.buf_fill = 0;
    m_stream.buf = 0;
    m_stream.aborted = FALSE;
    int return_code, response;
    qDebug("Downloader: starting libcurl");
    m_mutex.unlock();
    return_code = curl_easy_perform(m_handle);
    //qDebug("curl_easy_perform %d", return_code);

    m_mutex.lock();
    m_stream.aborted = TRUE;
    m_stream.buf_fill = 0;
    if (m_stream.buf)
        delete m_stream.buf;
    m_stream.buf = 0;
    m_mutex.unlock();
    qDebug("Downloader: thread exited");
}
