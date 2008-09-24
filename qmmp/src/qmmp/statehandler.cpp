/***************************************************************************
 *   Copyright (C) 2008 by Ilya Kotov                                      *
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

#include "statehandler.h"


StateHandler* StateHandler::m_instance = 0;

StateHandler::StateHandler(QObject *parent)
        : QObject(parent)
{
    m_instance = this;
    m_elapsed = -1;
    m_bitrate = 0;
    m_frequency = 0;
    m_precision = 0;
    m_channels = 0;
    m_state = Qmmp::Stopped;
}


StateHandler::~StateHandler()
{
    m_instance = 0;
}


void StateHandler::dispatch(qint64 elapsed,
                            qint64 totalTime,
                            int bitrate,
                            int frequency,
                            int precision,
                            int channels)
{
    m_mutex.lock();
    if (m_elapsed != elapsed)
    {
        m_elapsed = elapsed;
        emit (elapsedChanged(elapsed));
    }
    if (m_bitrate != bitrate)
    {
        m_bitrate = bitrate;
        emit (bitrateChanged(bitrate));
    }
    if (m_frequency != frequency)
    {
        m_frequency = frequency;
        emit (frequencyChanged(frequency));
    }
    if (m_precision != precision)
    {
        m_precision = precision;
        emit (precisionChanged(precision));
    }
    if (m_channels != channels)
    {
        m_channels = channels;
        emit (channelsChanged(channels));
    }
    m_mutex.unlock();
}

void StateHandler::dispatch(QMap<QString, QString> metaData)
{
    m_mutex.lock();
    if (m_metaData != metaData)
    {
        m_metaData = metaData;
        emit metaDataChanged ();
    }
    m_mutex.unlock();
}

void StateHandler::dispatch(const Qmmp::State &state)
{
    m_mutex.lock();
    if (m_state != state)
    {
        QStringList states;
        states << "Playing" << "Paused" << "Stopped" << "Buffering" << "NormalError" << "FatalError";
        qDebug("StateHandler: Current state: %s; previous state: %s",
               qPrintable(states.at(state)), qPrintable(states.at(m_state)));
        m_state = state;

        //clear
        if (m_state != Qmmp::Playing || m_state != Qmmp::Paused || m_state != Qmmp::Buffering)
        {
            m_elapsed = -1;
            m_bitrate = 0;
            m_frequency = 0;
            m_precision = 0;
            m_channels = 0;
        }
        emit stateChanged(state);
    }
    m_mutex.unlock();
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

int StateHandler::precision()
{
    return m_precision;
}

int StateHandler::channels()
{
    return m_channels;
}

Qmmp::State StateHandler::state()
{
    return m_state;
}

QMap<QString, QString> StateHandler::metaData()
{
    return m_metaData;
}

StateHandler *StateHandler::instance()
{
    return m_instance;
}
