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
#include "inputsource.h"
#include "qmmpaudioengine.h"
#include "metadatamanager.h"


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
    m_effects = Effect::create();
    m_bks = Buffer::size();
    m_decoder = 0;
    m_output = 0;
    reset();
}

QmmpAudioEngine::~QmmpAudioEngine()
{
    stop();
    reset();
    if(m_output_buf)
        delete [] m_output_buf;
    m_output_buf = 0;
    qDeleteAll(m_effects);
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
    m_next = FALSE;
}

bool QmmpAudioEngine::play()
{
    if(isRunning() || m_decoders.isEmpty() || (m_output && m_output->isRunning()))
        return FALSE;
    if(m_output)
        delete m_output;
    if(!(m_output = createOutput(m_decoders.head())))
        return FALSE;
    start();
    return TRUE;
}

bool QmmpAudioEngine::enqueue(InputSource *source)
{
    mutex()->lock();
    if(m_decoder && m_decoder->nextURL() == source->url())
    {
        delete source;
        m_next = TRUE;
        mutex()->unlock();
        return TRUE;
    }
    mutex()->unlock();

    DecoderFactory *factory = 0;

    if(!source->url().contains("://"))
        factory = Decoder::findByPath(source->url());
    if(!factory && source->ioDevice())
        factory = Decoder::findByContent(source->ioDevice());
    //TODO mimetype
    if(!factory && source->url().contains("://"))
        factory = Decoder::findByProtocol(source->url().section("://",0,0));
    if(!factory)
    {
        qWarning("QmmpAudioEngine: unsupported file format");
        return FALSE;
    }
    qDebug("QmmpAudioEngine: selected decoder: %s",qPrintable(factory->properties().shortName));
    if(factory->properties().noInput && source->ioDevice())
        source->ioDevice()->close();
    Decoder *decoder = factory->create(source->url(), source->ioDevice());
    if(!decoder->initialize())
    {
        qWarning("QmmpAudioEngine: invalid file format");
        delete decoder;
        return FALSE;
    }
    m_decoders.enqueue(decoder);
    m_inputs.insert(decoder, source);
    source->setParent(this);
    return TRUE;
}

qint64 QmmpAudioEngine::totalTime()
{
    if(m_decoder)
        return m_decoder->totalTime();
    else
        return 0;
}

void QmmpAudioEngine::setEQ(double bands[10], double preamp)
{
    mutex()->lock();
    set_preamp(0, 1.0 + 0.0932471 *preamp + 0.00279033 * preamp * preamp);
    set_preamp(1, 1.0 + 0.0932471 *preamp + 0.00279033 * preamp * preamp);
    for (int i=0; i<10; ++i)
    {
        double value = bands[i];
        set_gain(i,0, 0.03*value+0.000999999*value*value);
        set_gain(i,1, 0.03*value+0.000999999*value*value);
    }
    mutex()->unlock();
}

void QmmpAudioEngine::setEQEnabled(bool on)
{
    mutex()->lock();
    m_useEQ = on;
    mutex()->unlock();
}

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

    while(!m_decoders.isEmpty())
    {
        Decoder *d = m_decoders.dequeue();
        m_inputs.take(d)->deleteLater ();
        delete d;
    }
    reset();
    m_decoder = 0;
    while(!m_effects.isEmpty()) //delete effects
        delete m_effects.takeFirst();
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

    Buffer *b = m_output->recycler()->get(w);

    memcpy(b->data, out_data, w);

    if (data != out_data)
        delete out_data;

    if (w < _blksize + b->exceeding)
        memset(b->data + w, 0, _blksize + b->exceeding - w);

    b->nbytes = w;
    b->rate = brate;

    m_output->recycler()->add();

    size -= sz;
    memmove(data, data + sz, size);
    return sz;
}

void QmmpAudioEngine::finish()
{
    if (m_output)
    {
        m_output->mutex()->lock ();
        m_output->finish();
        m_output->mutex()->unlock();
    }
    emit playbackFinished();
}

