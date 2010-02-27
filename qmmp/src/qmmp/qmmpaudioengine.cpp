/***************************************************************************
 *   Copyright (C) 2009-2010 by Ilya Kotov                                 *
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
#include "replaygain.h"
#include "effect.h"
#include "buffer.h"
#include "decoder.h"
#include "output.h"
#include "decoderfactory.h"
#include "effectfactory.h"
#include "inputsource.h"
#include "audioconverter.h"
#include "qmmpaudioengine.h"
#include "metadatamanager.h"

extern "C"
{
#include "equ/iir.h"
}

QmmpAudioEngine::QmmpAudioEngine(QObject *parent)
        : AbstractEngine(parent), m_factory(0), m_output(0), m_eqInited(FALSE),
        m_useEq(FALSE), m_eqEnabled(FALSE)
{
    m_output_buf = new unsigned char[QMMP_BUFFER_SIZE];
    double b[] = {0,0,0,0,0,0,0,0,0,0};
    setEQ(b, 0);
    qRegisterMetaType<Qmmp::State>("Qmmp::State");
    m_effects = Effect::create();
    m_bks = QMMP_BLOCK_SIZE;
    m_decoder = 0;
    m_output = 0;
    m_replayGain = new ReplayGain;
    m_settings = QmmpSettings::instance();
    connect(m_settings,SIGNAL(replayGainSettingsChanged()), SLOT(updateReplayGainSettings()));
    updateReplayGainSettings();
    reset();
    m_instance = this;
}

QmmpAudioEngine::~QmmpAudioEngine()
{
    stop();
    reset();
    if(m_output_buf)
        delete [] m_output_buf;
    m_output_buf = 0;
    qDeleteAll(m_effects);
    m_instance = 0;
    delete m_replayGain;
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
        m_inputs.value(m_decoder)->setOffset(source->offset());
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
    if(!decoder->totalTime())
        source->setOffset(-1);
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
    m_eqEnabled = on;
    if(m_decoder)
        m_useEq = m_eqEnabled && m_decoder->audioParameters().format() == Qmmp::PCM_S16LE;
    mutex()->unlock();
}

void QmmpAudioEngine::addEffect(EffectFactory *factory)
{
    if(m_output && m_output->isRunning())
    {
        Effect *effect = factory->create();
        effect->configure(m_ap.sampleRate(), m_ap.channels(), m_ap.format());
        if(effect->audioParameters() == m_ap)
        {
            mutex()->lock();
            m_effects << effect;
            mutex()->unlock();
        }
        else
        {
            qDebug("QmmpAudioEngine: restart required");
            delete effect;
        }
    }
}

void QmmpAudioEngine::removeEffect(EffectFactory *factory)
{
    Effect *effect = 0;
    foreach(effect, m_effects)
    {
        if(effect->factory() == factory)
            break;
    }
    if(!effect)
        return;
    if(m_output && m_output->isRunning())
    {
        mutex()->lock();
        if(m_blockedEffects.contains(effect))
            qDebug("QmmpAudioEngine: restart required");
        else
            m_effects.removeAll(effect);
        mutex()->unlock();
    }
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
    uint sz = size < m_bks ? size : m_bks;
    m_replayGain->applyReplayGain(data, sz);
    if (m_useEq && m_decoder->audioParameters().format() == Qmmp::PCM_S16LE)
    {
        if (!m_eqInited)
        {
            init_iir();
            m_eqInited = TRUE;
        }
        iir((void*) data, sz, chan);
    }

    Buffer *b = m_output->recycler()->get();

    memcpy(b->data, data, sz);
    b->nbytes = sz;
    b->rate = brate;
    foreach(Effect* effect, m_effects)
    {
        effect->applyEffect(b);
    }
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

void QmmpAudioEngine::updateReplayGainSettings()
{
    mutex()->lock();
    m_replayGain->updateSettings(m_settings->replayGainMode(),
                                 m_settings->replayGainPreamp(),
                                 m_settings->replayGainDefaultGain(),
                                 m_settings->replayGainPreventClipping());
    mutex()->unlock();
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
    addOffset(); //offset
    m_replayGain->setReplayGainInfo(m_decoder->replayGainInfo());
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
            m_output->recycler()->clear();
            m_output->recycler()->mutex()->unlock ();
            m_output_at = 0;
        }

        len = m_decoder->read((char *)(m_output_buf + m_output_at),
                              QMMP_BUFFER_SIZE - m_output_at);

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
                addOffset(); //offset
                mutex()->unlock();
                continue;
            }
            else if(!m_decoders.isEmpty())
            {
                m_inputs.take(m_decoder)->deleteLater ();
                delete m_decoder;
                m_decoder = m_decoders.dequeue();
                //m_seekTime = m_inputs.value(m_decoder)->offset();
                m_replayGain->setReplayGainInfo(m_decoder->replayGainInfo());
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
                    addOffset(); //offset
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
                        addOffset(); //offset
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
            m_output->recycler()->clear();
            m_output->recycler()->mutex()->unlock ();
            m_output_at = 0;
            break;
        }

        while ((m_output->recycler()->full() || m_output->recycler()->blocked()) && (!m_done && !m_finish))
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

void QmmpAudioEngine::addOffset()
{
    qint64 pos = m_inputs.value(m_decoder)->offset();
    if(pos > 0)
    {
        m_seekTime = pos;
        m_output->seek(pos);
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
    m_blockedEffects.clear();
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
    AudioParameters ap = m_ap;
    m_replayGain->setSampleSize(m_ap.sampleSize());
    if(!m_eqInited)
    {
        init_iir();
        m_eqInited = TRUE;
    }
    m_useEq = m_eqEnabled && ap.format() == Qmmp::PCM_S16LE;

    if(m_settings->use16BitOutput())
        m_effects.prepend (new AudioConverter());

    foreach(Effect *effect, m_effects)
    {
        effect->configure(ap.sampleRate(), ap.channels(), ap.format());
        if (ap != effect->audioParameters())
        {
            m_blockedEffects << effect; //list of effects which require restart
            ap = effect->audioParameters();
        }
    }
    m_chan = ap.channels();
    output->configure(ap.sampleRate(), ap.channels(), ap.format());
    return output;
}

//static members
QmmpAudioEngine *QmmpAudioEngine::m_instance = 0;

QmmpAudioEngine *QmmpAudioEngine::instance()
{
    return m_instance;
}
