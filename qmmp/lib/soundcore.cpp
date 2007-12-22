/***************************************************************************
 *   Copyright (C) 2006 by Ilya Kotov                                      *
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
    for (int i = 1; i < 10; ++i)
        m_bands[i] = 0;
    m_error = NoError;
    m_output = Output::create(this);
    if (!m_output)
    {
        m_error = DecoderError;
        qWarning("SoundCore: unable to create output");
    }
    connect(m_output, SIGNAL(stateChanged(const OutputState&)),
            SIGNAL(outputStateChanged(const OutputState&)));

    QList<OutputFactory*> *outputFactories = Output::outputFactories();
    foreach(OutputFactory* of, *outputFactories)
    qApp->installTranslator(of->createTranslator(this));

    QList<DecoderFactory*> *decoderFactories = Decoder::decoderFactories();
    foreach(DecoderFactory* df, *decoderFactories)
    qApp->installTranslator(df->createTranslator(this));

    Effect::effectFactories();
}


SoundCore::~SoundCore()
{}

bool SoundCore::play(const QString &source)
{
    stop();
    if (source.isEmpty())
    {
        m_error = DecoderError;
        return FALSE;
    }
    if (source.left(4) == "http")
    {
        m_input = new StreamReader(source, this);
        connect(m_input, SIGNAL(titleChanged(const QString&)),
                SIGNAL(titleChanged(const QString&)));
        connect(m_input, SIGNAL(readyRead()),SLOT(decode()));
    }
    else
        m_input = new QFile(source);

    m_error = OutputError;
    if (!m_output)
    {
        m_output = Output::create(this);
        if (!m_output)
        {
            qWarning("SoundCore: unable to create output");
            return FALSE;
        }
        connect(m_output, SIGNAL(stateChanged(const OutputState&)),
                SIGNAL(outputStateChanged(const OutputState&)));
        connect(m_input, SIGNAL(readyRead()),SLOT(read()));
    }
    if (! m_output->initialize())
        return FALSE;

    m_error = DecoderError;

    Visual *visual = 0;
    foreach(visual, m_visuals)
    m_output->addVisual(visual);

    VisualFactory* factory;
    foreach(factory, *Visual::visualFactories())
    {
        if (Visual::isEnabled(factory))
            m_output->addVisual(factory, m_parentWidget);
    }

    m_source = source;
    if (source.left(4) != "http")
        return decode();
    else
        qobject_cast<StreamReader *>(m_input)->downloadFile();
    return TRUE;

}

uint SoundCore::error()
{
    return m_error;
}

void SoundCore::stop()
{
    if (m_block)
        return;
    m_paused = FALSE;
    if (m_decoder && m_decoder->isRunning())
    {
        m_decoder->mutex()->lock ();
        m_decoder->stop();
        m_decoder->mutex()->unlock();
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
    if (m_output && m_output->isInitialized())
    {
        m_output->uninitialize();
    }

    //display->setTime(0);
    if (m_decoder)
    {
        delete m_decoder;
        m_decoder = 0;
    }
    if (m_input)
    {
        delete m_input;
        m_input = 0;
    }
    // recreate output
    if (m_update && m_output)
    {
        delete m_output;
        m_output = 0;
        m_update = FALSE;
        m_output = Output::create(this);
        if (!m_output)
        {
            qWarning("SoundCore: unable to create output");
        }
        VisualFactory* factory;
        foreach(factory, *Visual::visualFactories())
        {
            if (Visual::isEnabled(factory))
                m_output->addVisual(factory, m_parentWidget);
        }
        connect(m_output, SIGNAL(stateChanged(const OutputState&)),
                SIGNAL(outputStateChanged(const OutputState&)));
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

void SoundCore::seek(int pos)
{
    if (m_output && m_output->isRunning())
    {
        m_output->recycler()->mutex()->lock ();
        m_output->recycler()->clear ();
        m_output->recycler()->mutex()->unlock ();
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

int SoundCore::length()
{
    if (m_decoder)
        return int(m_decoder->lengthInSeconds());
    return 0;
}

bool SoundCore::isInitialized()
{
    if (m_decoder)
        return TRUE;
    return FALSE;
}

bool SoundCore::isPaused()
{
    return m_paused;
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

void SoundCore::updateConfig()
{
    m_update = TRUE;
    if (isInitialized())
        return;
    stop();
}

void SoundCore::addVisualization(Visual *visual)
{
    if (m_visuals.indexOf (visual) == -1)
    {
        m_visuals.append(visual);
        if (m_output)
            m_output->addVisual(visual);
    }
}

bool SoundCore::decode()
{
    if (! m_decoder)
    {
        m_block = TRUE;
        qDebug ("SoundCore: creating decoder");
        m_decoder = Decoder::create(this, m_source, m_input, m_output);

        if (! m_decoder)
        {
            qWarning("SoundCore: unsupported fileformat");
            m_block = FALSE;
            stop();
            emit decoderStateChanged(DecoderState(DecoderState::Error));
            return FALSE;
        }
        qDebug ("ok");
        m_decoder->setBlockSize(globalBlockSize);
        connect(m_decoder, SIGNAL(stateChanged(const DecoderState&)),
                SIGNAL(decoderStateChanged(const DecoderState&)));
        setEQ(m_bands, m_preamp);
        setEQEnabled(m_useEQ);
    }
    qDebug("SoundCore: decoder was created successfully");

    if (m_decoder->initialize())
    {
        m_output->start();
        m_decoder->start();
        m_error = NoError;
        m_block = FALSE;
        return TRUE;
    }
    stop();
    m_block = FALSE;
    return FALSE;
}

void SoundCore::showVisualization(QWidget *parent)
{
    if (!m_parentWidget)
    {
        m_parentWidget = parent;
        if (!m_output)
            return;
        VisualFactory* factory;
        foreach(factory, *Visual::visualFactories())
        {
            if (Visual::isEnabled(factory))
                m_output->addVisual(factory, m_parentWidget);
        }
    }
}

void SoundCore::addVisual(VisualFactory *factory, QWidget *parent)
{
    if (m_output)
        m_output->addVisual(factory, parent);
    else
        Visual::setEnabled(factory, TRUE);
}

void SoundCore::removeVisual(VisualFactory *factory)
{
    if (m_output)
        m_output->removeVisual(factory);
    else
        Visual::setEnabled(factory, FALSE);
}

void SoundCore::removeVisual(Visual *visual)
{
    if (m_visuals.indexOf (visual) != -1)
    {
        m_visuals.removeAll(visual);
        if (m_output)
            m_output->removeVisual(visual);
    }
}

SoundCore* SoundCore::instance()
{
    return m_instance;
}
