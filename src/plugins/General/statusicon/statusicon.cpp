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

#include "qmmptrayicon.h"
#include "statusicon.h"

StatusIcon::StatusIcon(QObject *parent)
        : General(parent)
{
    m_tray = new QmmpTrayIcon(this);
    connect(m_tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayActivated(QSystemTrayIcon::ActivationReason)));
    //m_tray->show();
    m_core = SoundCore::instance();
    m_player = MediaPlayer::instance();
    QMenu *menu = new QMenu(qobject_cast<QWidget *>(parent));
    menu->addAction(tr("Play"), m_player, SLOT(play()));
    menu->addAction(tr("Pause"), m_core, SLOT(pause()));
    menu->addAction(tr("Stop"), m_core, SLOT(stop()));
    menu->addAction(tr("Next"), m_player, SLOT(next()));
    menu->addAction(tr("Previous"), m_player, SLOT(previous()));
    menu->addSeparator();
    menu->addAction(tr("Exit"), this, SLOT(exit()));
    m_tray->setContextMenu(menu);

    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("Tray");
    m_showMessage = settings.value("show_message",TRUE).toBool();
    m_messageDelay = settings.value("message_delay", 2000).toInt();
    m_showTooltip = settings.value("show_tooltip",FALSE).toBool();
    m_hideToTray = settings.value("hide_on_close", FALSE).toBool();
    m_useStandardIcons = settings.value("use_standard_icons",FALSE).toBool();
#if QT_VERSION >= 0x040400
    if(m_useStandardIcons)
        m_tray->setIcon(QApplication::style ()->standardIcon(QStyle::SP_MediaStop));
    else
#endif
        m_tray->setIcon ( QIcon(":/tray_stop.png"));
    m_tray->show();
    settings.endGroup();
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
#if QT_VERSION >= 0x040400
         if(m_useStandardIcons)
            m_tray->setIcon(QApplication::style ()->standardIcon(QStyle::SP_MediaPlay));
        else
#endif
            m_tray->setIcon ( QIcon(":/tray_play.png"));
        break;
    }
    case Qmmp::Paused:
    {
#if QT_VERSION >= 0x040400
         if(m_useStandardIcons)
            m_tray->setIcon(QApplication::style ()->standardIcon(QStyle::SP_MediaPause));
        else
#endif
            m_tray->setIcon ( QIcon(":/tray_pause.png"));
        break;
    }
    case Qmmp::Stopped:
    {
#if QT_VERSION >= 0x040400
        if(m_useStandardIcons)
            m_tray->setIcon(QApplication::style ()->standardIcon(QStyle::SP_MediaStop));
        else
#endif
            m_tray->setIcon ( QIcon(":/tray_stop.png"));
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
    if (m_showTooltip)
        m_tray->setToolTip(message);
}

void StatusIcon::trayActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Trigger)
        toggleVisibility();
    else if (reason == QSystemTrayIcon::MiddleClick)
    {
        if (SoundCore::instance()->state() == Qmmp::Stopped)
            m_player->play();
        else
            m_core->pause();
    }
}
