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
#include <qmmp/qmmp.h>
#include "commandlinemanager.h"

using namespace std;

QList<CommandLineOption *> *CommandLineManager::m_options = 0;
QStringList CommandLineManager::m_files;

void CommandLineManager::checkOptions()
{
    if (!m_options)
    {
        m_files.clear();
        m_options = new QList<CommandLineOption *>;

        QDir pluginsDir (Qmmp::pluginsPath());
        pluginsDir.cd("CommandLineOptions");
        foreach (QString fileName, pluginsDir.entryList(QDir::Files))
        {
            QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
            QObject *plugin = loader.instance();
            if (loader.isLoaded())
                /*qDebug("CommandLineManager: plugin loaded - %s", qPrintable(fileName))*/;
            else
                qWarning("CommandLineManager: %s", qPrintable(loader.errorString ()));

            CommandLineOption *option = 0;
            if (plugin)
                option = qobject_cast<CommandLineOption *>(plugin);

            if (option)
            {
                m_options->append(option);
                m_files << pluginsDir.absoluteFilePath(fileName);
                qApp->installTranslator(option->createTranslator(qApp));
            }
        }
    }
}

CommandLineManager::CommandLineManager(QObject *parent)
        : General(parent)
{
    //m_state = General::Stopped;
    m_left = 0;
    m_right = 0;
    m_time = 0;
}


CommandLineManager::~CommandLineManager()
{
}

void CommandLineManager::executeCommand(const QString& opt_str)
{
    checkOptions();
    foreach(CommandLineOption *opt, *m_options)
    {
        if (opt->identify(opt_str))
        {
            opt->executeCommand(opt_str, this);
            return;
        }
    }
}

bool CommandLineManager::hasOption(const QString &opt_str)
{
    checkOptions();
    foreach(CommandLineOption *opt, *m_options)
    {
        if (opt->identify(opt_str))
            return TRUE;
    }
    return FALSE;
}

void CommandLineManager::printUsage()
{
    checkOptions();
    foreach(CommandLineOption *opt, *m_options)
    cout << qPrintable(opt->helpString());
}
