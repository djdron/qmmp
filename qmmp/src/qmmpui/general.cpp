/***************************************************************************
 *   Copyright (C) 2008-2011 by Ilya Kotov                                 *
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
#include "general.h"

QList<GeneralFactory*> *General::m_factories = 0;
QHash <GeneralFactory*, QString> *General::m_files = 0;
QHash <GeneralFactory*, QObject*> *General::m_generals = 0;
QObject *General::m_parent = 0;

void General::checkFactories()
{
    if (!m_factories)
    {
        m_factories = new QList<GeneralFactory *>;
        m_files = new QHash <GeneralFactory*, QString>;
        QDir pluginsDir (Qmmp::pluginsPath());
        pluginsDir.cd("General");
        foreach (QString fileName, pluginsDir.entryList(QDir::Files))
        {
            QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
            QObject *plugin = loader.instance();
            if (loader.isLoaded())
                qDebug("General: plugin loaded - %s", qPrintable(fileName));
            else
                qWarning("General: %s", qPrintable(loader.errorString ()));

            GeneralFactory *factory = 0;
            if (plugin)
                factory = qobject_cast<GeneralFactory *>(plugin);

            if (factory)
            {
                m_factories->append(factory);
                m_files->insert(factory, pluginsDir.absoluteFilePath(fileName));
                qApp->installTranslator(factory->createTranslator(qApp));
            }
        }
    }
}

void General::create(QObject *parent)
{
    if(m_generals)
        return;
    m_generals = new QHash <GeneralFactory*, QObject*>();
    m_parent = parent;
    checkFactories();
    foreach(GeneralFactory* factory, *General::factories())
    {
        if (General::isEnabled(factory))
        {
            QObject *general = factory->create(parent);
            m_generals->insert(factory, general);
        }
    }
}

QList<GeneralFactory*> *General::factories()
{
    checkFactories();
    return m_factories;
}

QString General::file(GeneralFactory *factory)
{
    checkFactories();
    return m_files->value(factory);
}

void General::setEnabled(GeneralFactory* factory, bool enable)
{
    checkFactories();
    if (!m_factories->contains(factory))
        return;

    QString name = factory->properties().shortName;
    QSettings settings ( Qmmp::configFile(), QSettings::IniFormat );
    QStringList genList = settings.value("General/enabled_plugins").toStringList();

    if (enable)
    {
        if (!genList.contains(name))
            genList << name;
    }
    else
        genList.removeAll(name);
    settings.setValue("General/enabled_plugins", genList);
    if(!m_generals)
        return;


    if (enable == m_generals->keys().contains(factory))
        return;
    if (enable)
    {
        QObject *general = factory->create(m_parent);
        m_generals->insert(factory, general);
    }
    else
    {
        delete m_generals->value(factory);
        m_generals->remove(factory);
    }
}

void General::showSettings(GeneralFactory* factory, QWidget* parentWidget)
{
    QDialog *dialog = factory->createConfigDialog(parentWidget);
    if (!dialog)
        return;

    if (m_generals && dialog->exec() == QDialog::Accepted && m_generals->keys().contains(factory))
    {
        delete m_generals->value(factory);
        QObject *general = factory->create(m_parent);
        m_generals->insert(factory, general);
    }
    dialog->deleteLater();
}

bool General::isEnabled(GeneralFactory* factory)
{
    checkFactories();
    if (!m_factories->contains(factory))
        return false;
    QSettings settings (Qmmp::configFile(), QSettings::IniFormat );
    QStringList genList = settings.value("General/enabled_plugins").toStringList();
    return genList.contains(factory->properties().shortName);
}
