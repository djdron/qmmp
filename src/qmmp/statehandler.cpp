/***************************************************************************
 *   Copyright (C) 2008-2011 by Ilya Kotov                                 *
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

#include <QStringList>
#include <QApplication>
#include "soundcore.h"
#include "qmmpevents_p.h"
#include "statehandler.h"

#define TICK_INTERVAL 250
#define PREFINISH_TIME 7000

StateHandler* StateHandler::m_instance = 0;

StateHandler::StateHandler(QObject *parent)
        : QObject(parent), m_mutex(QMutex::Recursive)
{
    if(m_instance)
        qFatal("StateHandler: only one instance is allowed");
    m_instance = this;
    m_elapsed = -1;
    m_bitrate = 0;
    m_frequency = 0;
    m_precision = 0;
    m_channels = 0;
    m_sendAboutToFinish = true;
    m_state = Qmmp::Stopped;
}


StateHandler::~StateHandler()
{
    m_instance = 0;
}

void StateHandler::dispatch(qint64 elapsed,
                            int bitrate,
                            quint32 frequency,
                            int precision,
                            int channels)
{
    m_mutex.lock();
    if (qAbs(m_elapsed - elapsed) > TICK_INTERVAL)
    {
        m_elapsed = elapsed;
        emit (elapsedChanged(elapsed));
        if (m_bitrate != bitrate)
        {
            m_bitrate = bitrate;
            emit (bitrateChanged(bitrate));
        }
        if((SoundCore::instance()->totalTime() > PREFINISH_TIME)
                 && (SoundCore::instance()->totalTime() - m_elapsed < PREFINISH_TIME)
                 && m_sendAboutToFinish)
        {
            m_sendAboutToFinish = false;
            if(SoundCore::instance()->totalTime() - m_elapsed > PREFINISH_TIME/2)
                qApp->postEvent(parent(), new QEvent(EVENT_NEXT_TRACK_REQUEST));
        }
    }
    if (m_frequency != frequency)
    {
        m_frequency = frequency;
        emit frequencyChanged(frequency);
    }
    if (m_precision != precision)
    {
        m_precision = precision;
        emit sampleSizeChanged(precision);
    }
    if (m_channels != channels)
    {
        m_channels = channels;
        emit channelsChanged(channels);
    }
    m_mutex.unlock();
}

void StateHandler::dispatch(const QMap<Qmmp::MetaData, QString> &metaData)
{
    m_mutex.lock();
    QMap<Qmmp::MetaData, QString> tmp = metaData;
    foreach(QString value, tmp.values()) //remove empty keys
    {
        if (value.isEmpty() || value == "0")
            tmp.remove(tmp.key(value));
    }
    if(tmp.isEmpty() || tmp.value(Qmmp::URL).isEmpty()) //skip empty tags
    {
        qWarning("StateHandler: empty metadata");
        m_mutex.unlock();
        return;
    }
    if (m_state == Qmmp::Playing &&
        (m_metaData.isEmpty() || m_metaData.value(Qmmp::URL) == metaData.value(Qmmp::URL)))
    {
        if (m_metaData != tmp)
        {
            m_metaData = tmp;
            qApp->postEvent(parent(), new MetaDataChangedEvent(m_metaData));
        }
    }
    else
        m_cachedMetaData = tmp;
    m_mutex.unlock();
}

void StateHandler::dispatch(Qmmp::State state)
{
    m_mutex.lock();
    //clear
    QList <Qmmp::State> clearStates;
    clearStates << Qmmp::Stopped << Qmmp::NormalError << Qmmp::FatalError;
    if (clearStates.contains(state))
    {
        m_elapsed = -1;
        m_bitrate = 0;
        m_frequency = 0;
        m_precision = 0;
        m_channels = 0;
        m_metaData.clear();
        m_sendAboutToFinish = true;
    }
    if (m_state != state)
    {
        QStringList states;
        states << "Playing" << "Paused" << "Stopped" << "Buffering" << "NormalError" << "FatalError";
        qDebug("StateHandler: Current state: %s; previous state: %s",
               qPrintable(states.at(state)), qPrintable(states.at(m_state)));
        Qmmp::State prevState = state;
        m_state = state;
        qApp->postEvent(parent(), new StateChangedEvent(m_state, prevState));

        if(m_state == Qmmp::Playing && !m_cachedMetaData.isEmpty())
        {
            m_mutex.unlock();
            dispatch(m_cachedMetaData);
            m_mutex.lock();
            m_cachedMetaData.clear();
        }
    }
    m_mutex.unlock();
}

void StateHandler::dispatchBuffer(int percent)
{
    if(m_state == Qmmp::Buffering)
        emit bufferingProgress(percent);
}

qint64 StateHandler::elapsed()
{
    return m_elapsed;
}

int StateHandler::bitrate()
{
    return m_bitrate;
}

int StateHandler::frequency()
{
    return m_frequency;
}

int StateHandler::sampleSize()
{
    return m_precision;
}

int StateHandler::channels()
{
    return m_channels;
}

Qmmp::State StateHandler::state() const
{
    return m_state;
}

void StateHandler::sendNextTrackRequest()
{
    m_mutex.lock();
    if(m_sendAboutToFinish)
    {
        m_sendAboutToFinish = false;
        qApp->postEvent(parent(), new QEvent(EVENT_NEXT_TRACK_REQUEST));
    }
    m_mutex.unlock();
}

void StateHandler::sendFinished()
{
    qApp->postEvent(parent(), new QEvent(EVENT_FINISHED));
}

StateHandler *StateHandler::instance()
{
    return m_instance;
}
