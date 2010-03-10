/***************************************************************************
 *   Copyright (C) 2009-2010 by Artur Guzik                                *
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
#include <QFileInfo>
#include <QDir>
#include <QSettings>
#include <QDebug>
#include <QTimer>
#include <QImage>

#include <qmmp/soundcore.h>
#include <qmmp/metadatamanager.h>
#include "kdenotify.h"

KdeNotify::KdeNotify(QObject *parent) : General(parent),m_UseFreedesktopSpec(false)
{
    notifier = new QDBusInterface("org.kde.VisualNotifications",
                                  "/VisualNotifications", "org.kde.VisualNotifications");
    if(notifier->lastError().type() != QDBusError::NoError)
    {
        delete(notifier);
        qWarning() << "KdeNotify: unable to create dbus interface."
                   << "Have you got KDE SC 4.4 or newer? Lets try...";

        notifier = new QDBusInterface("org.freedesktop.Notifications",
                                      "/org/freedesktop/Notifications","org.freedesktop.Notifications");
        if(notifier->lastError().type() != QDBusError::NoError)
        {
            qWarning() << "KdeNotify: Can't create interface. Sorry.";
            return;
        }
        m_UseFreedesktopSpec = true;
    }
    qWarning() << "KdeNotify: DBus interfece created successfully.";
    m_ConfigDir = QFileInfo(Qmmp::configFile()).absoluteDir().path();

    QSettings settings(Qmmp::configFile(),QSettings::IniFormat);
    settings.beginGroup("Kde_Notifier");
    m_NotifyDelay = settings.value("notify_delay",10000).toInt();
    m_ShowCovers = settings.value("show_covers",true).toBool();
    settings.endGroup();

    QTimer *timer = new QTimer(this);
    timer->setSingleShot(true);
    timer->setInterval(3000); //after that notification will be showed.
    connect(timer,SIGNAL(timeout()),SLOT(showMetaData()));
    connect(SoundCore::instance(),SIGNAL(metaDataChanged()),timer, SLOT(start()));
}

KdeNotify::~KdeNotify()
{
    QDir dir(QDir::home());
    dir.remove(m_ConfigDir + "/cover.jpg");
    delete notifier;
}

QString KdeNotify::totalTimeString()
{
    int time = SoundCore::instance()->totalTime()/1000;

    if(time >= 3600)
    {
	return QString("%1:%2:%3").arg(time/3600,2,10,QChar('0')).arg(time%3600/60,2,10,QChar('0'))
				  .arg(time%60,2,10,QChar('0'));
    }
    return QString("%1:%2").arg(time/60,2,10,QChar('0')).arg(time%60,2,10,QChar('0'));
}

QList<QVariant> KdeNotify::prepareNotification()
{
    SoundCore *core = SoundCore::instance();
    if(core->metaData(Qmmp::URL).isEmpty()) //prevent show empty notification
    {
	return QList<QVariant>();
    }
    QList<QVariant> args;
    QString body(""); //metadata set
    QString title(core->metaData(Qmmp::TITLE));
    QString artist(core->metaData(Qmmp::ARTIST));
    QString album(core->metaData(Qmmp::ALBUM));

    args.append("Qmmp"); //app-name
    args.append(0U); //replaces-id
    if(!m_UseFreedesktopSpec)
        args.append(""); //event-id
    args.append(m_ConfigDir + "/app_icon.png"); //app-icon(path to icon on disk)
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
    if(!coverPath.isEmpty() && m_ShowCovers)
    {
	QImage image(coverPath);
	image.scaled(100,100,Qt::IgnoreAspectRatio,Qt::SmoothTransformation).save(m_ConfigDir + "/cover.jpg","JPG");
	coverPath = m_ConfigDir + "/cover.jpg";
    }
    else if(coverPath.isEmpty() || !m_ShowCovers)
    {
	coverPath = m_ConfigDir + "/empty_cover.png";
    }

    nBody.append("<table padding=\"3px\"><tr><td width=\"80px\" height=\"80px\" padding=\"3px\">");
    nBody.append("<img height=\"80\" width=\"80\" src=\"%1\"></td><td width=\"10\"></td><td>%2</td></tr><table>");
    nBody = nBody.arg(coverPath,body);

    args.append(nBody); //body
    args.append(QStringList()); //actions
    args.append(QVariantMap()); //hints
    args.append(m_NotifyDelay); //timeout

    return args;
}

void KdeNotify::showMetaData()
{
    QList<QVariant> n = prepareNotification();
    if(!n.isEmpty())
	notifier->callWithArgumentList(QDBus::NoBlock,"Notify",n);
}
