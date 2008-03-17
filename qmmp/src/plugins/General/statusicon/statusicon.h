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
#ifndef STATUSICON_H
#define STATUSICON_H

#include <QSystemTrayIcon>

#include <qmmpui/general.h>

class Control;

/**
	@author Ilya Kotov <forkotov02@hotmail.ru>
*/

class StatusIcon : public General
{
Q_OBJECT
public:
    StatusIcon(Control *control, QObject *parent = 0);

    ~StatusIcon();

    void setState(const uint& state);
    void setSongInfo(const SongInfo &song);

private slots:
    void trayActivated(QSystemTrayIcon::ActivationReason);
    void enable();

private:
    QSystemTrayIcon *m_tray;
    bool m_showMessage;
    bool m_showTooltip;
    bool m_hideToTray;
    bool m_enabled;
    int m_messageDelay;
    Control *m_control;

};

#endif
