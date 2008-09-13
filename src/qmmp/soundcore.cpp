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

#include "decoderfactory.h"
#include "constants.h"
#include "streamreader.h"
#include "effect.h"
#include "statehandler.h"

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
    m_state = Qmmp::Stopped;
    for (int i = 1; i < 10; ++i)
        m_bands[i] = 0;
}


SoundCore::~SoundCore()
{
    stop();
}

bool SoundCore::play(const QString &source)
{
    stop();
    if (m_state != Qmmp::Stopped) //clear error state
        setState(Qmmp::Stopped);
    m_input = new QFile(source);
    m_output = Output::create(this);
    if (!m_output)
    {
        qWarning("SoundCore: unable to create output");
        setState(Qmmp::FatalError);
        return FALSE;
    }
    if (!m_output->initialize())
    {
        qWarning("SoundCore: unable to initialize output");
        setState(Qmmp::FatalError);
        return FALSE;
    }
    m_source = source;
    return decode();

}

void SoundCore::stop()
{
    if (m_decoder && m_decoder->isRunning())
    {
        m_decoder->mutex()->lock ();
        m_decoder->stop();
        m_decoder->mutex()->unlock();
        m_decoder->stateHandler()->dispatch(Qmmp::Stopped);
    }
    if (m_output)
    {
        m_output->mutex()->lock ();
        m_output->stop();
        m_output->mutex()->unlock();
    }

    // wake up threads
    if (m_decoder)
    {
        m_decoder->mutex()->lock ();
        m_decoder->cond()->wakeAll();
        m_decoder->mutex()->unlock();
    }
    if (m_output)
    {
        m_output->recycler()->mutex()->lock ();
        m_output->recycler()->cond()->wakeAll();
        m_output->recycler()->mutex()->unlock();
    }
    if (m_decoder)
        m_decoder->wait();
    if (m_output)
        m_output->wait();

    if (m_output)
    {
        m_output->deleteLater();
        m_output = 0;
    }
    if (m_decoder)
    {
        m_decoder->deleteLater();
        m_decoder = 0;
    }
    if (m_input)
    {
        m_input->deleteLater();
        m_input = 0;
    }
}

void SoundCore::pause()
{
    if (m_output)
    {
        m_output->mutex()->lock ();
        m_output->pause();
        m_output->mutex()->unlock();
    }

    // wake up threads
    if (m_decoder)
    {
        m_paused = !m_paused;
        m_decoder->mutex()->lock ();
        m_decoder->cond()->wakeAll();
        m_decoder->mutex()->unlock();
    }

    if (m_output)
    {
        m_output->recycler()->mutex()->lock ();
        m_output->recycler()->cond()->wakeAll();
        m_output->recycler()->mutex()->unlock();
    }
}

void SoundCore::seek(qint64 pos)
{
    if (m_output && m_output->isRunning())
    {
        m_output->mutex()->lock ();
        m_output->seek(pos);
        m_output->mutex()->unlock();
        if (m_decoder && m_decoder->isRunning())
        {
            m_decoder->mutex()->lock ();
            m_decoder->seek(pos);
            m_decoder->mutex()->unlock();
        }
    }
}

qint64 SoundCore::length() const
{
    return  (m_decoder) ? m_decoder->lengthInSeconds() : 0;
}

void SoundCore::setEQ(int bands[10], const int &preamp)
{
    for (int i = 0; i < 10; ++i)
        m_bands[i] = bands[i];
    m_preamp = preamp;
    if (m_decoder)
    {
        m_decoder->mutex()->lock ();
        m_decoder->setEQ(m_bands, m_preamp);
        m_decoder->setEQEnabled(m_useEQ);
        m_decoder->mutex()->unlock();
    }
}

void SoundCore::setEQEnabled(bool on)
{
    m_useEQ = on;
    if (m_decoder)
    {
        m_decoder->mutex()->lock ();
        m_decoder->setEQ(m_bands, m_preamp);
        m_decoder->setEQEnabled(on);
        m_decoder->mutex()->unlock();
    }
}

