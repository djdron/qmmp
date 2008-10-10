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

#include <qmmp/constants.h>
#include <qmmp/buffer.h>
#include <qmmp/output.h>
#include <qmmp/recycler.h>
#include <qmmp/fileinfo.h>
#include <qmmp/decoderfactory.h>

#include <QObject>
#include <QFile>

#include "cueparser.h"
#include "decoder_cue.h"


DecoderCUE::DecoderCUE(QObject *parent, DecoderFactory *d, const QString &url)
        : Decoder(parent, d)
{
    path = url;
    m_decoder = 0;
    m_output2 = 0;
    m_input2 = 0;
}

DecoderCUE::~DecoderCUE()
{}

bool DecoderCUE::initialize()
{
    CUEParser parser(QUrl(path).path());
    int track = path.section("#", -1).toInt();
    path = parser.filePath();
    DecoderFactory *df = Decoder::findByPath(path);
    if (df)
    {
        m_input2 = new QFile(path);
        if (!m_input2->open(QIODevice::ReadOnly))
        {
            qDebug("DecoderCUE: cannot open input");
            stop();
            //m_handler->dispatch(Qmmp::NormalError);
            return FALSE;
        }
    }
    if (!df->properties().noOutput)
    {
        m_output2 = Output::create(this);
        if (!m_output2)
        {
            qWarning("DecoderCUE: unable to create output");
            //m_handler->dispatch(Qmmp::FatalError);
            return FALSE;
        }
        if (!m_output2->initialize())
        {
            qWarning("SoundCore: unable to initialize output");
            delete m_output2;
            m_output2 = 0;
            //m_handler->dispatch(Qmmp::FatalError);
            return FALSE;
        }
    }
    m_length = parser.length(track);
    m_offset = parser.offset(track);
    m_decoder = df->create(this, m_input2, m_output2, path);
    CUEStateHandler *csh = new CUEStateHandler(this, m_offset, m_length);
    m_decoder->setStateHandler(csh);
    connect(csh, SIGNAL(finished()), SLOT(finish()));
    connect(m_decoder, SIGNAL(playbackFinished()), SLOT(finish()));
    if (m_output2)
        m_output2->setStateHandler(m_decoder->stateHandler());
    m_decoder->initialize();
    m_decoder->seek(parser.offset(track));
    if (m_output2)
        m_output2->seek(parser.offset(track));

    //send metadata
    QMap<Qmmp::MetaData, QString> metaData = parser.info(track)->metaData();
    StateHandler::instance()->dispatch(metaData);
    return TRUE;
}

qint64 DecoderCUE::lengthInSeconds()
{
    return m_decoder ? m_length : 0;
}

void DecoderCUE::seek(qint64 pos)
{
    if (m_output2 && m_output2->isRunning())
    {
        m_output2->mutex()->lock ();
        m_output2->seek(m_offset + pos);
        m_output2->mutex()->unlock();
        if (m_decoder && m_decoder->isRunning())
        {
            m_decoder->mutex()->lock ();
            m_decoder->seek(m_offset + pos);
            m_decoder->mutex()->unlock();
        }
    }
    else if (m_decoder)
    {
        m_decoder->mutex()->lock ();
        m_decoder->seek(m_offset + pos);
        m_decoder->mutex()->unlock();
    }
}

void DecoderCUE::stop()
{
    if (m_decoder /*&& m_decoder->isRunning()*/)
    {
        m_decoder->mutex()->lock ();
        m_decoder->stop();
        m_decoder->mutex()->unlock();
        //m_decoder->stateHandler()->dispatch(Qmmp::Stopped);
    }
    if (m_output2)
    {
        m_output2->mutex()->lock ();
        m_output2->stop();
        m_output2->mutex()->unlock();
    }

    // wake up threads
    if (m_decoder)
    {
        m_decoder->mutex()->lock ();
        m_decoder->cond()->wakeAll();
        m_decoder->mutex()->unlock();
    }
    if (m_output2)
    {
        m_output2->recycler()->mutex()->lock ();
        m_output2->recycler()->cond()->wakeAll();
        m_output2->recycler()->mutex()->unlock();
    }
    if (m_decoder)
        m_decoder->wait();
    if (m_output2)
        m_output2->wait();

    if (m_input2)
    {
        m_input2->deleteLater();
        m_input2 = 0;
    }
}

void DecoderCUE::pause()
{
    if (m_output2)
    {
        m_output2->mutex()->lock ();
        m_output2->pause();
        m_output2->mutex()->unlock();
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

    if (m_output2)
    {
        m_output2->recycler()->mutex()->lock ();
        m_output2->recycler()->cond()->wakeAll();
        m_output2->recycler()->mutex()->unlock();
    }
}

void DecoderCUE::run()
{
    m_decoder->start();
    if (m_output2)
        m_output2->start();
}


CUEStateHandler::CUEStateHandler(QObject *parent, qint64 offset, qint64 length): StateHandler(parent)
{
    m_offset = offset;
    m_length2 = length;
}

CUEStateHandler::~CUEStateHandler(){};

void CUEStateHandler::dispatch(qint64 elapsed,
                               qint64 totalTime,
                               int bitrate,
                               int frequency,
                               int precision,
                               int channels)
{
    Q_UNUSED(totalTime);
    StateHandler::instance()->dispatch(elapsed - m_offset, m_length2, bitrate,
                                       frequency, precision, channels);
    if (elapsed - m_offset > m_length2)
        emit finished();
}

void CUEStateHandler::dispatch(const QMap<Qmmp::MetaData, QString> &metaData)
{
    //ignore media file metadata
    Q_UNUSED(metaData)
}

void CUEStateHandler::dispatch(const Qmmp::State &state)
{
    StateHandler::instance()->dispatch(state);
}
