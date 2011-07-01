/***************************************************************************
 *   Copyright (C) 2011 by Ilya Kotov                                      *
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
#include <QObject>
#include <QList>
#include <QApplication>
#include <qmmp/qmmp.h>
#include "uiloader.h"

QList<UiFactory*> *UiLoader::m_factories = 0;
QStringList UiLoader::m_files;

void UiLoader::checkFactories()
{
    if (!m_factories)
    {
        m_files.clear();
        m_factories = new QList<UiFactory *>;
        QDir pluginsDir(Qmmp::pluginsPath());
        pluginsDir.cd("Ui");
        foreach (QString fileName, pluginsDir.entryList(QDir::Files))
        {
            QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
            QObject *plugin = loader.instance();
            if (loader.isLoaded())
                qDebug("UiLoader: plugin loaded - %s", qPrintable(fileName));
            else
                qWarning("UiLoader: %s", qPrintable(loader.errorString()));

            UiFactory *factory = 0;
            if (plugin)
                factory = qobject_cast<UiFactory *>(plugin);

            if (factory)
            {
                m_factories->append(factory);
                m_files << pluginsDir.absoluteFilePath(fileName);
                qApp->installTranslator(factory->createTranslator(qApp));
            }
        }
    }
}

QList<UiFactory*> *UiLoader::factories()
{
    checkFactories();
    return m_factories;
}

QStringList UiLoader::files()
{
    checkFactories();
    return m_files;
}

void UiLoader::setCurrentUiFactory(UiFactory* factory)
{
    checkFactories();
    if (!m_factories->contains(factory))
        return;
    QSettings settings (Qmmp::configFile(), QSettings::IniFormat);
    settings.setValue ("Ui/current_plugin", factory->properties().shortName);
}

UiFactory *UiLoader::currentUiFactory()
{
    checkFactories();
    QSettings settings (Qmmp::configFile(), QSettings::IniFormat);
    QString name = settings.value("Ui/current_plugin", "skinned").toString();
    foreach(UiFactory *factory, *m_factories)
    {
        if (factory->properties().shortName == name)
            return factory;
    }
    if (!m_factories->isEmpty())
        return m_factories->at(0);
    return 0;
}
