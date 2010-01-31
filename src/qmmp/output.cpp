// Copyright (c) 2000-2001 Brad Hughes <bhughes@trolltech.com>
//
// Use, modification and distribution is allowed without limitation,
// warranty, or liability of any kind.
//

#include <QtGui>
#include <QObject>
#include <QStringList>
#include <QApplication>
#include <QTimer>
#include "audioparameters.h"
#include "buffer.h"
#include "output.h"
#include "volumecontrol.h"
#include "qmmp.h"

#include <stdio.h>

//static functions
static inline void s8_to_s16(qint8 *in, qint16 *out, qint64 samples)
{
    for(qint64 i = 0; i < samples; ++i)
        out[i] = in[i] << 8;
    return;
}

static inline void s24_to_s16(qint32 *in, qint16 *out, qint64 samples)
{
    for(qint64 i = 0; i < samples; ++i)
        out[i] = in[i] >> 8;
    return;
}

static inline void s32_to_s16(qint32 *in, qint16 *out, qint64 samples)
{
    for(qint64 i = 0; i < samples; ++i)
        out[i] = in[i] >> 16;
    return;
}

Output::Output (QObject* parent) : QThread (parent), m_recycler (stackSize())
{
    m_handler = StateHandler::instance();
    m_frequency = 0;
    m_channels = 0;
    m_format = Qmmp::PCM_UNKNOWM;
    m_totalWritten = 0;
    m_currentMilliseconds = -1;
    m_bytesPerMillisecond = 0;
    m_userStop = FALSE;
    m_pause = FALSE;
    m_finish = FALSE;
    m_visBuffer = 0;
    m_visBufferSize = 0;
    m_kbps = 0;
}

void Output::configure(quint32 freq, int chan, Qmmp::AudioFormat format)
{
    m_frequency = freq;
    m_channels = chan;
    m_format = format;
    QMap<Qmmp::AudioFormat, QString> formatNames;
    formatNames.insert(Qmmp::PCM_S8, "s8");
    formatNames.insert(Qmmp::PCM_S16LE, "s16le");
    formatNames.insert(Qmmp::PCM_S24LE, "s24le");
    formatNames.insert(Qmmp::PCM_S32LE, "s32le");
    qDebug("Output: %d Hz, %d ch, %s", freq, chan, qPrintable(formatNames.value(format)));
    m_bytesPerMillisecond = freq * chan * AudioParameters::sampleSize(format) / 1000;
    //visual buffer
    if(m_visBuffer)
    {
        delete [] m_visBuffer;
        m_visBuffer = 0;
        m_visBufferSize = 0;
    }
    if(format == Qmmp::PCM_S8)
    {
        m_visBufferSize = QMMP_BLOCK_SIZE * 2;
        m_visBuffer = new unsigned char [m_visBufferSize];
    }
    else if(format == Qmmp::PCM_S24LE || format == Qmmp::PCM_S32LE)
    {
        m_visBufferSize = QMMP_BLOCK_SIZE / 2;
        m_visBuffer = new unsigned char [m_visBufferSize];
    }
}

void Output::pause()
{
    m_pause = !m_pause;
    Qmmp::State state = m_pause ? Qmmp::Paused: Qmmp::Playing;
    dispatch(state);
}

void Output::stop()
{
    m_userStop = TRUE;
}

void Output::finish()
{
    m_finish = TRUE;
}

qint64 Output::written()
{
    return m_totalWritten;
}

void Output::seek(qint64 pos)
{
    m_totalWritten = pos * m_bytesPerMillisecond;
    m_currentMilliseconds = -1;
}

Recycler *Output::recycler()
{
    return &m_recycler;
}

QMutex *Output::mutex()
{
    return &m_mutex;
}

void Output::setStateHandler(StateHandler *handler)
{
    m_handler = handler;
}

quint32 Output::sampleRate()
{
    return m_frequency;
}

int Output::numChannels()
{
    return m_channels;
}

Qmmp::AudioFormat Output::format() const
{
    return m_format;
}

int Output::sampleSize() const
{
    return AudioParameters::sampleSize(m_format);
}

Output::~Output()
{}

void Output::dispatchVisual (Buffer *buffer)
{
    if (!buffer)
        return;

    int sampleSize = AudioParameters::sampleSize(m_format);
    int samples = buffer->nbytes/sampleSize;
    int outSize = samples*2;
    if((m_format != Qmmp::PCM_S16LE) && outSize > m_visBufferSize) //increase buffer size
    {
        delete[] m_visBuffer;
        m_visBufferSize = outSize;
        m_visBuffer = new unsigned char [m_visBufferSize];
    }
    switch(m_format)
    {
    case Qmmp::PCM_S8:
        s8_to_s16((qint8 *)buffer->data, (qint16 *) m_visBuffer, samples);
        break;
    case Qmmp::PCM_S16LE:
        m_visBuffer = buffer->data;
        outSize = buffer->nbytes;
        break;
    case Qmmp::PCM_S24LE:
        s24_to_s16((qint32 *)buffer->data, (qint16 *) m_visBuffer, samples);
        break;
    case Qmmp::PCM_S32LE:
        s32_to_s16((qint32 *)buffer->data, (qint16 *) m_visBuffer, samples);
        break;
    default:
        return;
    }
    foreach (Visual *visual, *Visual::visuals())
    {
        visual->mutex()->lock ();
        visual->add (m_visBuffer, outSize, m_channels);
        visual->mutex()->unlock();
    }
    if(m_format == Qmmp::PCM_S16LE)
        m_visBuffer = 0;
}


