// Copyright (c) 2000-2001 Brad Hughes <bhughes@trolltech.com>
//
// Use, modification and distribution is allowed without limitation,
// warranty, or liability of any kind.
//
#include <QtGui>
#include <QObject>
#include <QStringList>
#include <QApplication>
#include <QSettings>
#include <QIODevice>
#include <math.h>

#include "effect.h"
#include "effectfactory.h"

#include "buffer.h"
#include "output.h"
#include "visual.h"
#include "decoderfactory.h"
#include "qmmp.h"

extern "C"
{
#include "equ/iir.h"
}
#include "decoder.h"

Decoder::Decoder(QIODevice *input) : m_input(input)
{}

Decoder::~Decoder()
{}

void Decoder::setReplayGainInfo(const QMap<Qmmp::ReplayGainKey, double> &rg)
{
    m_rg = rg;
}

void Decoder::configure(quint32 srate, int chan, Qmmp::AudioFormat format)
{
    m_parameters = AudioParameters(srate, chan, format);
}

void Decoder::next()
{}

const QString Decoder::nextURL()
{
    return QString();
}

AudioParameters Decoder::audioParameters() const
{
    return m_parameters;
}

QMap<Qmmp::ReplayGainKey, double> Decoder::replayGainInfo() const
{
    return m_rg;
}

QIODevice *Decoder::input()
{
    return m_input;
}

// static methods
QList<DecoderFactory*> *Decoder::m_factories = 0;
QList<DecoderFactory*> *Decoder::m_disabledFactories = 0;
DecoderFactory *Decoder::m_lastFactory = 0;
QStringList Decoder::m_files;

void Decoder::checkFactories()
{
    if (!m_factories)
    {
        QSettings settings (Qmmp::configFile(), QSettings::IniFormat);
        QStringList disabledNames  = settings.value("Decoder/disabled_plugins").toStringList ();

        m_files.clear();
        m_factories = new QList<DecoderFactory *>;
        m_disabledFactories = new QList<DecoderFactory *>;

        QDir pluginsDir (Qmmp::pluginsPath());
        pluginsDir.cd("Input");
        foreach (QString fileName, pluginsDir.entryList(QDir::Files))
        {
            QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
            QObject *plugin = loader.instance();
            if (loader.isLoaded())
                qDebug("Decoder: plugin loaded - %s", qPrintable(fileName));
            else
                qWarning("Decoder: %s", qPrintable(loader.errorString ()));
            DecoderFactory *factory = 0;
            if (plugin)
                factory = qobject_cast<DecoderFactory *>(plugin);

            if (factory)
            {
                m_factories->append(factory);
                m_files << pluginsDir.absoluteFilePath(fileName);
                qApp->installTranslator(factory->createTranslator(qApp));
                if(disabledNames.contains(factory->properties().shortName))
                    m_disabledFactories->append(factory);
            }
        }
        //remove physically deleted plugins from disabled list
        QStringList names;
        foreach (DecoderFactory *factory, *m_factories)
        {
            names.append(factory->properties().shortName);
        }
        foreach (QString name, disabledNames)
        {
            if (!names.contains(name))
                disabledNames.removeAll(name);
        }
        settings.setValue("Decoder/disabled_plugins",disabledNames);
    }
}

QStringList Decoder::files()
{
    checkFactories();
    return m_files;
}

QStringList Decoder::protocols()
{
    QStringList protocolsList;
    foreach(DecoderFactory *f, *m_factories)
    {
        if(isEnabled(f))
            protocolsList << f->properties().protocols;
    }
    protocolsList.removeDuplicates();
    return protocolsList;
}

DecoderFactory *Decoder::findByPath(const QString& source)
{
    checkFactories();
    DecoderFactory *fact = m_lastFactory;
    if (fact && fact->supports(source) && isEnabled(fact)) //try last factory
        return fact;
    foreach(fact, *m_factories)
    {
        if (fact->supports(source) && isEnabled(fact))
        {
            m_lastFactory = fact;
            return fact;
        }
    }
    return 0;
}

DecoderFactory *Decoder::findByMime(const QString& type)
{
    if(type.isEmpty())
        return 0;
    checkFactories();
    DecoderFactory *fact;
    foreach(fact, *m_factories)
    {
        if (isEnabled(fact) && fact->properties().contentTypes.contains(type))
            return fact;
    }
    return 0;
}

DecoderFactory *Decoder::findByContent(QIODevice *input)
{
    checkFactories();
    foreach(DecoderFactory *fact, *m_factories)
    {
        if (isEnabled(fact) && fact->canDecode(input))
        {
            return fact;
        }
    }
    return 0;
}

DecoderFactory *Decoder::findByProtocol(const QString &p)
{
    checkFactories();
    foreach(DecoderFactory *fact, *m_factories)
    {
        if (isEnabled(fact) && fact->properties().protocols.contains(p))
            return fact;
    }
    return 0;
}

void Decoder::setEnabled(DecoderFactory* factory, bool enable)
{
    checkFactories();
    if (!m_factories->contains(factory))
        return;

    if(enable == isEnabled(factory))
        return;

    if(enable)
        m_disabledFactories->removeAll(factory);
    else
        m_disabledFactories->append(factory);

    QStringList disabledNames;
    foreach(DecoderFactory *f, *m_disabledFactories)
    {
        disabledNames << f->properties().shortName;
    }
    disabledNames.removeDuplicates();
    QSettings settings (Qmmp::configFile(), QSettings::IniFormat);
    settings.setValue("Decoder/disabled_plugins", disabledNames);
}

bool Decoder::isEnabled(DecoderFactory* factory)
{
    checkFactories();
    return !m_disabledFactories->contains(factory);
}

QList<DecoderFactory*> *Decoder::factories()
{
    checkFactories();
    return m_factories;
}
