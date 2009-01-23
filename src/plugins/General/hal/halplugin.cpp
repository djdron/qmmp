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
    {
        qDebug(qPrintable(udi));
        addDevice(udi);
    }
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
    if (device->property("info.category").toString() == "volume"
            && device->property("volume.disc.has_audio").toBool()) //add audio cd only TODO usb devices support
    {
        qDebug("HalPlugin: device \"%s\" added (cd audio)", qPrintable(udi));
        m_devices << device;
        updateActions();
        return;
    }
    delete device;
}

void HalPlugin::updateActions()
{
    // add device
    foreach(HalDevice *device, m_devices)
    {
        QString dev_path = "cdda://" + device->property("block.device").toString();
        bool exists = FALSE;
        foreach(QAction *action, m_actions->actions ())
        {
            if (action->data().toString() == dev_path)
            {
                exists = TRUE;
                break;
            }
        }
        if (!exists)
        {
            QAction *action = new QAction(QString(tr("Add CD (%1)"))
                                          .arg(device->property("block.device").toString()), this);
            action->setData(dev_path);
            m_actions->addAction(action);
            GeneralHandler::instance()->addAction(action, GeneralHandler::TOOLS_MENU);
        }
    }
    // remove device
    foreach(QAction *action, m_actions->actions ())
    {
        bool exists = FALSE;
        foreach(HalDevice *device, m_devices)
        {
            QString dev_path = "cdda://" + device->property("block.device").toString();
            if (dev_path == action->data().toString())
            {
                exists = TRUE;
                break;
            }
        }
        if (!exists)
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
    MediaPlayer::instance()->playListModel()->addFile(action->data().toString());
}
