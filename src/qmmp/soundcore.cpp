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

#include <QIODevice>
#include <QFile>
#include <QApplication>
#include <QSettings>
#include <QDir>

#include "qmmpaudioengine.h"
#include "decoderfactory.h"
#include "streamreader.h"
#include "effect.h"
#include "statehandler.h"
#include "volumecontrol.h"

#include "soundcore.h"


SoundCore *SoundCore::m_instance = 0;

SoundCore::SoundCore(QObject *parent)
        : QObject(parent)
{
    m_instance = this;
    m_decoder = 0;
    m_output = 0;
    m_input = 0;
    m_paused = FALSE;
    m_useEQ = FALSE;
    m_update = FALSE;
    m_block = FALSE;
    m_preamp = 0;
    m_vis = 0;
    m_parentWidget = 0;
    m_factory = 0;
    m_engine = 0;
    for (int i = 1; i < 10; ++i)
        m_bands[i] = 0;
    m_handler = new StateHandler(this);
    connect(m_handler, SIGNAL(elapsedChanged(qint64)), SIGNAL(elapsedChanged(qint64)));
    connect(m_handler, SIGNAL(bitrateChanged(int)), SIGNAL(bitrateChanged(int)));
    connect(m_handler, SIGNAL(frequencyChanged(quint32)), SIGNAL(frequencyChanged(quint32)));
    connect(m_handler, SIGNAL(precisionChanged(int)), SIGNAL(precisionChanged(int)));
    connect(m_handler, SIGNAL(channelsChanged(int)), SIGNAL(channelsChanged(int)));
    connect(m_handler, SIGNAL(metaDataChanged ()), SIGNAL(metaDataChanged ()));
    connect(m_handler, SIGNAL(stateChanged (Qmmp::State)), SIGNAL(stateChanged(Qmmp::State)));
    connect(m_handler, SIGNAL(aboutToFinish()), SIGNAL(aboutToFinish()));
    m_volumeControl = VolumeControl::create(this);
    connect(m_volumeControl, SIGNAL(volumeChanged(int, int)), SIGNAL(volumeChanged(int, int)));
}


SoundCore::~SoundCore()
{
    stop();
}

bool SoundCore::play(const QString &source,  bool queue)
{
    if(!queue)
        stop();
    m_source = source;
    /*if (m_handler->state() != Qmmp::Stopped) //clear error state
        m_handler->dispatch(Qmmp::Stopped);

    m_handler->dispatch(Qmmp::Buffering); //buffering state*/

    QUrl url;
    if (source.contains("://")) //url
        url = source;
    else if (QFile::exists(source))
        url = QUrl::fromLocalFile(source);
    else
    {
        qDebug("SoundCore: file doesn't exist");
        m_handler->dispatch(Qmmp::NormalError);
        return FALSE;
    }

    m_factory = Decoder::findByURL(url);
    m_input = 0;
    if (m_factory)
        return decode();

    if (url.scheme() == "file")
    {
        if ((m_factory = Decoder::findByPath(m_source)))
        {
            m_input = new QFile(m_source);
            if (!m_input->open(QIODevice::ReadOnly))
            {
                qDebug("SoundCore: cannot open input");
                stop();
                m_handler->dispatch(Qmmp::NormalError);
                return FALSE;
            }
            return decode();
        }
        else
        {
            qWarning("SoundCore: unsupported fileformat");
            stop();
            m_handler->dispatch(Qmmp::NormalError);
            return FALSE;
        }
    }
    if (url.scheme() == "http")
    {
        m_input = new StreamReader(source, this);
        connect(m_input, SIGNAL(bufferingProgress(int)), SIGNAL(bufferingProgress(int)));
        connect(m_input, SIGNAL(readyRead()),SLOT(decode()));
        qobject_cast<StreamReader *>(m_input)->downloadFile();
        return TRUE;
    }
    qWarning("SoundCore: unsupported fileformat");
    stop();
    m_handler->dispatch(Qmmp::NormalError);
    return FALSE;
}

