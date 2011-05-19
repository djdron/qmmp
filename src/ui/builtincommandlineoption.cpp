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

#include <QApplication>
#include <qmmp/soundcore.h>
#include "mainwindow.h"
#include "builtincommandlineoption.h"

BuiltinCommandLineOption::BuiltinCommandLineOption(QObject *parent) : QObject(parent)
{
    m_options << "--enqueue" << "-e"
              << "--play" << "-p"
              << "--pause" << "-u"
              << "--play-pause" << "-t"
              << "--stop" << "-s"
              << "--jump-to-file" << "-j"
              << "--quit" << "-q"
              << "--volume"
              << "--next" << "--previous"
              << "--toggle-visibility"
              << "--add-file" << "--add-dir";
}

BuiltinCommandLineOption::~BuiltinCommandLineOption()
{
}

// BuiltinCommandLineOption methods implementation
bool BuiltinCommandLineOption::identify(const QString &str) const
{
    return m_options.contains(str);
}

const QString BuiltinCommandLineOption::helpString() const
{
    return QString(
               "-e, --enqueue        "+tr("Don't clear the playlist") + "\n" +
               "-p, --play           "+tr("Start playing current song")+"\n" +
               "-u, --pause          "+tr("Pause current song")+ "\n"
               "-t, --play-pause     "+tr("Pause if playing, play otherwise")+ "\n"
               "-s, --stop           "+tr("Stop current song")+ "\n" +
               "-j, --jump-to-file   "+tr("Display Jump to File dialog")+ "\n" +
               "-q, --quit           "+tr("Quit application") + "\n" +
               "--volume <0..100>    "+tr("Set playback volume (example: qmmp --volume 20)") + "\n"
               "--next               "+tr("Skip forward in playlist")+ "\n" +
               "--previous           "+tr("Skip backwards in playlist")+"\n" +
               "--toggle-visibility  "+tr("Show/hide application")+ "\n" +
               "--add-file           "+tr("Display Add File dialog")+ "\n" +
               "--add-dir            "+tr("Display Add Directory dialog")
           );
}

void BuiltinCommandLineOption::executeCommand(const QString &option_string,
                                              const QStringList &args,
                                              const QString &cwd,
                                              MainWindow *mw)
{
    if(option_string == "--enqueue" || option_string == "-e" || option_string.isEmpty())
    {
        //QStringList args = commands.value(key);
        if(args.isEmpty())
            return;
        QStringList full_path_list;
        foreach(QString s, args)
        {
            if ((s.startsWith("/")) || (s.contains("://")))   //is it absolute path or url?
                full_path_list << s;
            else
                full_path_list << cwd + "/" + s;
        }
        //clear playlist if option is empty
        mw->setFileList(full_path_list, option_string.isEmpty());
    }
    else if (option_string == "--play" || option_string == "-p")
    {
        mw->play();
    }
    else if (option_string == "--stop" || option_string == "-s")
    {
        mw->stop();
    }
    else if (option_string == "--pause" || option_string == "-u")
    {
        mw->pause();
    }
    else if (option_string == "--next")
    {
        mw->next();
        if (mw->soundCore()->state() == Qmmp::Stopped)
            mw->play();
    }
    else if (option_string == "--previous")
    {
        mw->previous();
        if (mw->soundCore()->state() == Qmmp::Stopped)
            mw->play();
    }
    else if (option_string == "--play-pause"  || option_string == "-t")
    {
        mw->playPause();
    }
    else if (option_string == "--jump-to-file" || option_string == "-j")
    {
        mw->jumpToFile();
    }
    else if (option_string == "--quit" || option_string == "-q")
    {
        mw->close();
    }
    else if (option_string == "--toggle-visibility")
    {
        mw->toggleVisibility();
    }
    else if (option_string == "--add-file")
    {
        mw->addFile();
    }
    else if (option_string == "--add-dir")
    {
        mw->addDir();
    }
    else if (option_string == "--volume" && !args.isEmpty())
    {
        bool ok = false;
        int volume = args.at(0).toInt(&ok);
        if (ok)
            mw->soundCore()->setVolume(volume,volume);
    }
}

QHash <QString, QStringList> BuiltinCommandLineOption::splitArgs(const QStringList &args) const
{
    QHash <QString, QStringList> commands;
    foreach(QString arg, args)
    {
        if(arg.startsWith("-") || arg.startsWith("--"))
            commands.insert(arg, QStringList());
        else if(!commands.isEmpty())
            commands[commands.keys().last()] << arg;
    }
    return commands;
}
