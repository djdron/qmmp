/***************************************************************************
 *   Copyright (C) 2009 by Ilya Kotov                                      *
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

#include <QMetaType>
#include <QIODevice>
#include <QFile>

#include "effect.h"
#include "buffer.h"
#include "decoder.h"
#include "output.h"
#include "decoderfactory.h"
#include "qmmpaudioengine.h"

extern "C"
{
#include "equ/iir.h"
}

QmmpAudioEngine::QmmpAudioEngine(QObject *parent)
        : AbstractEngine(parent), m_factory(0), m_output(0), m_eqInited(FALSE),
        m_useEQ(FALSE)
{
    m_output_buf = new unsigned char[Qmmp::globalBufferSize()];
    double b[] = {0,0,0,0,0,0,0,0,0,0};
    setEQ(b, 0);
    qRegisterMetaType<Qmmp::State>("Qmmp::State");
    _blksize = Buffer::size();
    m_effects = Effect::create(this);
    m_bks = Buffer::size();
    m_decoder = 0;
    m_output = 0;
    m_decoder2 = 0;
    reset();
}

QmmpAudioEngine::~QmmpAudioEngine()
{
    reset();
    if(m_output_buf)
        delete [] m_output_buf;
    m_output_buf = 0;
}

void QmmpAudioEngine::reset()
{
    m_done = FALSE;
    m_finish = FALSE;
    m_totalTime = 0;
    m_seekTime = -1;
    m_output_at = 0;
    m_user_stop = FALSE;
    m_bitrate = 0;
    m_chan = 0;
    m_bps = 0;
    m_source.clear();
}


bool QmmpAudioEngine::initialize(const QString &source, QIODevice *input)
{
    if(m_decoder && isRunning() && m_output && m_output->isRunning())
    {
        m_factory = Decoder::findByPath(source);
        m_decoder2 = m_factory->create(input, source);
        if(!m_decoder2->initialize())
            return FALSE;
        if(m_decoder2->audioParameters() == m_decoder->audioParameters())
        {
            qDebug("accepted!!");
            m_source = source;
            return TRUE;
        }
        delete m_decoder2;
        m_decoder2 = 0;
    }
    else
        m_decoder2 = 0;
    stop();
    m_source = source;
    m_output = Output::create(this);
    m_output->recycler()->clear();
    if (!m_output)
    {
        qWarning("SoundCore: unable to create output");
        StateHandler::instance()->dispatch(Qmmp::FatalError);
        return FALSE;
    }
    if (!m_output->initialize())
    {
        qWarning("SoundCore: unable to initialize output");
        delete m_output;
        m_output = 0;
        StateHandler::instance()->dispatch(Qmmp::FatalError);
        return FALSE;
    }

    m_factory = Decoder::findByPath(source);
    m_decoder = m_factory->create(input, source);
    if (!m_decoder)
    {
        qWarning("SoundCore: unsupported fileformat");
        stop();
        StateHandler::instance()->dispatch(Qmmp::NormalError);
        return FALSE;
    }
    if(!m_decoder->initialize())
        return FALSE;

    m_output->configure(m_decoder->audioParameters().sampleRate(),
                        m_decoder->audioParameters().channels(),
                        m_decoder->audioParameters().bits());
    return TRUE;
}

qint64 QmmpAudioEngine::totalTime()
{
    if(m_decoder)
        return m_decoder->totalTime();
    else
        return 0;
}

Output *QmmpAudioEngine::output()
{
    return m_output;
}

void QmmpAudioEngine::setEQ(double bands[10], double preamp)
{
    set_preamp(0, 1.0 + 0.0932471 *preamp + 0.00279033 * preamp * preamp);
    set_preamp(1, 1.0 + 0.0932471 *preamp + 0.00279033 * preamp * preamp);
    for (int i=0; i<10; ++i)
    {
        double value = bands[i];
        set_gain(i,0, 0.03*value+0.000999999*value*value);
        set_gain(i,1, 0.03*value+0.000999999*value*value);
    }
}

void QmmpAudioEngine::setEQEnabled(bool on)
{
    m_useEQ = on;
}

/*void QmmpAudioEngine::configure(quint32 srate, int chan, int bps)
{
    Effect* effect = 0;
    m_freq = srate;
    m_chan = chan;
    m_bps = bps;
    foreach(effect, m_effects)
    {
        effect->configure(srate, chan, bps);
        srate = effect->sampleRate();
        chan = effect->channels();
        bps = effect->bitsPerSample();
    }
    m_chan = chan;
    if (m_output)
    {
        m_output->configure(srate, chan, bps);
        if (!m_output_buf)
            m_output_buf = new unsigned char[Qmmp::globalBufferSize()];
    }
}*/

void QmmpAudioEngine::seek(qint64 time)
{
    if (m_output && m_output->isRunning())
    {
        m_output->mutex()->lock ();
        m_output->seek(time);
        m_output->mutex()->unlock();
        if (isRunning())
        {
            mutex()->lock ();
            m_seekTime = time;
            mutex()->unlock();
        }
    }
}

int QmmpAudioEngine::bitrate()
{
    return 0;
}

void QmmpAudioEngine::pause()
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
        mutex()->lock ();
        cond()->wakeAll();
        mutex()->unlock();
    }

    if (m_output)
    {
        m_output->recycler()->mutex()->lock ();
        m_output->recycler()->cond()->wakeAll();
        m_output->recycler()->mutex()->unlock();
    }

}

