#include "kdenotify.h"
#include <qmmp/soundcore.h>
#include <qmmp/metadatamanager.h>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusInterface>
#include <QList>
#include <QStringList>
#include <QVariant>
#include <QFileInfo>
#include <QDir>
#include <QSettings>

KdeNotify::KdeNotify(QObject *parent) : General(parent)
{
    if (!QDBusConnection::sessionBus ().interface ()->isServiceRegistered ("org.kde.VisualNotifications"))
    {
        qWarning("KdeNotify: org.kde.VisualNotifications is not registered");
        return;
    }
    notifier = new QDBusInterface("org.kde.VisualNotifications",
                                  "/VisualNotifications", "org.kde.VisualNotifications");
    QSettings settings(Qmmp::configFile(),QSettings::IniFormat);
    settings.beginGroup("Kde_Notifier");
    m_NotifyDelay = settings.value("notify_delay",10000).toInt();
    settings.endGroup();
    connect(SoundCore::instance(),SIGNAL(metaDataChanged()),this,SLOT(showMetaData()));
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
