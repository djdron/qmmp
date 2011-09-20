/***************************************************************************
 *   Copyright (C) 2006-2011 by Ilya Kotov                                 *
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
#include "qmmpevents_p.h"
#include "qmmpaudioengine_p.h"
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
    if(m_instance)
        qFatal("SoundCore: only one instance is allowed");
    m_instance = this;
    m_decoder = 0;
    m_parentWidget = 0;
    m_engine = 0;
    m_volumeControl = 0;
    m_nextState = NO_ENGINE;
    m_handler = new StateHandler(this);
    connect(m_handler, SIGNAL(elapsedChanged(qint64)), SIGNAL(elapsedChanged(qint64)));
    connect(m_handler, SIGNAL(bitrateChanged(int)), SIGNAL(bitrateChanged(int)));
    connect(m_handler, SIGNAL(frequencyChanged(quint32)), SIGNAL(frequencyChanged(quint32)));
    connect(m_handler, SIGNAL(sampleSizeChanged(int)), SIGNAL(sampleSizeChanged(int)));
    connect(m_handler, SIGNAL(channelsChanged(int)), SIGNAL(channelsChanged(int)));
    connect(m_handler, SIGNAL(bufferingProgress(int)), SIGNAL(bufferingProgress(int)));
    updateVolume();
    connect(QmmpSettings::instance(), SIGNAL(eqSettingsChanged()), SIGNAL(eqSettingsChanged()));
    connect(QmmpSettings::instance(), SIGNAL(audioSettingsChanged()), SLOT(updateVolume()));
}

SoundCore::~SoundCore()
{
    stop();
    MetaDataManager::destroy();
    m_instance = 0;
}

bool SoundCore::play(const QString &source, bool queue, qint64 offset)
{
    if(!queue)
        stop();

    MetaDataManager::instance(); //create metadata manager

    InputSource *s = InputSource::create(source, this);
    s->setOffset(offset);
    m_sources.enqueue(s);

    connect(s, SIGNAL(ready()), SLOT(startNextSource()));
    connect(s, SIGNAL(error()), SLOT(startNextSource()));

    if(!s->initialize())
    {
        m_sources.removeAll(s);
        s->deleteLater();
        if(m_handler->state() == Qmmp::Stopped || m_handler->state() == Qmmp::Buffering)
                m_handler->dispatch(Qmmp::NormalError);
        return false;
    }
    if(m_handler->state() == Qmmp::Stopped)
        m_handler->dispatch(Qmmp::Buffering);
    return true;
}

void SoundCore::stop()
{
    qApp->sendPostedEvents(this, 0);
    m_url.clear();
    if(m_engine)
    {
        m_engine->stop();
        qApp->sendPostedEvents(this, 0);
        //m_engine->deleteLater();
        //m_engine = 0;
    }
    qDeleteAll(m_sources);
    m_sources.clear();
    m_nextState = NO_ENGINE;
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

const QString SoundCore::url() const
{
    return m_url;
}

bool SoundCore::nextTrackAccepted() const
{
    return m_nextState == SAME_ENGINE;
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

int SoundCore::sampleSize()
{
    return m_handler->sampleSize();
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
    return m_metaData;
}

QString SoundCore::metaData(Qmmp::MetaData key)
{
    return m_metaData[key];
}

void SoundCore::startNextSource()
{
    if(m_sources.isEmpty())
        return;

    InputSource *s = m_sources.dequeue();
    m_url = s->url();

    if(s->ioDevice() && !s->ioDevice()->open(QIODevice::ReadOnly))
    {
        qWarning("SoundCore: input error: %s", qPrintable(s->ioDevice()->errorString()));
        m_url.clear();
        s->deleteLater();
        m_nextState = INVALID_SOURCE;
        return;
    }

    if(!m_engine)
    {
        if((m_engine = AbstractEngine::create(s, this)))
        {
            m_engine->play();
            m_nextState = NO_ENGINE;
            return;
        }
        else
        {
            s->deleteLater();
            m_handler->dispatch(Qmmp::NormalError);
            return;
        }
    }
    else if(m_engine->enqueue(s))
    {
        if(state() == Qmmp::Stopped || state() == Qmmp::Buffering)
        {
            m_engine->play();
            m_nextState = NO_ENGINE;
        }
        else
        {
            m_nextState = SAME_ENGINE;
        }
    }
    else
    {
        m_sources.prepend(s); //try for next engine
        if(s->ioDevice())
            s->ioDevice()->close();
        m_nextState = ANOTHER_ENGINE;
        if(state() == Qmmp::Stopped || state() == Qmmp::Buffering)
        {
            startNextEngine();
        }
    }
}

void SoundCore::startNextEngine()
{
    switch(m_nextState)
    {
    case NO_ENGINE:
    case SAME_ENGINE:
    {
        if(m_sources.isEmpty())
            m_nextState = NO_ENGINE;
        break;
    }
    case ANOTHER_ENGINE:
    {
        m_nextState = NO_ENGINE;
        if(m_engine)
        {
            m_engine->deleteLater();
            m_engine = 0;
        }
        if(!m_sources.isEmpty())
        {
            m_handler->dispatch(Qmmp::Buffering);
            startNextSource();
        }
        break;
    }
    case INVALID_SOURCE:
        m_handler->dispatch(Qmmp::NormalError);
    }
}

SoundCore* SoundCore::instance()
{
    return m_instance;
}

bool SoundCore::event(QEvent *e)
{
    if(e->type() == EVENT_STATE_CHANGED)
    {
        Qmmp::State st = ((StateChangedEvent *) e)->currentState();
        emit stateChanged(st);
        if(st == Qmmp::Stopped)
            startNextEngine();
    }
    else if(e->type() == EVENT_METADATA_CHANGED)
    {
        m_metaData = ((MetaDataChangedEvent *) e)->metaData();
        emit metaDataChanged();
    }
    else if(e->type() == EVENT_NEXT_TRACK_REQUEST)
        emit nextTrackRequest();
    else if(e->type() == EVENT_FINISHED)
        emit finished();
    else
        return QObject::event(e);
    return true;
}
