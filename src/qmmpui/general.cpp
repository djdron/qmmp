/***************************************************************************
 *   Copyright (C) 2008 by Ilya Kotov                                      *
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

#include "general.h"


static QList<GeneralFactory*> *factories = 0;
static QStringList files;

static void checkFactories()
{
    if (! factories)
    {
        files.clear();
        factories = new QList<GeneralFactory *>;

        QDir pluginsDir (qApp->applicationDirPath());
        pluginsDir.cdUp();
        pluginsDir.cd("./"LIB_DIR"/qmmp/General");
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
                factories->append(factory);
                files << pluginsDir.absoluteFilePath(fileName);
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

void General::play()
{
    //TODO use AbstractPlayList and SoundCore
    emit playCalled();
}

void General::exit()
{
    emit exitCalled();
}

void General::toggleVisibility()
{
    emit toggleVisibilityCalled();
}

QList<GeneralFactory*> *General::generalFactories()
{
    checkFactories();
    return factories;
}

QStringList General::generalFiles()
{
    checkFactories();
    return files;
}

void General::setEnabled(GeneralFactory* factory, bool enable)
{
    checkFactories();
    if (!factories->contains(factory))
        return;

    QString name = files.at(factories->indexOf(factory)).section('/',-1);
    QSettings settings ( QDir::homePath() +"/.qmmp/qmmprc", QSettings::IniFormat );
    QStringList genList = settings.value("General/plugin_files").toStringList();

    if (enable)
    {
        if (!genList.contains(name))
            genList << name;
    }
    else
        genList.removeAll(name);
    settings.setValue("General/plugin_files", genList);
}

bool General::isEnabled(GeneralFactory* factory)
{
    checkFactories();
    if (!factories->contains(factory))
        return FALSE;
    QString name = files.at(factories->indexOf(factory)).section('/',-1);
    QSettings settings ( QDir::homePath() +"/.qmmp/qmmprc", QSettings::IniFormat );
    QStringList genList = settings.value("General/plugin_files").toStringList();
    return genList.contains(name);
}

