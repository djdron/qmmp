/***************************************************************************
 *   Copyright (C) 2009 by Artur Guzik                                     *
 *   a.guzik88@gmail.com                                                   *
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

#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusInterface>
#include <QList>
#include <QStringList>
#include <QVariant>
#include <QFileInfo>
#include <QDir>
#include <QSettings>
#include <qmmp/soundcore.h>
#include <qmmp/metadatamanager.h>
#include "kdenotify.h"

KdeNotify::KdeNotify(QObject *parent) : General(parent)
{
    notifier = new QDBusInterface("org.kde.VisualNotifications",
                                  "/VisualNotifications", "org.kde.VisualNotifications");
    if(notifier->isValid())
    {
        qWarning("KdeNotify: unable to create dbus interface");
        return;
    }
    QSettings settings(Qmmp::configFile(),QSettings::IniFormat);
    settings.beginGroup("Kde_Notifier");
    m_NotifyDelay = settings.value("notify_delay",10000).toInt();
    settings.endGroup();
    connect(SoundCore::instance(),SIGNAL(metaDataChanged()), SLOT(showMetaData()));
}

KdeNotify::~KdeNotify()
{
    delete notifier;
}

QString KdeNotify::totalTimeString()
{
    SoundCore * core = SoundCore::instance();
    int second = core->totalTime() / 1000;
    int minute = second / 60;
    int hour = minute / 60;

    if(core->totalTime() >= 3600000)
    {
        return QString("%1:%2:%3").arg(hour,2,10,QChar('0')).arg(minute%60,2,10,QChar('0'))
                                  .arg(second%60,2,10,QChar('0'));
    }
    return QString("%1:%2").arg(minute%60,2,10,QChar('0')).arg(second%60,2,10,QChar('0'));
}

void KdeNotify::showMetaData()
{
    SoundCore *core = SoundCore::instance();
    QList<QVariant> args;
    QString body(""); //metadata set
    QString title(core->metaData(Qmmp::TITLE));
    QString artist(core->metaData(Qmmp::ARTIST));
    QString album(core->metaData(Qmmp::ALBUM));

    args.append("Qmmp"); //app-name
    args.append(0U); //replaces-id
    args.append(""); //event-id
    args.append(QFileInfo(Qmmp::configFile()).absoluteDir().path() + "/app_icon.png"); //app-icon(path to icon on disk)
   args.append(tr("Qmmp now playing:")); //summary (notification title)

    if(title.isEmpty())
    {
        title = QFileInfo(core->metaData(Qmmp::URL)).completeBaseName();
    }
    body.append("<b>" + title + "</b> (" + totalTimeString() +")<br>");

    if(!artist.isEmpty())
    {
        body.append(tr("by ") + artist + "<br>");
    }

    if(!album.isEmpty())
    {
        body.append(tr("on ") + album);
    }

    QString nBody;
    QString coverPath = MetaDataManager::instance()->getCoverPath(core->metaData(Qmmp::URL));

    if(coverPath.isEmpty())
    {
        coverPath = QFileInfo(Qmmp::configFile()).absoluteDir().path() + "/empty_cover.png";
    }

    nBody.append("<table padding=\"3px\"><tr><td width=\"80px\" height=\"80px\" padding=\"3px\">");
    nBody.append("<img height=\"80\" width=\"80\" src=\"%1\"></td><td width=\"10\"></td><td>%2</td></tr><table>");
    nBody = nBody.arg(coverPath,body);

    args.append(nBody); //body
    args.append(QStringList()); //actions
    args.append(QVariantMap()); //hints
    args.append(m_NotifyDelay); //timeout

    notifier->callWithArgumentList(QDBus::AutoDetect,"Notify",args);
}
