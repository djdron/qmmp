/***************************************************************************
 *   Copyright (C) 2009 by Ilya Kotov                                      *
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

#define SCROBBLER_LASTFM_URL "post.audioscrobbler.com"
#define SCROBBLER_LIBREFM_URL "turtle.libre.fm"

#include <QSettings>
#include "scrobbler.h"
#include "scrobblerhandler.h"

ScrobblerHandler::ScrobblerHandler(QObject *parent) : General(parent)
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("Scrobbler");
    if(settings.value("use_lastfm", FALSE).toBool())
    {
        new Scrobbler(SCROBBLER_LASTFM_URL, settings.value("lastfm_login").toString(),
                   settings.value("lastfm_password").toString(), "lastfm", this);

    }
    if(settings.value("use_librefm", FALSE).toBool())
    {
        new Scrobbler(SCROBBLER_LIBREFM_URL, settings.value("librefm_login").toString(),
                   settings.value("librefm_password").toString(), "librefm", this);

    }
    settings.endGroup();
}

ScrobblerHandler::~ScrobblerHandler()
{}
