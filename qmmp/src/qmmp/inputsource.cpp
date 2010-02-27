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

#include <QFile>
#include <QSettings>
#include <QDir>
#include <QApplication>
#include <QPluginLoader>
#include "qmmp.h"
#include "fileinputsource.h"
#include "emptyinputsource.h"
#include "inputsource.h"

InputSource::InputSource(const QString &source, QObject *parent) : QObject(parent)
{
    m_url = source;
    m_offset = -1;
}

const QString InputSource::url() const
{
    return m_url;
}

qint64 InputSource::offset() const
{
    return m_offset;
}
void InputSource::setOffset(qint64 offset)
{
    m_offset = offset;
}

// static methods
QList<InputSourceFactory*> *InputSource::m_factories = 0;
QStringList InputSource::m_files;

InputSource *InputSource::create(const QString &url, QObject *parent)
{
    checkFactories();
    InputSourceFactory *factory = 0;
    if(!url.contains("://")) //local file path doesn't contain "://"
    {
        qDebug("InputSource: using file transport");
        return new FileInputSource(url, parent);
    }
    foreach(InputSourceFactory *f, *m_factories)
    {
        QStringList protocols = f->properties().protocols.split(" ");
        if(protocols.contains(url.section("://", 0, 0)))
        {
            factory =  f;
            break;
        }
    }
    if(factory)
    {
        qDebug("InputSource: using %s transport", qPrintable(url.section("://", 0, 0)));
        return factory->create(url, parent);
    }
    else
    {
        qDebug("InputSource: using fake transport");
        return new EmptyInputSource(url, parent);
    }
}

QList<InputSourceFactory*> *InputSource::factories()
{
    checkFactories();
    return m_factories;
}

QStringList InputSource::files()
{
    checkFactories();
    return m_files;
}

void InputSource::checkFactories()
{
    QSettings settings (Qmmp::configFile(), QSettings::IniFormat);

    if (!m_factories)
    {
        m_files.clear();
        m_factories = new QList<InputSourceFactory *>;

        QDir pluginsDir (Qmmp::pluginsPath());
        pluginsDir.cd("Transports");
        foreach (QString fileName, pluginsDir.entryList(QDir::Files))
        {
            QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
            QObject *plugin = loader.instance();
            if (loader.isLoaded())
                qDebug("InputSource: plugin loaded - %s", qPrintable(fileName));
            else
                qWarning("InputSource: %s", qPrintable(loader.errorString ()));
            InputSourceFactory *factory = 0;
            if (plugin)
                factory = qobject_cast<InputSourceFactory *>(plugin);

            if (factory)
            {
                m_factories->append(factory);
                m_files << pluginsDir.absoluteFilePath(fileName);
                qApp->installTranslator(factory->createTranslator(qApp));
            }
        }
    }
}

