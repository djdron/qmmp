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
#include <stdio.h>
#include "audioparameters.h"
#include "qmmpsettings.h"
#include "buffer.h"
#include "volumecontrol_p.h"
#include "qmmp.h"
#include "output.h"

extern "C" {
#include "equ/iir.h"
}

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

Output::Output (QObject* parent) : QThread (parent)
{
    m_handler = StateHandler::instance();
    m_frequency = 0;
    m_channels = 0;
    m_format = Qmmp::PCM_UNKNOWM;
    m_totalWritten = 0;
    m_currentMilliseconds = -1;
    m_bytesPerMillisecond = 0;
    m_userStop = false;
    m_finish = false;
    m_visBuffer = 0;
    m_visBufferSize = 0;
    m_kbps = 0;
    m_skip = false;
    m_pause = false;
    m_prev_pause = false;
    m_useEq = false;
    m_eqEnabled = false;
    m_settings = QmmpSettings::instance();
    connect(m_settings,SIGNAL(eqSettingsChanged()), SLOT(updateEqSettings()));
    updateEqSettings();
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
    qDebug("Output: [%s] %d Hz, %d ch, %s", qPrintable(Output::currentFactory()->properties().shortName),
           freq, chan, qPrintable(formatNames.value(format)));
    m_bytesPerMillisecond = freq * chan * AudioParameters::sampleSize(format) / 1000;
    m_recycler.configure(freq, chan, format); //calculate output buffer size
    //visual buffer
    if(m_visBuffer)
        delete [] m_visBuffer;
    m_visBufferSize = QMMP_BLOCK_FRAMES * 2 * chan; //16-bit samples
    m_visBuffer = new unsigned char [m_visBufferSize];
    m_useEq = m_eqEnabled && m_frequency && m_format == Qmmp::PCM_S16LE;
    if(m_frequency)
        init_iir(m_frequency, m_settings->eqSettings().bands());
}

void Output::pause()
{
    mutex()->lock();
    m_pause = !m_pause;
    mutex()->unlock();
    Qmmp::State state = m_pause ? Qmmp::Paused: Qmmp::Playing;
    dispatch(state);
}

void Output::stop()
{
    m_userStop = true;
}

void Output::finish()
{
    m_finish = true;
}

qint64 Output::written()
{
    return m_totalWritten;
}

void Output::seek(qint64 pos, bool reset)
{
    m_totalWritten = pos * m_bytesPerMillisecond;
    m_currentMilliseconds = -1;
    m_skip = isRunning() && reset;
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

AudioParameters Output::audioParameters() const
{
    return AudioParameters(m_frequency, m_channels, m_format);
}

quint32 Output::sampleRate()
{
    return m_frequency;
}

int Output::channels()
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

qint64 Output::bufferSize() const
{
    return m_recycler.size();
}

void Output::suspend()
{}

void Output::resume()
{}

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

    bool done = false;
    Buffer *b = 0;
    qint64 l, m = 0;

    dispatch(Qmmp::Playing);

    while (!done)
    {
        mutex()->lock ();
        if(m_pause != m_prev_pause)
        {
            if(m_pause)
            {
                suspend();
                mutex()->unlock();
                m_prev_pause = m_pause;
                continue;
            }
            else
                resume();
            m_prev_pause = m_pause;
        }
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
            if (m_useEq)
                iir((void*) b->data, b->nbytes, m_channels);
            dispatchVisual(b);
            if (SoftwareVolume::instance())
                SoftwareVolume::instance()->changeVolume(b, m_channels, m_format);
            l = 0;
            m = 0;
            while (l < b->nbytes && !m_pause)
            {
                mutex()->lock();
                if(m_skip)
                {
                    m_skip = false;
                    reset();
                    mutex()->unlock();
                    break;
                }
                mutex()->unlock();
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
        drain();
#ifdef Q_OS_WIN
        qDebug("Output: total written %I64d", m_totalWritten);
#else
        qDebug("Output: total written %lld", m_totalWritten);
#endif
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

void Output::updateEqSettings()
{
    mutex()->lock();
    m_eqEnabled = m_settings->eqSettings().isEnabled();
    double preamp = m_settings->eqSettings().preamp();
    int bands =  m_settings->eqSettings().bands();

    if(band_count != bands)
        init_iir(m_frequency, bands);

    set_preamp(0, 1.0 + 0.0932471 *preamp + 0.00279033 * preamp * preamp);
    set_preamp(1, 1.0 + 0.0932471 *preamp + 0.00279033 * preamp * preamp);
    for(int i = 0; i < bands; ++i)
    {
        double value =  m_settings->eqSettings().gain(i);
        set_gain(i,0, 0.03*value+0.000999999*value*value);
        set_gain(i,1, 0.03*value+0.000999999*value*value);
    }
    if(isRunning())
        m_useEq = m_eqEnabled && m_format == Qmmp::PCM_S16LE;
    mutex()->unlock();
}

// static methods

QList<OutputFactory*> *Output::m_factories = 0;
QHash <OutputFactory*, QString> *Output::m_files = 0;

void Output::checkFactories()
{
    if (!m_factories)
    {
        m_files = new QHash <OutputFactory*, QString>;
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
                m_factories->append (factory);
                m_files->insert(factory, pluginsDir.absoluteFilePath(fileName));
                qApp->installTranslator(factory->createTranslator(qApp));
            }
        }
    }
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

QString Output::file(OutputFactory *factory)
{
    checkFactories();
    return m_files->value(factory);
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
#elif defined Q_WS_WIN
    QString name = settings.value("Output/current_plugin", "waveout").toString();
#else
    QString name = settings.value("Output/current_plugin", "oss4").toString();
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