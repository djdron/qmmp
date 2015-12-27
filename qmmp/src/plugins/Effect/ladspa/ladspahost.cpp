/***************************************************************************
 *   Copyright (C) 2002-2003 Nick Lamb <njl195@zepler.org.uk>              *
 *   Copyright (C) 2005 Giacomo Lozito <city_hunter@users.sf.net>          *
 *   Copyright (C) 2009-2015 by Ilya Kotov <forkotov02@hotmail.ru>         *
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
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#include <QSettings>
#include <QByteArray>
#include <QDir>
#include <QFileInfo>
#include <QSettings>
#include <math.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <qmmp/qmmp.h>
#include <qmmp/audioparameters.h>
#include "ladspahost.h"

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

LADSPAHost *LADSPAHost::m_instance = 0;

LADSPAHost::LADSPAHost(QObject *parent) : QObject(parent)
{
    m_chan = 0;
    m_freq = 0;
    m_instance = this;
    findAllPlugins();

    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    int p = settings.value("LADSPA/plugins_number", 0).toInt();
    for(int i = 0; i < p; ++i)
    {
        QString section = QString("LADSPA_%1/").arg(i);
        int id = settings.value(section +"id").toInt();
        QString file = settings.value(section +"file").toString();
        int ports = settings.value(section +"ports").toInt();


        LADSPAPlugin *plugin = 0;
        foreach(LADSPAPlugin *p, plugins())
        {
            if(p->unique_id == id)
            {
                plugin = p;
                break;
            }
        }
        if(!plugin)
            continue;

        LADSPAEffect *effect = addPlugin(plugin);
        for(int j = 0; j < ports; ++j)
        {
            double value = settings.value(section + QString("port%1").arg(j)).toDouble();
            effect->knobs[j] = value;
        }
    }
}

LADSPAHost::~LADSPAHost()
{
    m_instance = 0;
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.setValue("LADSPA/plugins_number", m_effects.count());
    for(int i = 0; i < m_effects.count(); ++i)
    {
        QString section = QString("LADSPA_%1/").arg(i);
        settings.setValue(section +"id", (quint64)m_effects[i]->descriptor->UniqueID);
        settings.setValue(section +"file", m_effects[i]->fileName);

        int ports = qMin((int)m_effects[i]->descriptor->PortCount, MAX_KNOBS);
        settings.setValue(section +"ports", ports);
        for(int j = 0; j < ports; ++j)
        {
            settings.setValue(section + QString("port%1").arg(j),
                              m_effects[i]->knobs[j]);
        }
    }
    foreach(LADSPAEffect *effect, m_effects)
        unload(effect);
}

void LADSPAHost::configure(quint32 freq, int chan)
{
    m_chan = chan;
    m_freq = freq;
    foreach(LADSPAEffect *e, m_effects)
    {
        const LADSPA_Descriptor *descriptor = e->descriptor;
        if (e->handle)
        {
            if (descriptor->deactivate)
                descriptor->deactivate(e->handle);
            descriptor->cleanup(e->handle);
            e->handle = 0;
        }
        if (e->handle2)
        {
            if (descriptor->deactivate)
                descriptor->deactivate(e->handle2);
            descriptor->cleanup(e->handle2);
            e->handle2 = 0;
        }
        bootPlugin(e);
    }
}

LADSPAHost* LADSPAHost::instance()
{
    return m_instance;
}

QList <LADSPAPlugin *> LADSPAHost::plugins()
{
    return m_plugins;
}

QList <LADSPAEffect *> LADSPAHost::effects()
{
    return m_effects;
}

/*Based on xmms_ladspa */
void LADSPAHost::findAllPlugins()
{
    while(!m_plugins.isEmpty()) /* empty list */
        delete m_plugins.takeFirst();

    QString ladspa_path = qgetenv("LADSPA_PATH");
    QStringList directories;

    if (ladspa_path.isEmpty())
    {
        /* Fallback, look in obvious places */
        directories << "/usr/lib/ladspa";
        directories << "/usr/local/lib/ladspa";
        directories << "/usr/lib64/ladspa";
        directories << "/usr/local/lib64/ladspa";
    }
    else
        directories = ladspa_path.split(':');
    foreach(QString dir, directories)
        findPlugins(dir);
}

