// Copyright (c) 2000-2001 Brad Hughes <bhughes@trolltech.com>
//
// Use, modification and distribution is allowed without limitation,
// warranty, or liability of any kind.
//
#include <QStringList>
#include <QDir>
#include <stdio.h>
#include "audioparameters.h"
#include "qmmpsettings.h"
#include "buffer.h"
#include "volumecontrol_p.h"
#include "qmmp.h"
#include "qmmpplugincache_p.h"
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
QList<QmmpPluginCache*> *Output::m_cache = 0;

void Output::loadPlugins()
{
    if (m_cache)
        return;

    m_cache = new QList<QmmpPluginCache *>;
    QSettings settings (Qmmp::configFile(), QSettings::IniFormat);
    QDir pluginsDir (Qmmp::pluginsPath());
    pluginsDir.cd("Output");
    QStringList filters;
    filters << "*.dll" << "*.so";
    foreach (QString fileName, pluginsDir.entryList(filters, QDir::Files))
    {
        QmmpPluginCache *item = new QmmpPluginCache(pluginsDir.absoluteFilePath(fileName), &settings);
        if(item->hasError())
        {
            delete item;
            continue;
        }
        m_cache->append(item);
    }
}

Output *Output::create ()
{
    loadPlugins();
    Output *output = 0;
    if (m_cache->isEmpty ())
    {
        qDebug("Output: unable to find output plugins");
        return output;
    }
    OutputFactory *fact = Output::currentFactory();
    if (fact)
        output = fact->create ();
    return output;
}

QList<OutputFactory *> Output::factories()
{
    loadPlugins();
    QList<OutputFactory *> list;
    foreach(QmmpPluginCache *item, *m_cache)
    {
        if(item->outputFactory())
            list.append(item->outputFactory());
    }
    return list;
}

QString Output::file(OutputFactory *factory)
{
    loadPlugins();
    foreach(QmmpPluginCache *item, *m_cache)
    {
        if(item->shortName() == factory->properties().shortName)
            return item->file();
    }
    return QString();
}

void Output::setCurrentFactory(OutputFactory* factory)
{
    loadPlugins();
    if (file(factory).isEmpty())
        return;
    QSettings settings (Qmmp::configFile(), QSettings::IniFormat);
    settings.setValue ("Output/current_plugin", factory->properties().shortName);
}

OutputFactory *Output::currentFactory()
{
    loadPlugins();
    QSettings settings (Qmmp::configFile(), QSettings::IniFormat);
#ifdef Q_OS_LINUX
    QString name = settings.value("Output/current_plugin", "alsa").toString();
#elif defined Q_WS_WIN
    QString name = settings.value("Output/current_plugin", "waveout").toString();
#else
    QString name = settings.value("Output/current_plugin", "oss4").toString();
#endif
    foreach(QmmpPluginCache *item, *m_cache)
    {
        if (item->shortName() == name && item->outputFactory())
            return item->outputFactory();
    }
    if (!m_cache->isEmpty())
        return m_cache->at(0)->outputFactory();
    return 0;
}