void Output::clearVisuals()
{
    foreach (Visual *visual, *Visual::visuals())
    {
        visual->mutex()->lock ();
        visual->clear ();
        visual->mutex()->unlock();
    }
}

void Output::dispatch(qint64 elapsed,
                      int bitrate,
                      int frequency,
                      int precision,
                      int channels)
{
    if (m_handler)
        m_handler->dispatch(elapsed, bitrate, frequency, precision, channels);
}

void Output::dispatch(const Qmmp::State &state)
{
    if (m_handler)
        m_handler->dispatch(state);
    if (state == Qmmp::Stopped)
        clearVisuals();
}

void Output::run()
{
    mutex()->lock ();
    if (!m_bytesPerMillisecond)
    {
        qWarning("Output: invalid audio parameters");
        mutex()->unlock ();
        return;
    }
    mutex()->unlock ();

    bool done = FALSE;
    Buffer *b = 0;
    qint64 l, m = 0;

    dispatch(Qmmp::Playing);

    while (!done)
    {
        mutex()->lock ();
        recycler()->mutex()->lock ();
        done = m_userStop || (m_finish && recycler()->empty());

        while (!done && (recycler()->empty() || m_pause))
        {
            mutex()->unlock();
            recycler()->cond()->wakeOne();
            recycler()->cond()->wait(recycler()->mutex());
            mutex()->lock ();
            done = m_userStop || m_finish;
        }
        status();
        if (!b)
        {
            b = recycler()->next();
            if (b && b->rate)
                m_kbps = b->rate;
        }

        recycler()->cond()->wakeOne();
        recycler()->mutex()->unlock();
        mutex()->unlock();
        if (b)
        {
            dispatchVisual(b);
            if (SoftwareVolume::instance())
                SoftwareVolume::instance()->changeVolume(b, m_channels, m_format);
            l = 0;
            m = 0;
            while (l < b->nbytes)
            {
                m = writeAudio(b->data + l, b->nbytes - l);
                if(m >= 0)
                {
                    m_totalWritten += m;
                    l+= m;
                }
                else
                    break;
            }
            if(m < 0)
                break;
        }
        mutex()->lock();
        //force buffer change
        recycler()->mutex()->lock ();
        recycler()->done();
        recycler()->mutex()->unlock();
        b = 0;
        mutex()->unlock();
    }
    mutex()->lock ();
    //write remaining data
    if(m_finish)
    {
        flush();
        qDebug("Output: total written %lld", m_totalWritten);
    }
    dispatch(Qmmp::Stopped);
    mutex()->unlock();
}

void Output::status()
{
    qint64 ct = m_totalWritten / m_bytesPerMillisecond - latency();

    if (ct < 0)
        ct = 0;

    if (ct > m_currentMilliseconds)
    {
        m_currentMilliseconds = ct;
        dispatch(m_currentMilliseconds, m_kbps,
                 m_frequency, AudioParameters::sampleSize(m_format)*8, m_channels);
    }
}

// static methods

QList<OutputFactory*> *Output::m_factories = 0;
QStringList Output::m_files;
QTimer *Output::m_timer = 0;

void Output::checkFactories()
{
    if (!m_factories)
    {
        m_files.clear();
        m_factories = new QList<OutputFactory *>;

        QDir pluginsDir (Qmmp::pluginsPath());
        pluginsDir.cd("Output");
        foreach ( QString fileName, pluginsDir.entryList ( QDir::Files ) )
        {
            QPluginLoader loader ( pluginsDir.absoluteFilePath ( fileName ) );
            QObject *plugin = loader.instance();
            if ( loader.isLoaded() )
                qDebug ( "Output: plugin loaded - %s", qPrintable ( fileName ) );
            else
                qWarning("Output: %s", qPrintable(loader.errorString ()));

            OutputFactory *factory = 0;
            if (plugin)
                factory = qobject_cast<OutputFactory *> ( plugin );

            if (factory)
            {
                Output::registerFactory ( factory );
                m_files << pluginsDir.absoluteFilePath(fileName);
                qApp->installTranslator(factory->createTranslator(qApp));
            }
        }
    }
}

void Output::registerFactory (OutputFactory *fact)
{
    m_factories->append (fact);
}

Output *Output::create (QObject *parent)
{
    Output *output = 0;

    checkFactories();
    if (m_factories->isEmpty ())
    {
        qDebug("Output: unable to find output plugins");
        return output;
    }
    OutputFactory *fact = Output::currentFactory();
    if (!fact && !m_factories->isEmpty())
        fact = m_factories->at(0);
    if (fact)
        output = fact->create (parent);
    return output;
}

QList<OutputFactory*> *Output::factories()
{
    checkFactories();
    return m_factories;
}

QStringList Output::files()
{
    checkFactories();
    return m_files;
}

void Output::setCurrentFactory(OutputFactory* factory)
{
    checkFactories();
    if (!m_factories->contains(factory))
        return;
    QSettings settings (Qmmp::configFile(), QSettings::IniFormat);
    settings.setValue ("Output/current_plugin", factory->properties().shortName);
}

OutputFactory *Output::currentFactory()
{
    checkFactories();
    QSettings settings (Qmmp::configFile(), QSettings::IniFormat);
#ifdef Q_OS_LINUX
    QString name = settings.value("Output/current_plugin", "alsa").toString();
#else
    QString name = settings.value("Output/current_plugin", "oss").toString();
#endif
    foreach(OutputFactory *factory, *m_factories)
    {
        if (factory->properties().shortName == name)
            return factory;
    }
    if (!m_factories->isEmpty())
        return m_factories->at(0);
    return 0;
}