LADSPAEffect *LADSPAHost::load(const QString &filename, long int num)
{
    LADSPA_Descriptor_Function descriptor_fn;
    LADSPAEffect *instance = new LADSPAEffect;

    instance->fileName = filename;
    instance->library = dlopen(qPrintable(filename), RTLD_NOW);
    instance->handle = 0;
    instance->handle2 = 0;
    if (!instance->library)
    {
        delete instance;
        return 0;
    }
    descriptor_fn = (LADSPA_Descriptor_Function) dlsym(instance->library, "ladspa_descriptor");
    if (!descriptor_fn)
    {
        dlclose(instance->library);
        delete instance;
        return 0;
    }
    instance->descriptor = descriptor_fn(num);

    return instance;
}

void LADSPAHost::unload(LADSPAEffect *instance)
{
    const LADSPA_Descriptor *descriptor = instance->descriptor;

    if (instance->handle)
    {
        if (descriptor->deactivate)
            descriptor->deactivate(instance->handle);
        descriptor->cleanup(instance->handle);
        instance->handle = 0;
    }
    if (instance->handle2)
    {
        if (descriptor->deactivate)
            descriptor->deactivate(instance->handle2);
        descriptor->cleanup(instance->handle2);
        instance->handle2 = 0;
    }

    if (instance->library)
    {
        dlclose(instance->library);
        instance->library = 0;
    }
    m_effects.removeAll(instance);
    qDeleteAll(instance->controls);
    delete instance;
}

void LADSPAHost::bootPlugin(LADSPAEffect *instance)
{
    const LADSPA_Descriptor *descriptor = instance->descriptor;

    instance->handle = descriptor->instantiate(descriptor, m_freq);
    if (m_chan > 1 && !instance->stereo)
    {
        /* Create an additional instance */
        instance->handle2 = descriptor->instantiate(descriptor, m_freq);
    }

    portAssign(instance);

    if (descriptor->activate)
    {
        descriptor->activate(instance->handle);
        if (instance->handle2)
            descriptor->activate(instance->handle2);
    }
}

int LADSPAHost::applyEffect(float *data, size_t samples)
{
    LADSPAEffect *instance;
    uint k;

    if (m_effects.isEmpty())
        return samples;

    if (m_chan == 1)
    {
        memcpy(m_left, data, samples * sizeof(float));

        foreach(instance, m_effects)
        {
            if (instance->handle)
                instance->descriptor->run(instance->handle, samples);
        }
        for (k = 0; k < samples; ++k)
            data[k] = qBound(-1.0f, m_left[k], 1.0f);
    }
    else
    {
        for (k = 0; k < samples; k += 2)
        {
            m_left[k >> 1] = data[k];
            m_right[k >> 1] = data[k+1];
        }
        foreach(instance, m_effects)
        {
            if (instance->handle)
                instance->descriptor->run(instance->handle, samples >> 1);
            if (instance->handle2)
                instance->descriptor->run(instance->handle2, samples >> 1);
        }
        for (k = 0; k < samples; k+=2)
        {
            data[k] = qBound(-1.0f, m_left[k >> 1], 1.0f);
            data[k+1] = qBound(-1.0f, m_right[k>> 1], 1.0f);
        }
    }
    return samples;
}