void SoundCore::setVolume(int L, int R)
{
    QSettings settings(QDir::homePath()+"/.qmmp/qmmprc", QSettings::IniFormat);
    bool sofVolume = settings.value("Volume/software_volume", FALSE).toBool();
    if (sofVolume)
    {
        L = qMin(L,100);
        R = qMin(R,100);
        L = qMax(L,0);
        R = qMax(R,0);
        settings.setValue("Volume/left", L);
        settings.setValue("Volume/right", R);
        if (m_decoder)
            m_decoder->setVolume(L,R);
        if (m_output)
            m_output->checkSoftwareVolume();
    }
    else if (m_output)
        m_output->setVolume(L,R);
}

void SoundCore::volume(int *left, int *right)
{
    QSettings settings(QDir::homePath()+"/.qmmp/qmmprc", QSettings::IniFormat);
    bool sofVolume = settings.value("Volume/software_volume", FALSE).toBool();
    if (sofVolume)
    {
        *left = settings.value("Volume/left", 0).toInt();
        *right = settings.value("Volume/right", 0).toInt();
    }
    else if (m_output)
        m_output->volume(left,right);
    return;
}

void SoundCore::updateConfig()
{
    //m_update = TRUE;
    /*if (isInitialized())
        return;
    stop();*/
}

qint64 SoundCore::elapsed()
{
    return  (m_decoder) ? m_decoder->stateHandler()->elapsed() : 0;
}

int SoundCore::bitrate()
{
    return  (m_decoder) ? m_decoder->stateHandler()->bitrate() : 0;
}

int SoundCore::frequency()
{
    return  (m_decoder) ? m_decoder->stateHandler()->frequency() : 0;
}

int SoundCore::precision() //TODO rename
{
    return  (m_decoder) ? m_decoder->stateHandler()->precision() : 0;
}

int SoundCore::channels()
{
    return  (m_decoder) ? m_decoder->stateHandler()->channels() : 0;
}

Qmmp::State SoundCore::state() const
{
    return  (m_decoder) ? m_decoder->stateHandler()->state() : m_state;
}

void SoundCore::setState(Qmmp::State state)
{
    QStringList states;
    states << "Playing" << "Paused" << "Stopped" << "Buffering" << "NormalError" << "FatalError";
    qDebug("SoundCore: Current state: %s", qPrintable(states.at(state)));
    m_state = state;
    emit stateChanged (state);
}

bool SoundCore::decode()
{
    if (!m_input || !m_output)
        return FALSE;
    m_decoder = Decoder::create(this, m_source, m_input, m_output);
    if (!m_decoder)
    {
        qWarning("SoundCore: unsupported fileformat");
        m_block = FALSE;
        stop();
        setState(Qmmp::NormalError);
        return FALSE;
    }
    qDebug ("ok");
    StateHandler *handler = m_decoder->stateHandler();
    connect(handler, SIGNAL(elapsedChanged(qint64)), SIGNAL(elapsedChanged(qint64)));
    connect(handler, SIGNAL(bitrateChanged(int)), SIGNAL(bitrateChanged(int)));
    connect(handler, SIGNAL(frequencyChanged(int)), SIGNAL(frequencyChanged(int)));
    connect(handler, SIGNAL(precisionChanged(int)), SIGNAL(precisionChanged(int)));
    connect(handler, SIGNAL(channelsChanged(int)), SIGNAL(channelsChanged(int)));
    connect(handler, SIGNAL(metaDataChanged ()), SIGNAL(metaDataChanged ()));
    connect(handler, SIGNAL(stateChanged (Qmmp::State)), SLOT(setState(Qmmp::State)));
    connect(m_decoder, SIGNAL(finished()), SIGNAL(finished()));

    if (m_decoder->initialize())
    {
        m_output->start();
        m_decoder->start();
        return TRUE;
    }
    stop();
    return FALSE;
}

SoundCore* SoundCore::instance()
{
    return m_instance;
}
