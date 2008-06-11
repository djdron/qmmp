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

#include <cstdlib>
#include <iostream>

#include "control.h"

#include "commandlinemanager.h"

using namespace std;

static QList<CommandLineOption *> *options = 0;
static QStringList files;

static void checkOptions()
{
    if (! options)
    {
        files.clear();
        options = new QList<CommandLineOption *>;

        QDir pluginsDir (qApp->applicationDirPath());
        pluginsDir.cdUp();
        pluginsDir.cd("./"LIB_DIR"/qmmp/CommandLineOptions");
        foreach (QString fileName, pluginsDir.entryList(QDir::Files))
        {
            QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
            QObject *plugin = loader.instance();
            if (loader.isLoaded())
                ;//qDebug("CommandLineManager: plugin loaded - %s", qPrintable(fileName));
            else
                qWarning("CommandLineManager: %s", qPrintable(loader.errorString ()));

            CommandLineOption *option = 0;
            if (plugin)
                option = qobject_cast<CommandLineOption *>(plugin);

            if (option)
            {
                options->append(option);
                files << pluginsDir.absoluteFilePath(fileName);
                qApp->installTranslator(option->createTranslator(qApp));
            }
        }
    }
}

CommandLineManager::CommandLineManager(QObject *parent)
        : General(parent)
{
    m_state = General::Stopped;
    m_left = 0;
    m_right = 0;
    m_time = 0;
}


CommandLineManager::~CommandLineManager()
{
}

void CommandLineManager::setState(const uint& state)
{
    if (state == General::Stopped)
        m_time = 0;
    m_state = state;
}

void CommandLineManager::setSongInfo(const SongInfo &song)
{
    m_song = song;
}

void CommandLineManager::setTime(int time)
{
    m_time = time;
}

void CommandLineManager::setVolume(int left, int right)
{
    m_left = left;
    m_right = right;
}

uint CommandLineManager::state()
{
    return m_state;
}

SongInfo *CommandLineManager::info()
{
    return &m_song;
}

int CommandLineManager::elapsed()
{
    return m_time;
}

int CommandLineManager::leftVolume()
{
    return m_left;
}

int CommandLineManager::rightVolume()
{
    return m_right;
}

void CommandLineManager::executeCommand(const QString& opt_str, Control* ctrl)
{
    checkOptions();
    foreach(CommandLineOption *opt, *options)
    {
        if (opt->identify(opt_str))
        {
            opt->executeCommand(opt_str, this, ctrl);
            return;
        }
    }
}

bool CommandLineManager::hasOption(const QString &opt_str)
{
    checkOptions();
    foreach(CommandLineOption *opt, *options)
    {
        if (opt->identify(opt_str))
            return TRUE;
    }
    return FALSE;
}

void CommandLineManager::printUsage()
{
    checkOptions();
    foreach(CommandLineOption *opt, *options)
    cout << qPrintable(opt->helpString());
}