void LADSPAHost::portAssign(LADSPAEffect *instance)
{
    unsigned long port;
    unsigned long inputs = 0, outputs = 0;
    const LADSPA_Descriptor *plugin = instance->descriptor;

    for (port = 0; port < plugin->PortCount; ++port)
    {
        if (LADSPA_IS_PORT_CONTROL(plugin->PortDescriptors[port]))
        {
            if (port < MAX_KNOBS)
            {
                plugin->connect_port(instance->handle, port, &(instance->knobs[port]));
                if (instance->handle2)
                    plugin->connect_port(instance->handle2, port, &(instance->knobs[port]));
            }
            else
            {
                plugin->connect_port(instance->handle, port, m_trash);
                if (instance->handle2)
                    plugin->connect_port(instance->handle2, port, m_trash);
            }

        }
        else if (LADSPA_IS_PORT_AUDIO(plugin->PortDescriptors[port]))
        {

            if (LADSPA_IS_PORT_INPUT(plugin->PortDescriptors[port]))
            {
                if (inputs == 0)
                {
                    plugin->connect_port(instance->handle, port, m_left);
                    if (instance->handle2)
                        plugin->connect_port(instance->handle2, port, m_right);
                }
                else if (inputs == 1 && instance->stereo)
                {
                    plugin->connect_port(instance->handle, port, m_right);
                }
                else
                {
                    plugin->connect_port(instance->handle, port, m_trash);
                    if (instance->handle2)
                        plugin->connect_port(instance->handle2, port, m_trash);
                }
                inputs++;

            }
            else if (LADSPA_IS_PORT_OUTPUT(plugin->PortDescriptors[port]))
            {
                if (outputs == 0)
                {
                    plugin->connect_port(instance->handle, port, m_left);
                    if (instance->handle2)
                        plugin->connect_port(instance->handle2, port, m_right);
                }
                else if (outputs == 1 && instance->stereo)
                {
                    plugin->connect_port(instance->handle, port, m_right);
                }
                else
                {
                    plugin->connect_port(instance->handle, port, m_trash);
                    if (instance->handle2)
                        plugin->connect_port(instance->handle2, port, m_trash);
                }
                outputs++;
            }
        }
    }
}

void LADSPAHost::findPlugins(const QString &path_entry)
{
    LADSPAPlugin *plugin;
    void *library = 0;
    LADSPA_Descriptor_Function descriptor_fn;
    const LADSPA_Descriptor *descriptor;
    long int k;
    unsigned long int port, input, output;

    QDir dir (path_entry);
    dir.setFilter(QDir::Files | QDir::Hidden);
    dir.setSorting(QDir::Name);
    QFileInfoList files = dir.entryInfoList((QStringList() << "*.so"));

    foreach(QFileInfo file, files)
    {
        library = dlopen(qPrintable(file.absoluteFilePath ()), RTLD_LAZY);
        if (library == 0)
        {
            continue;
        }
        descriptor_fn = (LADSPA_Descriptor_Function) dlsym(library, "ladspa_descriptor");
        if (descriptor_fn == 0)
        {
            dlclose(library);
            continue;
        }

        for (k = 0;; ++k)
        {
            descriptor = descriptor_fn(k);
            if (descriptor == 0)
            {
                break;
            }
            plugin = new LADSPAPlugin;
            plugin->name = strdup(descriptor->Name);
            plugin->fileName = file.absoluteFilePath ();
            plugin->id = k;
            plugin->unique_id = descriptor->UniqueID;
            for (input = output = port = 0; port < descriptor->PortCount; ++port)
            {
                if (LADSPA_IS_PORT_AUDIO(descriptor->PortDescriptors[port]))
                {
                    if (LADSPA_IS_PORT_INPUT(descriptor->PortDescriptors[port]))
                        input++;
                    if (LADSPA_IS_PORT_OUTPUT(descriptor->PortDescriptors[port]))
                        output++;
                }
                else if (LADSPA_IS_PORT_CONTROL(descriptor->PortDescriptors[port]))
                {
                }
            }
            plugin->stereo = (input >= 2 && output >= 2);
            m_plugins.append(plugin);
        }
        dlclose(library);
    }
}

LADSPAEffect *LADSPAHost::addPlugin(LADSPAPlugin *plugin)
{
    if (!plugin)
        return 0;
    LADSPAEffect *instance;
    if (!(instance = load(plugin->fileName, plugin->id)))
        return 0;
    instance->stereo = plugin->stereo;
    if (m_chan && m_freq)
        bootPlugin(instance);
    initialize(instance);
    m_effects.append(instance);
    return instance;
}

