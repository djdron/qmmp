/***************************************************************************
 *   Copyright (C) 2006-2009 by Ilya Kotov                                 *
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
#include "effect.h"
#include "statehandler.h"
#include "inputsource.h"
#include "volumecontrol.h"
#include "enginefactory.h"
#include "metadatamanager.h"
#include "soundcore.h"

SoundCore *SoundCore::m_instance = 0;

SoundCore::SoundCore(QObject *parent)
        : QObject(parent)
{
    m_instance = this;
    m_decoder = 0;
    m_paused = FALSE;
    m_useEQ = FALSE;
    m_update = FALSE;
    m_block = FALSE;
    m_preamp = 0;
    m_vis = 0;
    m_parentWidget = 0;
    m_engine = 0;
    m_pendingEngine = 0;
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
    connect(m_handler, SIGNAL(stateChanged (Qmmp::State)), SLOT(startPendingEngine()));
    connect(m_handler, SIGNAL(aboutToFinish()), SIGNAL(aboutToFinish()));
    connect(m_handler, SIGNAL(bufferingProgress(int)), SIGNAL(bufferingProgress(int)));
    m_volumeControl = VolumeControl::create(this);
    connect(m_volumeControl, SIGNAL(volumeChanged(int, int)), SIGNAL(volumeChanged(int, int)));
}


SoundCore::~SoundCore()
{
    stop();
    MetaDataManager::destroy();
}

bool SoundCore::play(const QString &source, bool queue)
{
    if(!queue)
    {
        stop();
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    }
    MetaDataManager::instance(); //create metadata manager

    InputSource *s = InputSource::create(source, this);
    m_pendingSources.append(s);
    if(state() == Qmmp::Stopped)
        m_handler->dispatch(Qmmp::Buffering);
    connect(s, SIGNAL(ready(InputSource *)), SLOT(enqueue(InputSource *)));
    bool ok = s->initialize();
    if(!ok)
    {
        m_pendingSources.removeAll(s);
        s->deleteLater();
        m_handler->dispatch(Qmmp::NormalError);
    }
    return ok;
}

void SoundCore::stop()
{
    m_source.clear();
    if(m_engine)
        m_engine->stop();
    qDeleteAll(m_pendingSources);
    m_pendingSources.clear();
    if(m_pendingEngine)
        delete m_pendingEngine;
    m_pendingEngine = 0;
    //update VolumeControl
    delete m_volumeControl;
    m_volumeControl = VolumeControl::create(this);
    connect(m_volumeControl, SIGNAL(volumeChanged(int, int)), SIGNAL(volumeChanged(int, int)));
    if(state() == Qmmp::NormalError || state() == Qmmp::FatalError || state() == Qmmp::Buffering)
        StateHandler::instance()->dispatch(Qmmp::Stopped); //clear error and buffering state
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
    if (m_engine)
    {
        m_engine->setEQ(m_bands, m_preamp);
        m_engine->setEQEnabled(m_useEQ);
    }
}

void SoundCore::setEQEnabled(bool on)
{
    m_useEQ = on;
    if (m_engine)
    {
        m_engine->setEQ(m_bands, m_preamp);
        m_engine->setEQEnabled(on);
    }
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
    SoftwareVolume::setEnabled(b); //TODO move to engine settings
    if (m_engine)
        m_engine->mutex()->lock();
    delete m_volumeControl;
    m_volumeControl = VolumeControl::create(this);
    connect(m_volumeControl, SIGNAL(volumeChanged(int, int)), SIGNAL(volumeChanged(int, int)));
    if (m_engine)
        m_engine->mutex()->unlock();
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

bool SoundCore::enqueue(InputSource *s)
{
    m_pendingSources.removeAll(s);
    if(!m_engine)
    {
        m_engine = new QmmpAudioEngine(this);
        connect(m_engine, SIGNAL(playbackFinished()), SIGNAL(finished()));
    }

    setEQ(m_bands, m_preamp);
    setEQEnabled(m_useEQ);
    setReplayGainSettings(m_replayGainSettings);
    if(m_engine->enqueue(s))
    {
        m_source = s->url();
        if(state() == Qmmp::Stopped || state() == Qmmp::Buffering)
            m_engine->play();
    }
    else
    {
        //current engine doesn't support this stream, trying to find another
        AbstractEngine *engine = new QmmpAudioEngine(this); //internal engine
        if(!engine->enqueue(s))
        {
            engine->deleteLater();
            engine = 0;
        }

        if(!engine)
        {
            QList <EngineFactory*> factories = *AbstractEngine::factories();
            foreach(EngineFactory *f, *AbstractEngine::factories())
            {
                engine = f->create(this); //engine plugin
                if(!engine->enqueue(s))
                {
                    engine->deleteLater();
                    engine = 0;
                }
            }
        }

        if(!engine) //unsupported file format
        {
            s->deleteLater();
            return FALSE;
        }
        connect(engine, SIGNAL(playbackFinished()), SIGNAL(finished()));
        engine->setEQ(m_bands, m_preamp);
        engine->setEQEnabled(m_useEQ);
        engine->setReplayGainSettings(m_replayGainSettings);
        if (m_handler->state() == Qmmp::Playing || m_handler->state() == Qmmp::Paused)
        {
            if(m_pendingEngine)
                m_pendingEngine->deleteLater();
            m_pendingEngine = engine;
        }
        else
        {
            m_engine->deleteLater();
            m_engine = engine;
            m_engine->play();
            m_pendingEngine = 0;
        }
    }
    return TRUE;
}

void SoundCore::startPendingEngine()
{
    if(state() == Qmmp::Stopped && m_pendingEngine)
    {
        if(m_engine)
            delete m_engine;
        m_engine = m_pendingEngine;
        m_pendingEngine = 0;
        m_engine->play();
    }
}

ReplayGainSettings SoundCore::replayGainSettings() const
{
    return m_replayGainSettings;
}

void SoundCore::setReplayGainSettings(const ReplayGainSettings &settings)
{
    m_replayGainSettings = settings;
    if(m_engine)
        m_engine->setReplayGainSettings(settings);
}

SoundCore* SoundCore::instance()
{
    return m_instance;
}
