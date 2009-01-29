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

#include <QtDBus>
#include <QActionGroup>

#include <qmmpui/generalhandler.h>
#include <qmmpui/mediaplayer.h>
#include <qmmpui/playlistmodel.h>
#include "haldevice.h"
#include "halmanager.h"
#include "halplugin.h"

HalPlugin::HalPlugin(QObject *parent)
        : General(parent)
{
    m_manager = new HalManager(this);
    m_actions = new QActionGroup(this);
    connect(m_manager,SIGNAL(deviceAdded(const QString &)), SLOT(addDevice(const QString &)));
    connect(m_manager,SIGNAL(deviceRemoved(const QString &)), SLOT(removeDevice(const QString &)));
    connect(m_actions,SIGNAL(triggered (QAction *)), SLOT(processAction(QAction *)));
    //find existing devices
    QStringList udis = m_manager->findDeviceByCapability("volume");
    foreach(QString udi, udis)
    addDevice(udi);
}


HalPlugin::~HalPlugin()
{
}

void HalPlugin::removeDevice(const QString &udi)
{
    foreach(HalDevice *device, m_devices)
    {
        if (device->udi() == udi)
        {
            m_devices.removeAll(device);
            delete device;
            qDebug("HalPlugin: device \"%s\" removed", qPrintable(udi));
            updateActions();
            break;
        }
    }
}

void HalPlugin::addDevice(const QString &udi)
{
    foreach(HalDevice *device, m_devices) //is it already exists?
    {
        if (device->udi() == udi)
            return;
    }
    HalDevice *device = new HalDevice(udi, this);
    QStringList caps = device->property("info.capabilities").toStringList();
    if (!caps.contains("block") || !caps.contains("volume") ||
            device->property("info.category").toString() != "volume" ||
            device->property("info.volume.ignore").toBool()) //filter unsupported devices
    {
        delete device;
        return;
    }
    //audio cd
    if (caps.contains("volume.disc") && device->property("volume.disc.has_audio").toBool())
    {
        qDebug("HalPlugin: device \"%s\" added (cd audio)", qPrintable(udi));
        m_devices << device;
        updateActions();
        return;
    }

    HalDevice parentDevice(device->property("info.parent").toString());

    caps = parentDevice.property("info.capabilities").toStringList();
    // filter removable devices
    if (!(caps.contains("storage") && parentDevice.property("storage.removable").toBool()))
    {
        delete device;
        return;
    }

    if (device->property("volume.size").toLongLong() < 5000000000LL &&
            (device->property("volume.fstype").toString() == "vfat" ||
             device->property("volume.fstype").toString() == "iso" ||
             device->property("volume.fstype").toString() == "udf" ||
             device->property("volume.fstype").toString() == "ext2"))
    {
        qDebug("HalPlugin: device \"%s\" added (removable)", qPrintable(udi));
        m_devices << device;
        updateActions();
        connect(device, SIGNAL(propertyModified(int, const QList<ChangeDescription> &)),
                SLOT(updateActions()));
        return;
    }
    delete device;
}

void HalPlugin::updateActions()
{
    // add action for cd audio or mounted volume
    foreach(HalDevice *device, m_devices)
    {
        QStringList caps = device->property("info.capabilities").toStringList();
        QString dev_path;
        if (caps.contains("volume.disc") && device->property("volume.disc.has_audio").toBool()) //cd audio
            dev_path = "cdda://" + device->property("block.device").toString();
        else if (device->property("volume.is_mounted").toBool()) //mounted volume
            dev_path = device->property("volume.mount_point").toString();
        else
            continue;

        if (!findAction(dev_path))
        {
            QAction *action = new QAction(this);
            QString actionText;
            if (caps.contains("volume.disc") && device->property("volume.disc.has_audio").toBool())
                actionText = QString(tr("Add CD \"%1\"")).arg(device->property("block.device").toString());
            else
            {
                QString name = device->property("volume.label").toString();
                if (name.isEmpty())
                    name = dev_path;
                actionText = QString(tr("Add Volume \"%1\"")).arg(name);
            }
            action->setText(actionText);
            action->setData(dev_path);
            m_actions->addAction(action);
            GeneralHandler::instance()->addAction(action, GeneralHandler::TOOLS_MENU);
        }
    }
    // remove action if device is unmounted/removed
    foreach(QAction *action, m_actions->actions ())
    {
        if (!findDevice(action))
        {
            m_actions->removeAction(action);
            GeneralHandler::instance()->removeAction(action);
            action->deleteLater();
        }
    }
}

void HalPlugin::processAction(QAction *action)
{
    qDebug("HalPlugin: action triggered: %s", qPrintable(action->data().toString()));
    QString path = action->data().toString();
    if (path.startsWith("cdda://"))
        MediaPlayer::instance()->playListModel()->addFile(path);
    else
        MediaPlayer::instance()->playListModel()->addDirectory(path);
}

QAction *HalPlugin::findAction(const QString &dev_path)
{
    foreach(QAction *action, m_actions->actions ())
    {
        if (action->data().toString() == dev_path)
            return action;
    }
    return 0;
}

HalDevice *HalPlugin::findDevice(QAction *action)
{
    foreach(HalDevice *device, m_devices)
    {
        QStringList caps = device->property("info.capabilities").toStringList();
        QString dev_path;
        if (caps.contains("volume.disc") && device->property("volume.disc.has_audio").toBool())
        {
            dev_path = "cdda://" + device->property("block.device").toString();
            if (dev_path == action->data().toString())
                return device;
        }
        if (device->property("volume.is_mounted").toBool())
        {
            dev_path = device->property("volume.mount_point").toString();
            if (dev_path == action->data().toString())
                return device;
        }
    }
    return 0;
}
