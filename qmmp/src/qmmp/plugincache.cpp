/***************************************************************************
 *   Copyright (C) 2013 by Ilya Kotov                                      *
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
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#include <QStringList>
#include <QDateTime>
#include <QFileInfo>
#include <QPluginLoader>
#include <QApplication>
#include <QTranslator>
#include "decoderfactory.h"
#include "outputfactory.h"
#include "plugincache.h"

QmmpPluginCache::QmmpPluginCache(const QString &file, QSettings *settings)
{
    m_error = false;
    m_instance = 0;
    m_decoderFactory = 0;
    bool update = false;
    QFileInfo info(file);
    m_path = info.QFileInfo::canonicalFilePath();

    settings->beginGroup("PluginCache");
    QString copy = m_path;
    if(settings->allKeys().contains(copy.remove(0,1)))
    {
        QStringList values = settings->value(m_path).toStringList();
        if(values.count() != 2)
            update = true;
        else
        {
            m_shortName = values.at(0);
            update = (info.lastModified().toString(Qt::ISODate) != values.at(1));
        }
    }
    else
        update = true;


    if(update)
    {
        if(DecoderFactory *factory = decoderFactory())
        {
            m_shortName = factory->properties().shortName;
        }
        /*else if(OutputFactory *factory = outputFactory())
        {
            m_shortName = factory->properties().shortName;
        }*/
        else
            m_error = true;

        if (!m_error)
        {
            QStringList values;
            values << m_shortName;
            values << info.lastModified().toString(Qt::ISODate);
            settings->setValue(m_path, values);
            qDebug("PluginCache: added to cache: %s", qPrintable(m_path));
        }
    }

    settings->endGroup();
}

const QString QmmpPluginCache::shortName() const
{
    return m_shortName;
}

const QString QmmpPluginCache::file() const
{
    return m_path;
}

DecoderFactory *QmmpPluginCache::decoderFactory()
{
    if(!m_decoderFactory)
    {
        m_decoderFactory = qobject_cast<DecoderFactory *> (instance());
        if(m_decoderFactory)
            qApp->installTranslator(m_decoderFactory->createTranslator(qApp));
    }
    return m_decoderFactory;
}

bool QmmpPluginCache::hasError() const
{
    return m_error;
}

QObject *QmmpPluginCache::instance()
{
    if(m_error)
        return 0;
    if(m_instance)
        return m_instance;
    QPluginLoader loader(m_path);
    m_instance = loader.instance();
    if (loader.isLoaded())
        qDebug("PluginCache: loaded plugin %s", qPrintable(QFileInfo(m_path).fileName()));
    else
    {
        m_error = true;
        qWarning("PluginCache: %s", qPrintable(loader.errorString ()));
    }
    return m_instance;
}
