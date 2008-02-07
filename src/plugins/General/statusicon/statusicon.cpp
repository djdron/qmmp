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

#include "statusicon.h"

StatusIcon::StatusIcon(QObject *parent)
        : General(parent)
{
    m_tray = new QSystemTrayIcon(this);
    connect(m_tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayActivated(QSystemTrayIcon::ActivationReason)));
    m_tray->setIcon ( QIcon(":/tray_stop.png"));
    m_tray->show();
    QMenu *menu = new QMenu(qobject_cast<QWidget *>(parent));
    menu->addAction(tr("Play"), this, SLOT(play()));
    menu->addAction(tr("Pause"), this, SLOT(pause()));
    menu->addAction(tr("Stop"), this, SLOT(stop()));
    menu->addAction(tr("Next"), this, SLOT(next()));
    menu->addAction(tr("Previous"), this, SLOT(previous()));
    menu->addSeparator();
    menu->addAction(tr("Exit"), this, SLOT(exit()));
    m_tray->setContextMenu(menu);

    QSettings settings(QDir::homePath()+"/.qmmp/qmmprc", QSettings::IniFormat);
    settings.beginGroup("Tray");
    m_showMessage = settings.value("show_message",TRUE).toBool();
    m_messageDelay = settings.value("message_delay", 2000).toInt();
    m_showTooltip = settings.value("show_tooltip",FALSE).toBool();
    m_hideToTray = settings.value("hide_on_close", FALSE).toBool();
    settings.endGroup();
    m_enabled = FALSE;
    QTimer::singleShot(200, this, SLOT(enable()));

}


StatusIcon::~StatusIcon()
{}

void StatusIcon::setState(const uint &state)
{
    switch ((uint) state)
    {
    case General::Playing:
    {
        m_tray->setIcon(QIcon(":/tray_play.png"));
        break;
    }
    case General::Paused:
    {
        m_tray->setIcon(QIcon(":/tray_pause.png"));
        break;
    }
    case General::Stopped:
    {
        m_tray->setIcon(QIcon(":/tray_stop.png"));
        break;
    }
    }
}

void StatusIcon::setSongInfo(const SongInfo &song)
{
    if(!m_enabled)
        return;
    QString message = song.artist() + " - " +song.title();
    if (song.artist().isEmpty())
        message = song.title();
    if (song.title().isEmpty())
        message = song.artist();
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
}

void StatusIcon::enable()
{
    m_enabled = TRUE;
}