void QmmpAudioEngine::stop()
{
    mutex()->lock ();
    m_user_stop = TRUE;
    mutex()->unlock();

    if (m_output)
    {
        m_output->mutex()->lock ();
        m_output->stop();
        m_output->mutex()->unlock();
    }

    // wake up threads

    mutex()->lock ();
    cond()->wakeAll();
    mutex()->unlock();

    if (m_output)
    {
        m_output->recycler()->mutex()->lock ();
        m_output->recycler()->cond()->wakeAll();
        m_output->recycler()->mutex()->unlock();
    }

    wait();
    if (m_output)
        m_output->wait();

    if (m_output)
    {
        delete m_output;
        m_output = 0;
    }

    if(m_decoder)
    {
        delete m_decoder;
        m_decoder = 0;
    }
    reset();
}

qint64 QmmpAudioEngine::produceSound(char *data, qint64 size, quint32 brate, int chan)
{
    ulong sz = size < _blksize ? size : _blksize;

    if (m_useEQ)
    {
        if (!m_eqInited)
        {
            init_iir();
            m_eqInited = TRUE;
        }
        iir((void*) data, sz, chan);
    }
    char *out_data = data;
    char *prev_data = data;
    qint64 w = sz;
    Effect* effect = 0;
    foreach(effect, m_effects)
    {
        w = effect->process(prev_data, sz, &out_data);

        if (w <= 0)
        {
            // copy data if plugin can not process it
            w = sz;
            out_data = new char[w];
            memcpy(out_data, prev_data, w);
        }
        if (data != prev_data)
            delete prev_data;
        prev_data = out_data;
    }

    Buffer *b = output()->recycler()->get(w);

    memcpy(b->data, out_data, w);

    if (data != out_data)
        delete out_data;

    if (w < _blksize + b->exceeding)
        memset(b->data + w, 0, _blksize + b->exceeding - w);

    b->nbytes = w;
    b->rate = brate;

    output()->recycler()->add();

    size -= sz;
    memmove(data, data + sz, size);
    return sz;
}

void QmmpAudioEngine::finish()
{
    if (output())
    {
        output()->mutex()->lock ();
        output()->finish();
        output()->mutex()->unlock();
    }
    emit playbackFinished();
}

void QmmpAudioEngine::run()
{
    Q_ASSERT(m_chan == 0);
    Q_ASSERT(!m_output_buf);
    mutex()->lock ();
    if (m_output)
        m_output->start();    
    qint64 len = 0;
    mutex()->unlock();

    sendMetaData();

    while (! m_done && ! m_finish)
    {
        mutex()->lock ();
        // decode

        if (m_seekTime >= 0)
        {
            m_decoder->seek(m_seekTime);
            m_seekTime = -1;
            output()->recycler()->mutex()->lock ();
            while(output()->recycler()->used() > 1)
               output()->recycler()->done();
            output()->recycler()->mutex()->unlock ();
        }

        len = m_decoder->read((char *)(m_output_buf + m_output_at),
                              Qmmp::globalBufferSize() - m_output_at);

        if (len > 0)
        {
            m_bitrate = m_decoder->bitrate();
            m_output_at += len;
            if (output())
                flush();
        }
        else if (len == 0)
        {
            qDebug("0");
            if(m_decoder2)
            {
                qDebug("next decoder");
                delete m_decoder;
                m_decoder = m_decoder2;
                emit playbackFinished();
                StateHandler::instance()->dispatch(Qmmp::Stopped); //fake stop/start cycle
                StateHandler::instance()->dispatch(Qmmp::Buffering);
                StateHandler::instance()->dispatch(Qmmp::Playing);
                m_output->seek(0); //reset counter
                mutex()->unlock();
                sendMetaData();
                continue;
            }

            flush(TRUE);
            if (output())
            {
                output()->recycler()->mutex()->lock ();
                // end of stream
                while (!output()->recycler()->empty() && !m_user_stop)
                {
                    output()->recycler()->cond()->wakeOne();
                    mutex()->unlock();
                    output()->recycler()->cond()->wait(output()->recycler()->mutex());
                    mutex()->lock ();
                }
                output()->recycler()->mutex()->unlock();
            }
            m_done = TRUE;
            m_finish = !m_user_stop;
        }
        else
            m_finish = TRUE;
        mutex()->unlock();
    }

    mutex()->lock ();
    if (m_finish)
        finish();
    mutex()->unlock();
}

void QmmpAudioEngine::flush(bool final)
{
    ulong min = final ? 0 : m_bks;

    while ((!m_done && !m_finish) && m_output_at > min)
    {
        output()->recycler()->mutex()->lock ();
        if(m_seekTime >= 0)
        {
            while(output()->recycler()->used() > 1)
                output()->recycler()->done();
            output()->recycler()->mutex()->unlock ();
            m_output_at = 0;
            break;
        }
        while ((!m_done && !m_finish) && output()->recycler()->full())
        {
            mutex()->unlock();
            output()->recycler()->cond()->wait(output()->recycler()->mutex());
            mutex()->lock ();
            m_done = m_user_stop;
        }

        if (m_user_stop || m_finish)
            m_done = TRUE;
        else
        {
            m_output_at -= produceSound((char*)m_output_buf, m_output_at, m_bitrate, m_chan);
        }

        if (output()->recycler()->full())
        {
            output()->recycler()->cond()->wakeOne();
        }

        output()->recycler()->mutex()->unlock();
    }
}

void QmmpAudioEngine::sendMetaData()
{
    if (QFile::exists(m_source)) //send metadata for local files
    {
        QList <FileInfo *> list = m_factory->createPlayList(m_source, TRUE);
        if (!list.isEmpty())
        {
            StateHandler::instance()->dispatch(list[0]->metaData());
            while (!list.isEmpty())
                delete list.takeFirst();
        }
    }
}
