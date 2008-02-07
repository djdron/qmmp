/***************************************************************************
 *   Copyright (C) 2007 by Ilya Kotov                                      *
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

#include <QDir>
#include <QPluginLoader>

#include "commandlineoption.h"
#include "mainwindow.h"
#include <soundcore.h>


// Command option manager methods
CommandLineOptionManager::CommandLineOptionManager()
{
    this->registerBuiltingCommandLineOptions();
    this->registerExternalCommandLineOptions();
}

bool CommandLineOptionManager::hasOption(const QString &opt)
{
    for(int i = 0; i < m_options.count(); i++)
    {
        if(m_options[i]->identify(opt))
        {
            return true;
        }
    }
    return false;
}

void CommandLineOptionManager::executeCommand(const QString &opt, MainWindow *mw)
{
    for(int i = 0; i < m_options.count(); i++)
    {
        if(m_options[i]->identify(opt))
        {
            m_options[i]->executeCommand(opt,mw);
        }
    }
}

CommandLineOption * CommandLineOptionManager::operator [](int index)
{
    return m_options[index];
}

int CommandLineOptionManager::count() const
{
    return m_options.count();
}


void CommandLineOptionManager::registerBuiltingCommandLineOptions()
{
    m_options.append(new BuiltinCommandLineOption());
}


void CommandLineOptionManager::registerExternalCommandLineOptions()
{
    QDir pluginsDir (QDir::homePath()+"/.qmmp/plugins/CommandLineOptions");
    foreach (QString fileName, pluginsDir.entryList(QDir::Files))
    {
        QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = loader.instance();
        if (loader.isLoaded())
            qDebug("CommandLineOption: plugin loaded - %s", qPrintable(fileName));
        else
            qWarning(qPrintable(loader.errorString()));

        CommandLineOption *cmd_option = 0;
        if (plugin)
            cmd_option = qobject_cast<CommandLineOption *>(plugin);

        if (cmd_option)
        {
            foreach(CommandLineOption* opt,m_options)
            {
                if (opt->name() == cmd_option->name())
                {
                    qDebug("CommandLineOption: Plugin with name %s is already registered...",
                       qPrintable(cmd_option->name()));
                    return;
                }
            }
            m_options.append(cmd_option);
        }
    }
}

///////////////////////////////////////////////////////////////////


// BuiltinCommandLineOption methods implementation
bool BuiltinCommandLineOption::identify(const QString & str) const
{
    if(
       str == QString("--help") ||
       str == QString("--next") ||
       str == QString("--previous") ||
       str == QString("--play") ||
       str == QString("--pause") ||
       str == QString("--play-pause") ||
       str == QString("--stop") ||
       str.startsWith("--volume") ||
       str.startsWith("--jump-to-file") ||
       str.startsWith("--toggle-visibility") ||
       str.startsWith("--add-file")
      )
    {
        return TRUE;
    }
    
    return FALSE;
}

const QString BuiltinCommandLineOption::helpString() const
{
    return  QString(
            "--next               Skip forward in playlist\n"
            "--previous           Skip backwards in playlist\n"
            "--play               Start playing current song\n"
            "--pause              Pause current song\n"
            "--play-pause         Pause if playing, play otherwise\n"
            "--stop               Stop current song\n"
            "--next               Skip forward in playlist\n"
            "--volume             Set playback volume(example: qmmp --volume20, qmmp --volume100)\n"
            "--jump-to-file       Display Jump to File dialog\n"
            "--toggle-visibility  Show/hide application\n"
            "--add-file           Display Add File dialog"
                   );
}

void BuiltinCommandLineOption::executeCommand(const QString & option_string, MainWindow *mw)
{
    if (option_string == "--play")
    {
        mw->play();
    }
    else if (option_string == "--stop")
    {
        mw->stop();
        mw->mainDisplay()->hideTimeDisplay();
    }
    else if (option_string == "--pause")
    {
        mw->pause();
    }
    else if (option_string == "--next")
    {
        mw->next();
        if(!mw->soundCore()->isInitialized())
            mw->play();
    }
    else if (option_string == "--previous")
    {
        mw->previous();
        if(!mw->soundCore()->isInitialized())
            mw->play();
    }
    else if (option_string == "--play-pause")
    {
        mw->playPause();
    }
    else if (option_string == "--jump-to-file")
    {
        mw->jumpToFile();
    }
    else if (option_string == "--toggle-visibility")
    {
        mw->toggleVisibility();
    }
    else if (option_string == "--add-file")
    {
        mw->addFile();
    }
    else if (option_string.startsWith("--volume"))
    {
        QString vol_str(option_string);
        vol_str.remove("--volume");
        bool ok = FALSE;
        int volume = vol_str.toUInt(&ok);
        if(ok)
        {
            mw->soundCore()->setVolume(volume,volume);
        }
    }
}

const QString BuiltinCommandLineOption::name() const
{
    return "BuiltinCommandLineOption";
}






