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
#include "qmmpsettings.h"
#include "soundcore.h"

SoundCore *SoundCore::m_instance = 0;

SoundCore::SoundCore(QObject *parent)
        : QObject(parent)
{
    m_instance = this;
    m_decoder = 0;
    m_error = false;
    m_parentWidget = 0;
    m_engine = 0;
    m_pendingEngine = 0;
    m_volumeControl = 0;
    m_handler = new StateHandler(this);
    connect(m_handler, SIGNAL(elapsedChanged(qint64)), SIGNAL(elapsedChanged(qint64)));
    connect(m_handler, SIGNAL(bitrateChanged(int)), SIGNAL(bitrateChanged(int)));
    connect(m_handler, SIGNAL(frequencyChanged(quint32)), SIGNAL(frequencyChanged(quint32)));
    connect(m_handler, SIGNAL(precisionChanged(int)), SIGNAL(precisionChanged(int)));
    connect(m_handler, SIGNAL(channelsChanged(int)), SIGNAL(channelsChanged(int)));
    connect(m_handler, SIGNAL(metaDataChanged ()), SIGNAL(metaDataChanged ()));
    connect(m_handler, SIGNAL(stateChanged (Qmmp::State)), SIGNAL(stateChanged(Qmmp::State)));
    connect(m_handler, SIGNAL(stateChanged (Qmmp::State)), SLOT(startPendingEngine()));
    connect(m_handler, SIGNAL(nextTrackRequest()), SIGNAL(nextTrackRequest()));
    connect(m_handler, SIGNAL(bufferingProgress(int)), SIGNAL(bufferingProgress(int)));
    updateVolume();
    connect(QmmpSettings::instance(), SIGNAL(eqSettingsChanged()), SIGNAL(eqSettingsChanged()));
    connect(QmmpSettings::instance(), SIGNAL(audioSettingsChanged()), SLOT(updateVolume()));
}

SoundCore::~SoundCore()
{
    stop();
    MetaDataManager::destroy();
}

bool SoundCore::play(const QString &source, bool queue, qint64 offset)
{
    if(!queue)
        stop();
    else
    {
        qDeleteAll(m_pendingSources);
        m_pendingSources.clear();
        if(m_pendingEngine)
            delete m_pendingEngine;
        m_pendingEngine = 0;
    }
    MetaDataManager::instance(); //create metadata manager

    InputSource *s = InputSource::create(source, this);
    s->setOffset(offset);
    m_pendingSources.append(s);
    if(state() == Qmmp::Stopped)
        m_handler->dispatch(Qmmp::Buffering);
    connect(s, SIGNAL(ready()), SLOT(enqueue()));
    connect(s, SIGNAL(error()), SLOT(enqueue()));
    bool ok = s->initialize();
    if(!ok)
    {
        m_pendingSources.removeAll(s);
        s->deleteLater();
        if(m_handler->state() == Qmmp::Stopped || m_handler->state() == Qmmp::Buffering)
            m_handler->dispatch(Qmmp::NormalError);
    }
    return ok;
}

void SoundCore::stop()
{
    m_url.clear();
    if(m_pendingEngine)
        delete m_pendingEngine;
    m_pendingEngine = 0;
    if(m_engine)
        m_engine->stop();
    qDeleteAll(m_pendingSources);
    m_pendingSources.clear();
    updateVolume();
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
    return m_url;
}

qint64 SoundCore::totalTime() const
{
    return  (m_engine) ? m_engine->totalTime() : 0;
}

EqSettings SoundCore::eqSettings() const
{
    return QmmpSettings::instance()->eqSettings();
}

void SoundCore::setEqSettings(const EqSettings &settings)
{
    QmmpSettings::instance()->setEqSettings(settings);
}

void SoundCore::setVolume(int L, int R)
{
    L = qBound(0, L, 100);
    R = qBound(0, R, 100);
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

void SoundCore::updateVolume()
{
    if (m_engine)
        m_engine->mutex()->lock();
    if(m_volumeControl)
        delete m_volumeControl;
    m_volumeControl = VolumeControl::create(this);
    connect(m_volumeControl, SIGNAL(volumeChanged(int, int)), SIGNAL(volumeChanged(int, int)));
    if (m_engine)
        m_engine->mutex()->unlock();
}

qint64 SoundCore::elapsed()
{
    return m_handler->elapsed();
}

int SoundCore::bitrate()
{
    return m_handler->bitrate();
}

quint32 SoundCore::frequency()
{
    return m_handler->frequency();
}

int SoundCore::precision() //TODO rename
{
    return m_handler->precision();
}

int SoundCore::channels()
{
    return m_handler->channels();
}

Qmmp::State SoundCore::state() const
{
    return m_handler->state();
}

QMap <Qmmp::MetaData, QString> SoundCore::metaData()
{
    return m_handler->metaData();
}

QString SoundCore::metaData(Qmmp::MetaData key)
{
    return m_handler->metaData(key);
}

bool SoundCore::enqueue()
{
    InputSource *s = qobject_cast<InputSource*>(sender());
    if(!s)
    {
        qWarning("SoundCore: unknown signal source");
        return false;
    }

    m_pendingSources.removeAll(s);
    m_url = s->url();

    if(s->ioDevice())
    {
        bool ok = s->ioDevice()->open(QIODevice::ReadOnly);
        if(!ok)
        {
            qWarning("SoundCore: input error: %s", qPrintable(s->ioDevice()->errorString()));
            m_url.clear();
            s->deleteLater();
            if(state() == Qmmp::Stopped || state() == Qmmp::Buffering)
            {
                m_handler->dispatch(Qmmp::NormalError);
            }
            else
                m_error = true;
            return false;
        }
    }

    if(!m_engine)
    {
        if((m_engine = AbstractEngine::create(s, this)))
        {
            connect(m_engine, SIGNAL(playbackFinished()), SIGNAL(finished()));
            m_engine->play();
            m_handler->setCurrentEngine(m_engine);
            return true;
        }
        else
        {
            s->deleteLater();
            m_handler->setCurrentEngine(0);
            m_handler->dispatch(Qmmp::NormalError);
            return false;
        }
    }

    if(m_engine->enqueue(s))
    {
        if(state() == Qmmp::Stopped || state() == Qmmp::Buffering)
            m_engine->play();
        else
            m_handler->setNextEngine(m_engine);
        m_handler->setCurrentEngine(m_engine);
    }
    else
    {
        AbstractEngine *engine = AbstractEngine::create(s, this);
        if(!engine)
        {
            s->deleteLater();
            m_handler->setCurrentEngine(0);
            if(state() == Qmmp::Stopped || state() == Qmmp::Buffering)
                m_handler->dispatch(Qmmp::NormalError);
            else
                m_error = true;
            return false;
        }
        connect(engine, SIGNAL(playbackFinished()), SIGNAL(finished()));
        if (m_handler->state() == Qmmp::Playing || m_handler->state() == Qmmp::Paused)
        {
            if(m_pendingEngine)
                m_pendingEngine->deleteLater();
            m_pendingEngine = engine;
            m_handler->setNextEngine(engine);
        }
        else
        {
            m_engine->deleteLater();
            m_engine = engine;
            m_engine->play();
            m_handler->setCurrentEngine(m_engine);
            m_pendingEngine = 0;
        }
    }
    return true;
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
        m_handler->setCurrentEngine(m_engine);
    }
    else if(state() == Qmmp::Stopped && m_error)
    {
        m_error = false;
        m_handler->dispatch(Qmmp::NormalError);
    }
}

SoundCore* SoundCore::instance()
{
    return m_instance;
}
