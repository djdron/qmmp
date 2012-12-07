// Copyright (c) 2000-2001 Brad Hughes <bhughes@trolltech.com>
//
// Use, modification and distribution is allowed without limitation,
// warranty, or liability of any kind.
//

#include <QtGui>
#include <QObject>
#include <QStringList>
#include <QApplication>
#include <stdio.h>
#include "audioparameters.h"
#include "qmmpsettings.h"
#include "buffer.h"
#include "volumecontrol_p.h"
#include "qmmp.h"
#include "output.h"

Output::Output()
{
    m_frequency = 0;
    m_channels = 0;
    m_format = Qmmp::PCM_UNKNOWM;
}

void Output::configure(quint32 freq, int chan, Qmmp::AudioFormat format)
{
    m_frequency = freq;
    m_channels = chan;
    m_format = format;
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

void Output::suspend()
{}

void Output::resume()
{}

Output::~Output()
{}

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
                qDebug ( "Output: loaded plugin %s", qPrintable ( fileName ) );
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

Output *Output::create ()
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
        output = fact->create ();
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
