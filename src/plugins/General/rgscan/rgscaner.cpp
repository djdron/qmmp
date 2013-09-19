/***************************************************************************
 *   Copyright (C) 2013 by Ilya Kotov                                      *
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
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#include <stdio.h>
#include <QStringList>
#include <QDesktopServices>
#include <qmmp/inputsourcefactory.h>
#include <qmmp/decoderfactory.h>
#include <qmmp/metadatamanager.h>
#include <qmmpui/metadataformatter.h>
#include <QtEndian>
#include <QThread>
#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <taglib/mpegfile.h>
#include "rgscaner.h"

#define QStringToTString_qt4(s) TagLib::String(s.toUtf8().constData(), TagLib::String::UTF8)

//static functions
/*static inline void s8_to_s16_2(qint8 *in, qint16 *out, qint64 samples)
{
    for(qint64 i = 0; i < samples; ++i)
        out[i] = in[i] << 8;
    return;
}

static inline void s24_to_s16_2(qint32 *in, qint16 *out, qint64 samples)
{
    for(qint64 i = 0; i < samples; ++i)
        out[i] = in[i] >> 8;
    return;
}

static inline void s32_to_s16_2(qint32 *in, qint16 *out, qint64 samples)
{
    for(qint64 i = 0; i < samples; ++i)
        out[i] = in[i] >> 16;
    return;
}*/

RGScaner::RGScaner()
{}

RGScaner::~RGScaner()
{
    stop();
}

bool RGScaner::prepare(const QString &url)
{
    InputSource *source = InputSource::create(url, 0);
    if(!source->initialize())
    {
        delete source;
        qWarning("RGScaner: Invalid url");
        return false;
    }

    if(source->ioDevice())
    {
        if(!source->ioDevice()->open(QIODevice::ReadOnly))
        {
            delete source;
            qWarning("RGScaner: unable to open input stream, error: %s",
                     qPrintable(m_source->ioDevice()->errorString()));
            return false;
        }
    }

    DecoderFactory *factory = Decoder::findByPath(source->url());

    if(!factory)
    {
        qWarning("RGScaner: unsupported file format");
        delete source;
        return false;
    }
    qDebug("RGScaner: selected decoder: %s",qPrintable(factory->properties().shortName));

    if(factory->properties().noInput && source->ioDevice())
        source->ioDevice()->close();


    Decoder *decoder = factory->create(source->url(), source->ioDevice());
    if(!decoder->initialize())
    {
        qWarning("RGScaner: invalid file format");
        delete source;
        delete decoder;
        return false;
    }
    m_decoder = decoder;
    m_source = source;
    return true;
}

void RGScaner::stop()
{
    /*m_mutex.lock();
    m_user_stop = true;
    m_mutex.unlock();
    wait();
    m_presets.clear();
    qDeleteAll(m_inputs.values());
    m_inputs.clear();
    qDeleteAll(m_decoders);
    m_decoders.clear();*/
}

void RGScaner::run()
{
    qDebug("RGScaner: staring thread %lu",  QThread::currentThreadId());
    m_user_stop = false;

    const int buf_size = 8192;
    AudioParameters ap = m_decoder->audioParameters();
    Qmmp::AudioFormat format = ap.format();
    unsigned char output_buf[(format == Qmmp::PCM_S8) ? buf_size : buf_size * 2];
    qint64 output_at = 0;
    qint64 total = 0;
    quint64 len = 0;
    qint64 totalSize = m_decoder->totalTime() * ap.sampleRate() * ap.channels() * ap.sampleSize() / 1000;

    forever
    {
        // decode
        len = m_decoder->read((char *)(output_buf + output_at), buf_size - output_at);
        if (len > 0)
        {
            output_at += len;
            total += len;
            emit progress(100 * total / totalSize);
            output_at = 0;
        }
        else if (len <= 0)
            break;
    }
    qDebug("RGScaner: thread %ld finished", QThread::currentThreadId());
    emit progress(100);
    emit finished();
}

