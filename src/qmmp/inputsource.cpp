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
#include <QDir>
#include <QApplication>
#include <QPluginLoader>
#include "qmmp.h"
#include "fileinputsource_p.h"
#include "emptyinputsource_p.h"
#include "inputsource.h"

InputSource::InputSource(const QString &source, QObject *parent) : QObject(parent)
{
    m_url = source;
    m_offset = -1;
    m_hasMetaData = false;
}

QString InputSource::contentType() const
{
    return QString();
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

bool InputSource::hasMetaData() const
{
    return m_hasMetaData;
}

QMap<Qmmp::MetaData, QString> InputSource::takeMetaData()
{
    m_hasMetaData = false;
    return m_metaData;
}

void InputSource::addMetaData(const QMap<Qmmp::MetaData, QString> &metaData)
{
    m_metaData = metaData;
    m_hasMetaData = true;
}

// static methods
QList<InputSourceFactory*> *InputSource::m_factories = 0;
QHash <InputSourceFactory*, QString> *InputSource::m_files = 0;

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
        if(f->properties().protocols.contains(url.section("://", 0, 0)))
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

QString InputSource::file(InputSourceFactory *factory)
{
    checkFactories();
    return m_files->value(factory);
}

QStringList InputSource::protocols()
{
    checkFactories();
    QStringList protocolsList;
    foreach(InputSourceFactory *f, *m_factories)
    {
        protocolsList << f->properties().protocols;
    }
    protocolsList.removeDuplicates();
    return protocolsList;
}

void InputSource::checkFactories()
{
    if (!m_factories)
    {
        m_files = new QHash <InputSourceFactory*, QString>;
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
                m_files->insert(factory, pluginsDir.absoluteFilePath(fileName));
                qApp->installTranslator(factory->createTranslator(qApp));
            }
        }
    }
}

