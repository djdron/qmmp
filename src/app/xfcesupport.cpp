/***************************************************************************
 *   Copyright (C) 2015 by Ilya Kotov                                      *
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
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#include <QIcon>
#include <QProcess>
#include "xfcesupport.h"

void XFCESupport::load()
{
    if(qgetenv("XDG_CURRENT_DESKTOP") != "XFCE" && qgetenv("DESKTOP_SESSION") != "XFCE")
        return;

    QProcess process;
    process.start("xfconf-query -c xsettings -p /Net/IconThemeName");
    process.waitForFinished(3000);
    QString iconTheme = QString::fromLocal8Bit(process.readAllStandardOutput().trimmed());
    if(!iconTheme.isEmpty())
        QIcon::setThemeName(iconTheme);
}
