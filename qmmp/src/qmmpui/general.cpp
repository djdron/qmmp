/***************************************************************************
 *   Copyright (C) 2008-2010 by Ilya Kotov                                 *
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
QStringList General::m_files;
QMap <GeneralFactory*, General*> *General::m_generals = 0;
QObject *General::m_parent = 0;

void General::checkFactories()
{
    if (!m_factories)
    {
        m_files.clear();
        m_factories = new QList<GeneralFactory *>;
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
                m_files << pluginsDir.absoluteFilePath(fileName);
                qApp->installTranslator(factory->createTranslator(qApp));
            }
        }
    }
}

General::General(QObject *parent)
        : QObject(parent)
{
    Q_UNUSED(parent);
}


General::~General()
{}

void General::create(QObject *parent)
{
    if(m_generals)
        return;
    m_generals = new QMap <GeneralFactory*, General*>();
    m_parent = parent;
    checkFactories();
    foreach(GeneralFactory* factory, *General::factories())
    {
        if (General::isEnabled(factory))
        {
            General *general = factory->create(parent);
            m_generals->insert(factory, general);
        }
    }
}

QList<GeneralFactory*> *General::factories()
{
    checkFactories();
    return m_factories;
}

QStringList General::files()
{
    checkFactories();
    return m_files;
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
        General *general = factory->create(m_parent);
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
        General *general = factory->create(m_parent);
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
