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

#include <QtPlugin>
#include <QTranslator>
#include <QLocale>
#include <qmmp/soundcore.h>
#include <qmmpui/playlistmanager.h>
#include <qmmpui/metadataformatter.h>
#include <qmmpui/mediaplayer.h>
#include "playlistoption.h"

bool PlayListOption::identify(const QString & str) const
{
    return  str == QString("--pl-help") ||
            str == QString("--pl-list") ||
            str == QString("--pl-dump") ||
            str == QString("--pl-clear") ||
            str == QString("--pl-repeat-toggle") ||
            str == QString("--pl-shuffle-toggle") ||
            str == QString("--pl-state");
}

const QString PlayListOption::helpString() const
{
    return QString(
                "--pl-help            " + tr("Show playlist manipulation commands")+"\n"
                );
}

QString PlayListOption::executeCommand(const QString& opt_str, const QStringList &args)
{
    Q_UNUSED(args);
    QString out;
    PlayListManager *pl_manager = PlayListManager::instance();
    MediaPlayer *player = MediaPlayer::instance();

    if(opt_str == "--pl-help")
    {
        out = "--pl-list            " + tr("List all available playlists")+"\n"+
              "--pl-dump <id>       " + tr("Show playlist content")+"\n" +
              "--pl-clear <id>      " + tr("Clear playlist")+"\n"+
              "--pl-repeat-toggle   " + tr("Toggle playlist repeat")+"\n"+
              "--pl-shuffle-toggle  " + tr("Toggle playlist shuffle")+"\n"+
              "--pl-state           " + tr("Show playlist options")+"\n";
    }
    else if(opt_str == "--pl-list")
    {
        QStringList names = pl_manager->playListNames();
        for(int i = 0; i <  names.count(); ++i)
        {
            if(i == pl_manager->currentPlayListIndex())
                out += QString("%1. %2 [*]\n").arg(i+1).arg(names.at(i));
            else
                out += QString("%1. %2\n").arg(i+1).arg(names.at(i));
        }
    }
    else if(opt_str == "--pl-dump")
    {
        MetaDataFormatter formatter("%p%if(%p&%t, - ,)%t%if(%p,,%if(%t,,%f))%if(%l, - %l,)");
        int id = args.isEmpty() ? pl_manager->currentPlayListIndex() : args.at(0).toInt() - 1;
        PlayListModel *model = pl_manager->playListAt(id);
        if(!model)
            return tr("Invalid playlist ID") + "\n";
        for(int i = 0; i < model->count(); ++i)
        {
            out += QString("%1. %2").arg(i+1).arg(formatter.parse(model->item(i)));
            if(i == model->currentRow())
                out += " [*]";
            out += "\n";
        }
    }
    else if(opt_str == "--pl-clear")
    {
        int id = args.isEmpty() ? pl_manager->currentPlayListIndex() : args.at(0).toInt() - 1;
        PlayListModel *model = pl_manager->playListAt(id);
        if(!model)
            return tr("Invalid playlist ID") + "\n";
        model->clear();
    }
    else if(opt_str == "--pl-repeat-toggle")
    {
        player->setRepeatable(!player->isRepeatable());
    }
    else if(opt_str == "--pl-shuffle-toggle")
    {
        pl_manager->setShuffle(!pl_manager->isShuffle());
    }
    else if(opt_str == "--pl-state")
    {
        out += "SHUFFLE:             " + boolToText(pl_manager->isShuffle()) + "\n";
        out += "REPEAT PLAYLIST:     " + boolToText(pl_manager->isRepeatableList()) + "\n";
        out += "REPEAT TRACK:        " + boolToText(player->isRepeatable()) + "\n";
        out += "NO PLAYLIST ADVANCE: " + boolToText(player->isNoPlaylistAdvance()) + "\n";
    }
    return out;
}

const QString PlayListOption::name() const
{
    return "PlayListOption";
}

QTranslator *PlayListOption::createTranslator(QObject *parent)
{
    QTranslator *translator = new QTranslator(parent);
    QString locale = Qmmp::systemLanguageID();
    translator->load(QString(":/playlist_plugin_") + locale);
    return translator;
}

QString PlayListOption::boolToText(bool enabled)
{
    return QString(enabled ? "[+]" : "[-]");
}

Q_EXPORT_PLUGIN2(playlistoption, PlayListOption)