void LADSPAHost::initialize(LADSPAEffect *instance)
{
    const LADSPA_Descriptor *plugin = instance->descriptor;
    const LADSPA_PortRangeHint *hints = plugin->PortRangeHints;
    LADSPA_Data fact, min, max, step, start;

    for (unsigned long k = 0; k < MAX_KNOBS && k < plugin->PortCount; ++k)
    {
        if (!LADSPA_IS_PORT_CONTROL(plugin->PortDescriptors[k]))
            continue;

        LADSPAControl *c = new LADSPAControl;
        c->name = QString(plugin->PortNames[k]);

        if (LADSPA_IS_HINT_TOGGLED(hints[k].HintDescriptor))
        {
            c->type = LADSPAControl::BUTTON;
            c->min = 0;
            c->max = 0;
            c->step = 0;
            c->value = &instance->knobs[k];
            instance->controls << c;
            continue;
        }

        if (LADSPA_IS_HINT_SAMPLE_RATE(hints[k].HintDescriptor))
            fact = m_freq;
        else
            fact = 1.0f;

        if (LADSPA_IS_HINT_BOUNDED_BELOW(hints[k].HintDescriptor))
            min = hints[k].LowerBound * fact;
        else
            min = -10000.0f;

        if (LADSPA_IS_HINT_BOUNDED_ABOVE(hints[k].HintDescriptor))
            max = hints[k].UpperBound * fact;
        else
            max = 10000.0f;

        /* infinity */
        if (10000.0f <= max - min)
        {
            step = 5.0f;

            /* 100.0 ... lots */
        }
        else if (100.0f < max - min)
        {
            step = 5.0f;

            /* 10.0 ... 100.0 */
        }
        else if (10.0f < max - min)
        {
            step = 0.5f;

            /* 1.0 ... 10.0 */
        }
        else if (1.0f < max - min)
        {
            step = 0.05f;

            /* 0.0 ... 1.0 */
        }
        else
        {
            step = 0.005f;
        }

        if (LADSPA_IS_HINT_INTEGER(hints[k].HintDescriptor))
        {
            if (step < 1.0f)
                step = 1.0f;
        }

        if (LADSPA_IS_HINT_DEFAULT_MINIMUM(hints[k].HintDescriptor))
            start = min;
        else if (LADSPA_IS_HINT_DEFAULT_LOW(hints[k].HintDescriptor))
            start = min * 0.75f + max * 0.25f;
        else if (LADSPA_IS_HINT_DEFAULT_MIDDLE(hints[k].HintDescriptor))
            start = min * 0.5f + max * 0.5f;
        else if (LADSPA_IS_HINT_DEFAULT_HIGH(hints[k].HintDescriptor))
            start = min * 0.25f + max * 0.75f;
        else if (LADSPA_IS_HINT_DEFAULT_MAXIMUM(hints[k].HintDescriptor))
            start = max;
        else if (LADSPA_IS_HINT_DEFAULT_0(hints[k].HintDescriptor))
            start = 0.0f;
        else if (LADSPA_IS_HINT_DEFAULT_1(hints[k].HintDescriptor))
            start = 1.0f;
        else if (LADSPA_IS_HINT_DEFAULT_100(hints[k].HintDescriptor))
            start = 100.0f;
        else if (LADSPA_IS_HINT_DEFAULT_440(hints[k].HintDescriptor))
            start = 440.0f;
        else if (LADSPA_IS_HINT_INTEGER(hints[k].HintDescriptor))
            start = min;
        else if (max >= 0.0f && min <= 0.0f)
            start = 0.0f;
        else
            start = min * 0.5f + max * 0.5f;

        instance->knobs[k] = start;
        if(LADSPA_IS_PORT_OUTPUT(plugin->PortDescriptors[k]))
            c->type = LADSPAControl::LABEL;
        else
            c->type = LADSPAControl::SLIDER;
        c->min = min;
        c->max = max;
        c->step = step;
        c->value = &instance->knobs[k];
        instance->controls << c;
    }
}
