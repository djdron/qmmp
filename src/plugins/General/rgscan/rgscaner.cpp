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
#include <stdint.h>
#include <QStringList>
#include <QThread>
#include <qmmp/inputsourcefactory.h>
#include <qmmp/decoderfactory.h>
#include "rgscaner.h"

RGScaner::RGScaner()
{
    m_gain = 0.;
    m_user_stop = false;
    m_is_running = false;
    m_handle = 0;
    m_decoder = 0;
    m_source = 0;
}

RGScaner::~RGScaner()
{
    stop();
    if(m_handle)
    {
        DeinitGainAbalysis(m_handle);
        m_handle = 0;
    }
    if(m_decoder)
    {
        delete m_decoder;
        m_decoder = 0;
    }
    if(m_source)
    {
        delete m_source;
        m_source = 0;
    }
}

bool RGScaner::prepare(const QString &url)
{
    m_url = url;
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
    m_mutex.lock();
    m_user_stop = true;
    m_mutex.unlock();
    wait();
}

bool RGScaner::isRunning()
{
    return m_is_running;
}

double RGScaner::gain()
{
    return m_gain;
}

GainHandle_t *RGScaner::handle()
{
    return m_handle;
}

void RGScaner::run()
{
    m_is_running = true;
    qDebug("RGScaner: staring thread %lu",  QThread::currentThreadId());
    m_user_stop = false;

    const int buf_size = 8192; //samples
    AudioParameters ap = m_decoder->audioParameters();
    //Qmmp::AudioFormat format = ap.format();
    unsigned char output_buf[buf_size];
    qint64 output_at = 0;
    qint64 total = 0;
    qint64 len = 0;
    qint64 totalSize = m_decoder->totalTime() * ap.sampleRate() * ap.channels() * ap.sampleSize() / 1000;
    double out_left[buf_size/4];
    double out_right[buf_size/4];


    if(m_handle)
    {
        DeinitGainAbalysis(m_handle);
        m_handle = 0;
    }

    InitGainAnalysis(&m_handle, 44100);

    forever
    {
        // decode
        len = m_decoder->read((char *)(output_buf + output_at), buf_size - output_at);
        if (len > 0)
        {
            output_at += len;
            total += len;
            emit progress(100 * total / totalSize);


            for(int i = 0; i < len/4; ++i)
            {
                out_left[i] = ((short *) output_buf)[i*2];
                out_right[i] = ((short *) output_buf)[i*2+1];
            }
            AnalyzeSamples(m_handle, out_left, out_right, len/4, 2);

            output_at = 0;
        }
        else if (len <= 0)
            break;

        m_mutex.lock();
        if(m_user_stop)
        {
            m_mutex.unlock();
            break;
        }
        m_mutex.unlock();
    }

    m_gain = GetTitleGain(m_handle);
    qDebug("RGScaner: thread %ld finished", QThread::currentThreadId());
    m_is_running = false;
    emit progress(100);
    emit finished(m_url);
}
