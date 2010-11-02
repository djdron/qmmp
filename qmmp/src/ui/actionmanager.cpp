/***************************************************************************
 *   Copyright (C) 2010 by Ilya Kotov                                      *
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

#include <QSettings>
#include <QAction>
#include <QIcon>
#include <QFile>
#include <qmmp/qmmp.h>
#include "actionmanager.h"

ActionManager *ActionManager::m_instance = 0;

ActionManager::ActionManager(QObject *parent) :
    QObject(parent)
{
    m_instance = this;
    m_settings = new QSettings(Qmmp::configFile(), QSettings::IniFormat);
    m_settings->beginGroup("Shortcuts");
    //playback
    m_actions[PLAY] = createAction(tr("&Play"), "play", tr("X"), "media-playback-start");
    m_actions[PAUSE] = createAction(tr("&Pause"), "pause", tr("C"), "media-playback-pause");
    m_actions[STOP] = createAction(tr("&Stop"), "stop", tr("V"), "media-playback-stop");
    m_actions[PREVIOUS] = createAction(tr("&Previous"), "previous", tr("Z"), "media-skip-backward");
    m_actions[NEXT] = createAction(tr("&Next"), "next", tr("B"), "media-skip-forward");
    m_actions[PLAY_PAUSE] = createAction(tr("&Play/Pause"), "play_pause", tr("Space"));
    m_actions[JUMP] = createAction(tr("&Jump to File"), "jump", tr("J"), "go-up");
    m_actions[REPEAT_ALL] = createAction2(tr("&Repeat Playlist"), "repeate_playlist", tr("R"));
    m_actions[REPEAT_TRACK] = createAction2(tr("&Repeat Track"), "repeate_track", tr("Ctrl+R"));
    m_actions[SHUFFLE] = createAction2(tr("&Shuffle"), "shuffle", tr("S"));
    m_actions[NO_PL_ADVANCE] = createAction2(tr("&No Playlist Advance"), "no_playlist_advance",
                                            tr("Ctrl+N"));
    m_actions[STOP_AFTER_SELECTED] = createAction(tr("&Stop After Selected"), "stop_after_selected",
                                                  tr("Ctrl+S"));
    m_actions[CLEAR_QUEUE] = createAction(tr("&Clear Queue"), "clear_queue", tr("Alt+Q"));

    m_settings->endGroup();
    delete m_settings;
    m_settings = 0;
}

ActionManager::~ActionManager()
{
    m_instance = 0;
}

QAction *ActionManager::action(int type)
{
    return m_actions[type];
}

QAction *ActionManager::use(int type, const QObject *receiver, const char *member)
{
    QAction *act = m_actions[type];
    connect(act,SIGNAL(triggered(bool)), receiver, member);
    return act;
}

ActionManager* ActionManager::instance()
{
    return m_instance;
}

QAction *ActionManager::createAction(QString name, QString confKey, QString key, QString iconName)
{
    QAction *action = new QAction(name, this);
    action->setShortcut(m_settings->value(confKey, key).toString());
    action->setObjectName(confKey);
    if(iconName.isEmpty())
        return action;
    if(QFile::exists(iconName))
        action->setIcon(QIcon(iconName));
    else
        action->setIcon(QIcon::fromTheme(iconName));
    return action;
}

QAction *ActionManager::createAction2(QString name, QString confKey, QString key)
{
    QAction *action = createAction(name, confKey, key);
    action->setCheckable(true);
    return action;
}