void SoundCore::stop()
{
    m_factory = 0;
    m_source.clear();
    if(m_engine)
        m_engine->stop();
    m_input = 0;
    //update VolumeControl
    delete m_volumeControl;
    m_volumeControl = VolumeControl::create(this);
    connect(m_volumeControl, SIGNAL(volumeChanged(int, int)), SIGNAL(volumeChanged(int, int)));
}

void SoundCore::pause()
{
    if(m_engine)
        m_engine->pause();
}

void SoundCore::seek(qint64 pos)
{
    if(m_engine)
        m_engine->seek(pos);
}

const QString SoundCore::url()
{
    return m_source;
}

qint64 SoundCore::totalTime() const
{
    return  (m_engine) ? m_engine->totalTime() : 0;
}

void SoundCore::setEQ(double bands[10], double preamp)
{
    for (int i = 0; i < 10; ++i)
        m_bands[i] = bands[i];
    m_preamp = preamp;
    if (m_decoder)
    {
        m_engine->mutex()->lock ();
        m_engine->setEQ(m_bands, m_preamp);
        m_engine->setEQEnabled(m_useEQ);
        m_engine->mutex()->unlock();
    }
}

void SoundCore::setEQEnabled(bool on)
{
    /*m_useEQ = on;
    if (m_decoder)
    {
        m_decoder->mutex()->lock ();
        m_decoder->setEQ(m_bands, m_preamp);
        m_decoder->setEQEnabled(on);
        m_decoder->mutex()->unlock();
    }*/
}

void SoundCore::setVolume(int L, int R)
{
    m_volumeControl->setVolume(L, R);
}

int SoundCore::leftVolume()
{
    return m_volumeControl->left();
}

int SoundCore::rightVolume()
{
    return m_volumeControl->right();
}

void SoundCore::setSoftwareVolume(bool b)
{
    /*SoftwareVolume::setEnabled(b);
    if (m_decoder)
        m_decoder->mutex()->lock();
    delete m_volumeControl;
    m_volumeControl = VolumeControl::create(this);
    connect(m_volumeControl, SIGNAL(volumeChanged(int, int)), SIGNAL(volumeChanged(int, int)));
    if (m_decoder)
        m_decoder->mutex()->unlock();*/
}

bool SoundCore::softwareVolume()
{
    return SoftwareVolume::instance() != 0;
}

qint64 SoundCore::elapsed()
{
    return  m_handler->elapsed();
}

int SoundCore::bitrate()
{
    return  m_handler->bitrate();
}

quint32 SoundCore::frequency()
{
    return  m_handler->frequency();
}

int SoundCore::precision() //TODO rename
{
    return  m_handler->precision();
}

int SoundCore::channels()
{
    return  m_handler->channels();
}

Qmmp::State SoundCore::state() const
{
    return  m_handler->state();
}

QMap <Qmmp::MetaData, QString> SoundCore::metaData()
{
    return m_handler->metaData();
}

QString SoundCore::metaData(Qmmp::MetaData key)
{
    return m_handler->metaData(key);
}

bool SoundCore::decode()
{
    if (!m_factory)
    {
        if (!m_input->open(QIODevice::ReadOnly))
        {
            qDebug("SoundCore:: cannot open input");
            m_handler->dispatch(Qmmp::NormalError);
            return FALSE;
        }
        if (!(m_factory = Decoder::findByMime(qobject_cast<StreamReader *>(m_input)->contentType())))
            if (!(m_factory = Decoder::findByContent(m_input)))
            {
                m_handler->dispatch(Qmmp::NormalError);
                return FALSE;
            }
    }

    if(!m_engine)
    {
        m_engine = new QmmpAudioEngine(this);
        connect(m_engine, SIGNAL(playbackFinished()), SIGNAL(finished()));
    }

    if(m_engine->initialize(m_source, m_input))
        m_engine->start();
    else
        return FALSE;

    setEQ(m_bands, m_preamp);
    setEQEnabled(m_useEQ);
    qDebug ("ok");

    return TRUE;
}

SoundCore* SoundCore::instance()
{
    return m_instance;
}
