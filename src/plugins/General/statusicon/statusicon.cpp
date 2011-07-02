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

#include <QMenu>
#include <QSettings>
#include <QDir>
#include <QTimer>
#include <QCoreApplication>
#include <QWheelEvent>
#include <QEvent>
#include <QStyle>
#include <QApplication>

#include <qmmp/soundcore.h>
#include <qmmpui/mediaplayer.h>
#include <qmmpui/generalhandler.h>

#include "qmmptrayicon.h"
#include "statusicon.h"

StatusIcon::StatusIcon(QObject *parent)
        : General(parent)
{
    m_tray = new QmmpTrayIcon(this);
    connect(m_tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), SLOT(trayActivated(QSystemTrayIcon::ActivationReason)));
    //m_tray->show();
    m_core = SoundCore::instance();
    m_player = MediaPlayer::instance();
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("Tray");
    m_showMessage = settings.value("show_message",true).toBool();
    m_messageDelay = settings.value("message_delay", 2000).toInt();
    m_hideToTray = settings.value("hide_on_close", false).toBool();
    m_useStandardIcons = settings.value("use_standard_icons",false).toBool();
    m_tray->showNiceToolTip(settings.value("show_nicetooltip",true).toBool());
    if(m_useStandardIcons)
        m_tray->setIcon(QApplication::style ()->standardIcon(QStyle::SP_MediaStop));
    else
        m_tray->setIcon ( QIcon(":/tray_stop.png"));
    m_tray->show();
    settings.endGroup();
    //actions
    QMenu *menu = new QMenu(qobject_cast<QWidget *>(parent));
    QIcon playIcon = QApplication::style()->standardIcon(QStyle::SP_MediaPlay);
    QIcon pauseIcon = QApplication::style()->standardIcon(QStyle::SP_MediaPause);
    QIcon stopIcon = QApplication::style()->standardIcon(QStyle::SP_MediaStop);
    QIcon nextIcon = QApplication::style()->standardIcon(QStyle::SP_MediaSkipForward);
    QIcon previousIcon = QApplication::style()->standardIcon(QStyle::SP_MediaSkipBackward);
    menu->addAction(playIcon,tr("Play"), m_player, SLOT(play()));
    menu->addAction(pauseIcon,tr("Pause"), m_core, SLOT(pause()));
    menu->addAction(stopIcon,tr("Stop"), m_core, SLOT(stop()));
    menu->addSeparator();
    menu->addAction(nextIcon, tr("Next"), m_player, SLOT(next()));
    menu->addAction(previousIcon, tr("Previous"), m_player, SLOT(previous()));
    menu->addSeparator();
    menu->addAction(tr("Exit"), this, SLOT(exit()));
    m_tray->setContextMenu(menu);
    connect (m_core, SIGNAL(metaDataChanged ()), SLOT(showMetaData()));
    connect (m_core, SIGNAL(stateChanged (Qmmp::State)), SLOT(setState(Qmmp::State)));
    setState(m_core->state()); //update state
    if (m_core->state() == Qmmp::Playing) //show test message
        QTimer::singleShot(1500, this, SLOT(showMetaData()));
}


StatusIcon::~StatusIcon()
{}

void StatusIcon::setState(Qmmp::State state)
{
    switch ((uint) state)
    {
    case Qmmp::Playing:
    {
         if(m_useStandardIcons)
            m_tray->setIcon(QApplication::style ()->standardIcon(QStyle::SP_MediaPlay));
        else
            m_tray->setIcon (QIcon(":/tray_play.png"));
        break;
    }
    case Qmmp::Paused:
    {
         if(m_useStandardIcons)
            m_tray->setIcon(QApplication::style ()->standardIcon(QStyle::SP_MediaPause));
        else
            m_tray->setIcon (QIcon(":/tray_pause.png"));
        break;
    }
    case Qmmp::Stopped:
    {
        if(m_useStandardIcons)
            m_tray->setIcon(QApplication::style ()->standardIcon(QStyle::SP_MediaStop));
        else
            m_tray->setIcon (QIcon(":/tray_stop.png"));
        break;
    }
    }
}

void StatusIcon::showMetaData()
{
    QString message = m_core->metaData(Qmmp::ARTIST) + " - " + m_core->metaData(Qmmp::TITLE);
    if (message.startsWith (" - ") || message.endsWith (" - "))
        message.remove(" - ");
    if (message.isEmpty())
        message = m_core->metaData(Qmmp::URL).section('/',-1);

    if (m_showMessage)
        m_tray->showMessage (tr("Now Playing"), message,
                             QSystemTrayIcon::Information, m_messageDelay);
}

void StatusIcon::trayActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Trigger)
        GeneralHandler::instance()->toggleVisibility();
    else if (reason == QSystemTrayIcon::MiddleClick)
    {
        if (SoundCore::instance()->state() == Qmmp::Stopped)
            m_player->play();
        else
            m_core->pause();
    }
}
