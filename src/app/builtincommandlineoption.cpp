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
#include <qmmpui/mediaplayer.h>
#include <qmmpui/uihelper.h>
#include <qmmpui/filedialog.h>
#include <qmmp/metadatamanager.h>
#include "builtincommandlineoption.h"

BuiltinCommandLineOption::BuiltinCommandLineOption(QObject *parent) : QObject(parent)
{
    m_model = 0;
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
                                              const QString &cwd)
{
    SoundCore *core = SoundCore::instance();
    MediaPlayer *player = MediaPlayer::instance();
    PlayListManager *pl_manager = PlayListManager::instance();
    if(!core || !player)
        return;
    if(option_string == "--enqueue" || option_string == "-e" || option_string.isEmpty())
    {
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
        pl_manager->activatePlayList(pl_manager->selectedPlayList());
        if(option_string.isEmpty()) //clear playlist if option is empty
        {
            if (core->state() != Qmmp::Stopped)
            {
                core->stop();
                qApp->processEvents(); //receive stop signal
            }
            m_model = pl_manager->selectedPlayList();
            m_model->clear();
            connect(m_model, SIGNAL(itemAdded(PlayListItem*)), player, SLOT(play()));
            connect(core, SIGNAL(stateChanged(Qmmp::State)), SLOT(disconnectPl()));
            connect(m_model, SIGNAL(loaderFinished()), SLOT(disconnectPl()));
            m_model->add(full_path_list);
        }
        else
        {
            pl_manager->selectedPlayList()->add(full_path_list);
            return;
        }
    }
    else if (option_string == "--play" || option_string == "-p")
    {
        player->play();
    }
    else if (option_string == "--stop" || option_string == "-s")
    {
        core->stop();
    }
    else if (option_string == "--pause" || option_string == "-u")
    {
        core->pause();
    }
    else if (option_string == "--next")
    {
        player->next();
        if (core->state() == Qmmp::Stopped)
            player->play();
    }
    else if (option_string == "--previous")
    {
        player->previous();
        if (core->state() == Qmmp::Stopped)
            player->play();
    }
    else if (option_string == "--play-pause"  || option_string == "-t")
    {
        if (core->state() == Qmmp::Playing)
            core->pause();
        else
            player->play();
    }
    else if (option_string == "--jump-to-file" || option_string == "-j")
    {
        //mw->jumpToFile();
    }
    else if (option_string == "--quit" || option_string == "-q")
    {
        qApp->closeAllWindows();
        qApp->quit();
    }
    else if (option_string == "--toggle-visibility")
    {
        UiHelper::instance()->toggleVisibility();
    }
    else if (option_string == "--add-file")
    {
        QString m_lastDir;
        QStringList filters;
        filters << tr("All Supported Bitstreams")+" (" +
                MetaDataManager::instance()->nameFilters().join (" ") +")";
        filters << MetaDataManager::instance()->filters();
        FileDialog::popup(qApp->activeWindow(), FileDialog::AddDirsFiles, &m_lastDir,
                          pl_manager->selectedPlayList(), SLOT(add(const QStringList&)),
                          tr("Select one or more files to open"), filters.join(";;"));
    }
    else if (option_string == "--add-dir")
    {
        QString m_lastDir;
        FileDialog::popup(qApp->activeWindow(), FileDialog::AddDirs, &m_lastDir,
                          pl_manager->selectedPlayList(), SLOT(add(const QStringList&)),
                          tr("Choose a directory"));
    }
    else if (option_string == "--volume" && !args.isEmpty())
    {
        bool ok = false;
        int volume = args.at(0).toInt(&ok);
        if (ok)
            core->setVolume(volume,volume);
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

void BuiltinCommandLineOption::disconnectPl()
{
    if(m_model)
    {
        disconnect(m_model, SIGNAL(itemAdded(PlayListItem*)), MediaPlayer::instance(), SLOT(play()));
        disconnect(m_model, SIGNAL(loaderFinished()), this, SLOT(disconnectPl()));
        disconnect(SoundCore::instance(), SIGNAL(stateChanged(Qmmp::State)), this, SLOT(disconnectPl()));
        m_model = 0;
    }
}
