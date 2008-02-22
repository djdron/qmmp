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

#include "config.h"
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
            {
                qDebug("General: plugin loaded - %s", qPrintable(fileName));
            }
            GeneralFactory *factory = 0;
            if (plugin)
                factory = qobject_cast<GeneralFactory *>(plugin);

            if (factory)
            {
                factories->append(factory);
                files << pluginsDir.absoluteFilePath(fileName);
            }
        }
    }
}

General::General(QObject *parent)
        : QObject(parent)
{}


General::~General()
{}

void General::setState(const uint&)
{}

void General::setSongInfo(const SongInfo &song)
{}


QList<GeneralFactory*> *General::generalFactories()
{
    checkFactories();
    return factories;
}

void General::updateVolume(int left, int right)
{}

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

void General::play()
{
    emit commandCalled(Play);
}

void General::pause()
{
    emit commandCalled(Pause);
}

void General::stop()
{
    emit commandCalled(Stop);
}

void General::next()
{
    emit commandCalled(Next);
}

void General::previous()
{
    emit commandCalled(Previous);
}

void General::exit()
{
    emit commandCalled(Exit);
}

void General::toggleVisibility()
{
    emit commandCalled(ToggleVisibility);
}

void General::setVolume(int left, int right)
{
    emit volumeChanged(left, right);
}

