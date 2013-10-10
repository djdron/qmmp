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
#include <math.h>
#include <qmmp/inputsourcefactory.h>
#include <qmmp/decoderfactory.h>
#include "rgscaner.h"

RGScaner::RGScaner()
{
    m_gain = 0.;
    m_peak = 0.;
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
    m_user_stop = false;
    return true;
}

void RGScaner::stop()
{
    m_mutex.lock();
    m_user_stop = true;
    m_mutex.unlock();
}

bool RGScaner::isRunning()
{
    return m_is_running;
}

double RGScaner::gain() const
{
    return m_gain;
}

double RGScaner::peak() const
{
    return m_peak;
}

GainHandle_t *RGScaner::handle()
{
    return m_handle;
}

void RGScaner::run()
{
    if(m_user_stop)
        return;
    m_is_running = true;
    qDebug("RGScaner: staring thread %lu",  QThread::currentThreadId());
    m_user_stop = false;

    AudioParameters ap = m_decoder->audioParameters();
    Qmmp::AudioFormat format = ap.format();
    bool headroom = m_decoder->hasHeadroom();
    const int buf_size = 8192; //samples
    double out_left[buf_size], out_right[buf_size]; //replay gain buffers
    float float_buf[buf_size]; //float buffer
    char char_buf[buf_size*ap.sampleSize()]; //char buffer
    qint64 totalSamples = m_decoder->totalTime() * ap.sampleRate() * ap.channels() / 1000;
    qint64 sample_counter = 0;
    qint64 samples = 0;
    double max = 0;

    if(m_handle)
        DeinitGainAbalysis(m_handle);
    InitGainAnalysis(&m_handle, ap.sampleRate());

    forever
    {
        if(headroom)
        {
            samples = m_decoder->read(float_buf, buf_size);

            if(samples <= 0)
                break;          //TODO add error handler

            if(ap.channels() == 2)
            {
                for(int i = 0; i < (samples >> 1); ++i)
                {
                    out_left[i] = float_buf[i*2]*32768.0;
                    out_right[i] = float_buf[i*2+1]*32768.0;
                    max = qMax(fabs(out_left[i]), max);
                    max = qMax(fabs(out_right[i]), max);
                }
            }
            else if(ap.channels() == 1)
            {
                for(int i = 0; i < samples; ++i)
                {
                    out_left[i] = float_buf[i]*32768.0;
                    max = qMax(fabs(out_left[i]), max);
                }
            }
        }
        else
        {
            qint64 len = m_decoder->read(char_buf, buf_size*ap.sampleSize());

            if(len <= 0)
                break;          //TODO add error handler

            samples = len / ap.sampleSize();

            if(ap.channels() == 2)
            {
                for(int i = 0; i < (samples >> 1); ++i)
                {
                    switch (format)
                    {
                    case Qmmp::PCM_S8:
                        out_left[i] = char_buf[i*2]*32768.0/128.0;
                        out_right[i] = char_buf[i*2+1]*32768.0/128.0;
                        break;
                    case Qmmp::PCM_S16LE:
                        out_left[i] = ((short *)char_buf)[i*2];
                        out_right[i] = ((short *)char_buf)[i*2+1];
                        break;
                    case Qmmp::PCM_S24LE:
                        out_left[i] = ((qint32 *)char_buf)[i*2]*32768.0/((1U << 23));
                        out_right[i] = ((qint32 *)char_buf)[i*2+1]*32768.0/((1U << 23));
                        break;
                    case Qmmp::PCM_S32LE:
                        out_left[i] = ((qint32 *)char_buf)[i*2]*32768.0/((1U << 31));
                        out_right[i] = ((qint32 *)char_buf)[i*2+1]*32768.0/((1U << 31));
                        break;
                    default:
                        break;
                    }
                    max = qMax(fabs(out_left[i]), max);
                    max = qMax(fabs(out_right[i]), max);
                }
            }
            else if(ap.channels() == 1)
            {
                for(int i = 0; i < samples; ++i)
                {
                    switch (format)
                    {
                    case Qmmp::PCM_S8:
                        out_left[i] = char_buf[i*2]*32768.0/128.0;
                        break;
                    case Qmmp::PCM_S16LE:
                        out_left[i] = ((short *)char_buf)[i*2];
                        break;
                    case Qmmp::PCM_S24LE:
                        out_left[i] = ((qint32 *)char_buf)[i*2]*32768.0/((1U << 23));
                        break;
                    case Qmmp::PCM_S32LE:
                        out_left[i] = ((qint32 *)char_buf)[i*2]*32768.0/((1U << 31));
                        break;
                    default:
                        break;
                    }
                    max = qMax(fabs(out_left[i]), max);
                }
            }
        }

        size_t samples_per_channel = samples >> ((ap.channels() == 2) ? 1 : 0);

        AnalyzeSamples(m_handle, out_left, out_right, samples_per_channel, ap.channels());
        sample_counter += samples;
        emit progress(100 * sample_counter / totalSamples);

        m_mutex.lock();
        if(m_user_stop)
        {
            m_mutex.unlock();
            break;
        }
        m_mutex.unlock();
    }

    m_gain = GetTitleGain(m_handle);
    m_peak = max/32768.0;
    qDebug("RGScaner: peak = %f", m_peak);
    qDebug("RGScaner: thread %lu finished", QThread::currentThreadId());
    emit progress(100);
    emit finished(m_url);
    m_is_running = false;
}
