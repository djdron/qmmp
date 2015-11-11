/***************************************************************************
 *   Copyright (C) 2015 by Ivan Ponomarev                                  *
 *   ivantrue@gmail.com                                                    *
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

#include "outputqtmultimedia.h"

#include <QAudioOutput>
#include <QAudioFormat>
#include <QAudioDeviceInfo>
#include <QBuffer>
#include <QSettings>
#include <QDebug>


OutputQtMultimedia::OutputQtMultimedia() : Output(), m_buffer(NULL)
{
}

OutputQtMultimedia::~OutputQtMultimedia()
{
}

bool OutputQtMultimedia::initialize(quint32 freq, ChannelMap map, Qmmp::AudioFormat format)
{
    QAudioFormat qformat;
    qformat.setCodec("audio/pcm");
    qformat.setSampleRate(freq);
    qformat.setByteOrder(QAudioFormat::LittleEndian);
    qformat.setChannelCount(map.size());
    qformat.setSampleType(QAudioFormat::SignedInt);
    switch (format)
    {
    case Qmmp::PCM_S8:
        qformat.setSampleSize(8);
        break;
    case Qmmp::PCM_S16LE:
        qformat.setSampleSize(16);
        break;
    case Qmmp::PCM_S24LE:
        qformat.setSampleSize(24);
        break;
    case Qmmp::PCM_S32LE:
        qformat.setSampleSize(32);
        break;
    default:
        break;
    }

    if (!qformat.isValid())
        return false;

    const QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    const QString saved_device_name = settings.value("QTMULTIMEDIA/device").toString();

    QAudioDeviceInfo device_info;
    if (!saved_device_name.isEmpty())
    {
        const QList<QAudioDeviceInfo> devices = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
        foreach (const QAudioDeviceInfo &info, devices)
        {
            if (info.deviceName()==saved_device_name)
            {
                if (info.isFormatSupported(qformat))
                {
                    device_info = info;
                    break;
                }
                else
                    qDebug() << "OutputQtMultimedia: Output device: " << saved_device_name << " is not supported";
            }
        }
    }

    if (device_info.isNull())
    {
        device_info = QAudioDeviceInfo::defaultOutputDevice();
        if (!device_info.isFormatSupported(qformat))
            return false;
    }

    qDebug() << "OutputQtMultimedia: Using output device: " << device_info.deviceName();

    m_output.reset(new QAudioOutput(device_info, qformat));
    m_buffer = m_output->start();

    configure(freq, map, format);
    return true;
}


qint64 OutputQtMultimedia::latency()
{
    return 0;
}

qint64 OutputQtMultimedia::writeAudio(unsigned char *data, qint64 maxSize)
{
    return m_buffer->write((const char*)data, maxSize);
}

void OutputQtMultimedia::drain()
{
    m_buffer->waitForBytesWritten(-1);
}

void OutputQtMultimedia::reset()
{
    m_buffer->reset();
    m_buffer = m_output->start();
}

void OutputQtMultimedia::suspend()
{
    m_output->suspend();
}

void OutputQtMultimedia::resume()
{
    m_output->resume();
}
