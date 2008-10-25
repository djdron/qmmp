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
    for (int i = 1; i < 10; ++i)
        m_bands[i] = 0;
    m_handler = new StateHandler(this);
    //StateHandler::instance() = 0;
    //StateHandler::m_instance = m_handler;
    connect(m_handler, SIGNAL(elapsedChanged(qint64)), SIGNAL(elapsedChanged(qint64)));
    connect(m_handler, SIGNAL(bitrateChanged(int)), SIGNAL(bitrateChanged(int)));
    connect(m_handler, SIGNAL(frequencyChanged(int)), SIGNAL(frequencyChanged(int)));
    connect(m_handler, SIGNAL(precisionChanged(int)), SIGNAL(precisionChanged(int)));
    connect(m_handler, SIGNAL(channelsChanged(int)), SIGNAL(channelsChanged(int)));
    connect(m_handler, SIGNAL(metaDataChanged ()), SIGNAL(metaDataChanged ()));
    connect(m_handler, SIGNAL(stateChanged (Qmmp::State)), SIGNAL(stateChanged(Qmmp::State)));
    m_volumeControl = VolumeControl::create(this);
    connect(m_volumeControl, SIGNAL(volumeChanged(int, int)), SIGNAL(volumeChanged(int, int)));
}


SoundCore::~SoundCore()
{
    stop();
}

bool SoundCore::play(const QString &source)
{
    stop();
    qDebug("SoundCore: url=%s", qPrintable(source));
    m_source = source;
    if (m_handler->state() != Qmmp::Stopped) //clear error state
        m_handler->dispatch(Qmmp::Stopped);

    m_handler->dispatch(Qmmp::Buffering); //buffering state

    QUrl url;
    if (QFile::exists(source)) //local file
        url = QUrl::fromLocalFile(source);
    else
        url = source;

    m_factory = Decoder::findByURL(url);
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
            QList <FileInfo *> list = m_factory->createPlayList(url.toLocalFile ());
            if (!list.isEmpty())
            {
                m_handler->dispatch(list[0]->metaData());
                while (!list.isEmpty())
                    delete list.takeFirst();
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
    if (m_decoder /*&& m_decoder->isRunning()*/)
    {
        m_decoder->mutex()->lock ();
        m_decoder->stop();
        m_decoder->mutex()->unlock();
        //m_decoder->stateHandler()->dispatch(Qmmp::Stopped);
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
        delete m_output;
        m_output = 0;
    }
    if (m_decoder)
    {
        delete m_decoder;
        m_decoder = 0;
    }
    if (m_input)
    {
        m_input->deleteLater();
        m_input = 0;
    }
    //update VolumeControl
    delete m_volumeControl;
    m_volumeControl = VolumeControl::create(this);
    connect(m_volumeControl, SIGNAL(volumeChanged(int, int)), SIGNAL(volumeChanged(int, int)));
}

void SoundCore::pause()
{
    if (m_output)
    {
        m_output->mutex()->lock ();
        m_output->pause();
        m_output->mutex()->unlock();
    }
    else if (m_decoder)
    {
        m_decoder->mutex()->lock ();
        m_decoder->pause();
        m_decoder->mutex()->unlock();
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
    else if (m_decoder)
    {
        m_decoder->mutex()->lock ();
        m_decoder->seek(pos);
        m_decoder->mutex()->unlock();
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
    SoftwareVolume::setEnabled(b);
    if (m_decoder)
        m_decoder->mutex()->lock();
    delete m_volumeControl;
    m_volumeControl = VolumeControl::create(this);
    connect(m_volumeControl, SIGNAL(volumeChanged(int, int)), SIGNAL(volumeChanged(int, int)));
    if (m_decoder)
        m_decoder->mutex()->unlock();
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

int SoundCore::frequency()
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
        if (!(m_factory = Decoder::findByContent(m_input)))
        {
            m_handler->dispatch(Qmmp::NormalError);
            return FALSE;
        }
    }
    if (!m_factory->properties().noOutput)
    {
        m_output = Output::create(this);
        if (!m_output)
        {
            qWarning("SoundCore: unable to create output");
            m_handler->dispatch(Qmmp::FatalError);
            return FALSE;
        }
        if (!m_output->initialize())
        {
            qWarning("SoundCore: unable to initialize output");
            delete m_output;
            m_output = 0;
            m_handler->dispatch(Qmmp::FatalError);
            return FALSE;
        }
    }
    m_decoder = m_factory->create(this, m_input, m_output, m_source);
    if (!m_decoder)
    {
        qWarning("SoundCore: unsupported fileformat");
        m_block = FALSE;
        stop();
        m_handler->dispatch(Qmmp::NormalError);
        return FALSE;
    }
    m_decoder->setStateHandler(m_handler);
    setEQ(m_bands, m_preamp);
    setEQEnabled(m_useEQ);
    qDebug ("ok");
    connect(m_decoder, SIGNAL(playbackFinished()), SIGNAL(finished()));
    if (m_output)
        m_output->setStateHandler(m_decoder->stateHandler());

    if (m_decoder->initialize())
    {
        if (m_output)
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
