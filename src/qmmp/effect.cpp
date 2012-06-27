/***************************************************************************
 *   Copyright (C) 2007-2009 by Ilya Kotov                                 *
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
#include <QtGui>
#include <QStringList>
#include <QDir>
#include <QApplication>
#include <QHash>
#include "qmmpaudioengine_p.h"
#include "qmmp.h"
#include "effectfactory.h"
#include "effect.h"

Effect::Effect()
{
    m_freq = 0;
    m_chan = 0;
    m_format = Qmmp::PCM_UNKNOWM;
    m_factory = 0;
}

Effect::~Effect()
{}

void Effect::configure(quint32 freq, int chan, Qmmp::AudioFormat format)
{
    m_freq = freq;
    m_chan = chan;
    m_format = format;
}

quint32 Effect::sampleRate()
{
    return m_freq;
}

int Effect::channels()
{
    return m_chan;
}

Qmmp::AudioFormat Effect::format()
{
    return m_format;
}

const AudioParameters Effect::audioParameters() const
{
    return AudioParameters(m_freq, m_chan, m_format);
}

EffectFactory* Effect::factory() const
{
    return m_factory;
}

bool effectCompareFunc(EffectFactory *e1, EffectFactory *e2)
{
    return e1->properties().priority > e2->properties().priority;
}

//static members

QList<EffectFactory*> *Effect::m_factories = 0;
QHash <EffectFactory*, QString> *Effect::m_files = 0;

void Effect::checkFactories()
{
    if (!m_factories)
    {
        m_factories = new QList<EffectFactory *>;
        m_files = new QHash <EffectFactory*, QString>;

        QDir pluginsDir (Qmmp::pluginsPath());
        pluginsDir.cd("Effect");

        foreach (QString fileName, pluginsDir.entryList(QDir::Files))
        {
            QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
            QObject *plugin = loader.instance();
            if (loader.isLoaded())
                qDebug("Effect: plugin loaded - %s", qPrintable(fileName));
            else
                qWarning("Effect: %s", qPrintable(loader.errorString ()));

            EffectFactory *factory = 0;
            if (plugin)
                factory = qobject_cast<EffectFactory *>(plugin);

            if (factory)
            {
                m_factories->append(factory);
                m_files->insert(factory, pluginsDir.absoluteFilePath(fileName));
                qApp->installTranslator(factory->createTranslator(qApp));
            }
        }
        qSort(m_factories->begin(), m_factories->end(), effectCompareFunc);
    }
}

Effect* Effect::create(EffectFactory *factory)
{
    checkFactories();
    Effect *effect = factory->create();
    effect->m_factory = factory;
    return effect;
}

QList<EffectFactory*> *Effect::factories()
{
    checkFactories();
    return m_factories;
}

QString Effect::file(EffectFactory *factory)
{
    checkFactories();
    return m_files->value(factory);
}

void Effect::setEnabled(EffectFactory* factory, bool enable)
{
    checkFactories();
    if(!m_factories->contains(factory))
        return;

    QString name = factory->properties().shortName;
    QSettings settings (Qmmp::configFile(), QSettings::IniFormat);
    QStringList effList = settings.value("Effect/enabled_plugins").toStringList();

    if(enable)
    {
        if (!effList.contains(name))
        {
            effList << name;
            if(QmmpAudioEngine::instance())
                QmmpAudioEngine::instance()->addEffect(factory);
        }
    }
    else if (effList.contains(name))
    {
        effList.removeAll(name);
        if(QmmpAudioEngine::instance())
            QmmpAudioEngine::instance()->removeEffect(factory);
    }
    settings.setValue("Effect/enabled_plugins", effList);
}

bool Effect::isEnabled(EffectFactory* factory)
{
    checkFactories();
    if(!m_factories->contains(factory))
        return false;
    QString name = factory->properties().shortName;
    QSettings settings (Qmmp::configFile(), QSettings::IniFormat);
    QStringList effList = settings.value("Effect/enabled_plugins").toStringList();
    return effList.contains(name);
}
