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

#include <QSettings>
#include <QDir>
#include <QPluginLoader>
#include <QApplication>
#include "enginefactory.h"
#include "qmmp.h"
#include "abstractengine.h"

AbstractEngine::AbstractEngine(QObject *parent) : QThread(parent)
{
}

QMutex *AbstractEngine::mutex()
{
    return &m_mutex;
}

QWaitCondition *AbstractEngine::cond()
{
    return &m_waitCondition;
}

// static methods
QList<EngineFactory*> *AbstractEngine::m_factories = 0;
QStringList AbstractEngine::m_files;

void AbstractEngine::checkFactories()
{
    if (!m_factories)
    {
        QSettings settings (Qmmp::configFile(), QSettings::IniFormat);
        m_files.clear();
        m_factories = new QList<EngineFactory *>;

        QDir pluginsDir (Qmmp::pluginsPath());
        pluginsDir.cd("Engines");
        foreach (QString fileName, pluginsDir.entryList(QDir::Files))
        {
            QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
            QObject *plugin = loader.instance();
            if (loader.isLoaded())
                qDebug("AbstractEngine: plugin loaded - %s", qPrintable(fileName));
            else
                qWarning("AbstractEngine: %s", qPrintable(loader.errorString ()));
            EngineFactory *factory = 0;
            if (plugin)
                factory = qobject_cast<EngineFactory *>(plugin);

            if (factory)
            {
                m_factories->append(factory);
                m_files << pluginsDir.absoluteFilePath(fileName);
                qApp->installTranslator(factory->createTranslator(qApp));
            }
        }
        //remove physically deleted plugins from disabled list
        QStringList names;
        foreach (EngineFactory *factory, *m_factories)
        {
            names.append(factory->properties().shortName);
        }
        QStringList disabledList  = settings.value("Engine/disabled_plugins").toStringList ();
        foreach (QString name, disabledList)
        {
            if (!names.contains(name))
                disabledList.removeAll(name);
        }
        settings.setValue("Engine/disabled_plugins",disabledList);
    }
}

QList<EngineFactory*> *AbstractEngine::factories()
{
    checkFactories();
    return m_factories;
}

EngineFactory *AbstractEngine::findByPath(const QString& source)
{
    checkFactories();
    foreach(EngineFactory *fact, *m_factories)
    {
        if (fact->supports(source) && isEnabled(fact))
            return fact;
    }
    return 0;
}

void AbstractEngine::setEnabled(EngineFactory* factory, bool enable)
{
    checkFactories();
    if (!m_factories->contains(factory))
        return;

    QString name = factory->properties().shortName;
    QSettings settings (Qmmp::configFile(), QSettings::IniFormat );
    QStringList disabledList = settings.value("Engine/disabled_plugins").toStringList();

    if (enable)
        disabledList.removeAll(name);
    else
    {
        if (!disabledList.contains(name))
            disabledList << name;
    }
    settings.setValue("Engine/disabled_plugins", disabledList);
}

bool AbstractEngine::isEnabled(EngineFactory* factory)
{
    checkFactories();
    if (!m_factories->contains(factory))
        return false;
    QString name = factory->properties().shortName;
    QSettings settings ( Qmmp::configFile(), QSettings::IniFormat );
    QStringList disabledList = settings.value("Engine/disabled_plugins").toStringList();
    return !disabledList.contains(name);
}

QStringList AbstractEngine::files()
{
    checkFactories();
    return m_files;
}