void QmmpAudioEngine::run()
{
    Q_ASSERT(m_chan == 0);
    Q_ASSERT(!m_output_buf);
    mutex()->lock ();
    m_next = FALSE;
    qint64 len = 0;
    if(m_decoders.isEmpty())
    {
         mutex()->unlock ();
         return;
    }

    m_decoder = m_decoders.dequeue();
    mutex()->unlock();
    m_output->start();
    sendMetaData();

    while (! m_done && ! m_finish)
    {
        mutex()->lock ();
        // decode

        if (m_seekTime >= 0)
        {
            m_decoder->seek(m_seekTime);
            m_seekTime = -1;
            m_output->recycler()->mutex()->lock ();
            while(m_output->recycler()->used() > 1)
                m_output->recycler()->done();
            m_output->recycler()->mutex()->unlock ();
        }

        len = m_decoder->read((char *)(m_output_buf + m_output_at),
                              Qmmp::globalBufferSize() - m_output_at);

        if (len > 0)
        {
            m_bitrate = m_decoder->bitrate();
            m_output_at += len;
            if (m_output)
                flush();
        }
        else if (len == 0)
        {
            if(m_next) //decoder can play next track without initialization
            {
                m_next = FALSE;
                qDebug("QmmpAudioEngine: switching to the next track");
                emit playbackFinished();
                StateHandler::instance()->dispatch(Qmmp::Stopped); //fake stop/start cycle
                StateHandler::instance()->dispatch(Qmmp::Buffering);
                StateHandler::instance()->dispatch(Qmmp::Playing);
                m_decoder->next();
                m_output->seek(0); //reset counter
                mutex()->unlock();
                continue;
            }
            else if(!m_decoders.isEmpty())
            {
                m_inputs.take(m_decoder)->deleteLater ();
                delete m_decoder;
                m_decoder = m_decoders.dequeue();
                //use current output if possible
                if(m_decoder->audioParameters() == m_ap)
                {
                    emit playbackFinished();
                    StateHandler::instance()->dispatch(Qmmp::Stopped); //fake stop/start cycle
                    StateHandler::instance()->dispatch(Qmmp::Buffering);
                    StateHandler::instance()->dispatch(Qmmp::Playing);
                    m_output->seek(0); //reset counter
                    mutex()->unlock();
                    sendMetaData();
                    continue;
                }
                else
                {
                    flush(TRUE);
                    finish();
                    //wake up waiting threads
                    cond()->wakeAll();
                    mutex()->unlock();
                    m_output->recycler()->mutex()->lock ();
                    m_output->recycler()->cond()->wakeAll();
                    m_output->recycler()->mutex()->unlock();

                    m_output->wait();
                    delete m_output;
                    m_output = createOutput(m_decoder);
                    if(m_output)
                    {
                        m_output->start();
                        sendMetaData();
                        continue;
                    }
                }
            }

            flush(TRUE);
            if (m_output)
            {
                m_output->recycler()->mutex()->lock ();
                // end of stream
                while (!m_output->recycler()->empty() && !m_user_stop)
                {
                    m_output->recycler()->cond()->wakeOne();
                    mutex()->unlock();
                    m_output->recycler()->cond()->wait(m_output->recycler()->mutex());
                    mutex()->lock ();
                }
                m_output->recycler()->mutex()->unlock();
            }
            m_done = TRUE;
            m_finish = !m_user_stop;
        }
        else
            m_finish = TRUE;
        mutex()->unlock();
    }
    if(m_decoder)
    {
        m_inputs.take(m_decoder)->deleteLater ();
        delete m_decoder;
        m_decoder = 0;
    }

    mutex()->lock ();
    m_next = FALSE;
    if (m_finish)
        finish();
    m_output->recycler()->cond()->wakeAll();
    mutex()->unlock();
}

void QmmpAudioEngine::flush(bool final)
{
    ulong min = final ? 0 : m_bks;

    while ((!m_done && !m_finish) && m_output_at > min)
    {
        m_output->recycler()->mutex()->lock ();
        if(m_seekTime >= 0)
        {
            while(m_output->recycler()->used() > 1)
                m_output->recycler()->done();
            m_output->recycler()->mutex()->unlock ();
            m_output_at = 0;
            break;
        }

        while ((!m_done && !m_finish) && m_output->recycler()->full())
        {
            mutex()->unlock();
            m_output->recycler()->cond()->wait(m_output->recycler()->mutex());
            mutex()->lock ();
            m_done = m_user_stop;
        }

        if (m_user_stop || m_finish)
            m_done = TRUE;
        else
        {
            m_output_at -= produceSound((char*)m_output_buf, m_output_at, m_bitrate, m_chan);
        }

        if (m_output->recycler()->full())
        {
            m_output->recycler()->cond()->wakeOne();
        }

        m_output->recycler()->mutex()->unlock();
    }
}

void QmmpAudioEngine::sendMetaData()
{
    if(!m_decoder || m_inputs.isEmpty())
        return;
    QString url = m_inputs.value(m_decoder)->url();
    if (QFile::exists(url)) //send metadata for local files only
    {
        QList <FileInfo *> list = MetaDataManager::instance()->createPlayList(url, TRUE);
        if (!list.isEmpty())
        {
            StateHandler::instance()->dispatch(list[0]->metaData());
            while (!list.isEmpty())
                delete list.takeFirst();
        }
    }
}

Output *QmmpAudioEngine::createOutput(Decoder *d)
{
    while(!m_effects.isEmpty()) //delete effects
        delete m_effects.takeFirst();

    m_ap = d->audioParameters();
    Output *output = Output::create(0);
    if(!output)
    {
        qWarning("QmmpAudioEngine: unable to create output");
        StateHandler::instance()->dispatch(Qmmp::FatalError);
        return 0;
    }
    if (!output->initialize())
    {
        qWarning("QmmpAudioEngine: unable to initialize output");
        delete output;
        output = 0;
        StateHandler::instance()->dispatch(Qmmp::FatalError);
        return FALSE;
    }
    m_effects = Effect::create();
    quint32 srate = m_ap.sampleRate();
    int chan = m_ap.channels();
    int bps = m_ap.bits();

    foreach(Effect *effect, m_effects)
    {
        effect->configure(srate, chan, bps);
        srate = effect->sampleRate();
        chan = effect->channels();
        bps = effect->bitsPerSample();
    }
    m_chan = chan;
    output->configure(srate, chan, bps);
    return output;
}
