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

#include <QTimer>
#include <QFile>
#include <QDir>
#include <QSettings>
#include <qmmp/soundcore.h>

#include "popupwidget.h"
#include "notifier.h"

Notifier::Notifier(QObject *parent)
        : General(parent)
{
    m_popupWidget = 0;
    m_l = -1;
    m_r = -1;
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("Notifier");
    m_desktop = settings.value("song_notification", true).toBool();
    m_showVolume = settings.value("volume_notification", true).toBool();
    m_psi = settings.value("psi_notification", false).toBool();
    settings.endGroup();
    m_core = SoundCore::instance();
    connect (m_core, SIGNAL(metaDataChanged ()), SLOT(showMetaData()));
    connect (m_core, SIGNAL(stateChanged (Qmmp::State)), SLOT(setState(Qmmp::State)));
    connect (m_core, SIGNAL(volumeChanged(int, int)), SLOT(showVolume(int, int)));
    if(m_psi) //clear psi notification
    {
        QFile::remove(QDir::homePath()+"/.psi/tune");
        QFile::remove(QDir::homePath()+"/.psi-plus/tune");
    }
    if (m_core->state() == Qmmp::Playing) //test message
        showMetaData();
}

Notifier::~Notifier()
{
    if(m_psi) //clear psi notification
    {
        QFile::remove(QDir::homePath()+"/.psi/tune");
        QFile::remove(QDir::homePath()+"/.psi-plus/tune");
    }
}

void Notifier::setState(Qmmp::State state)
{
    switch ((uint) state)
    {
    case Qmmp::Playing:
    case Qmmp::Paused:
    {
        break;
    }
    case Qmmp::Stopped:
    {
        if (m_psi)
        {
            QFile::remove(QDir::homePath()+"/.psi/tune");
            QFile::remove(QDir::homePath()+"/.psi-plus/tune");
        }
        break;
    }
    }
}

void Notifier::showMetaData()
{
    if (m_desktop)
    {
        if (!m_popupWidget)
            m_popupWidget = new PopupWidget();
        m_popupWidget->showMetaData();
    }

    if (!m_psi)
        return;

    QByteArray data;
    data.append(m_core->metaData(Qmmp::TITLE).toUtf8()+"\n");
    data.append(m_core->metaData(Qmmp::ARTIST).toUtf8()+"\n");
    data.append(m_core->metaData(Qmmp::ALBUM).toUtf8()+"\n");
    data.append(m_core->metaData(Qmmp::TRACK).toUtf8()+"\n");
    data.append(QString("%1").arg(m_core->totalTime()/1000).toUtf8()+"\n");

    QDir psi_dir(QDir::homePath()+"/.psi/");
    if(psi_dir.exists())
    {
        QFile file(QDir::homePath()+"/.psi/tune");
        file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
        file.write(data);
        file.close();
    }

    QDir psi_plus_dir(QDir::homePath()+"/.psi-plus/");
    if(psi_plus_dir.exists())
    {
        QFile file(QDir::homePath()+"/.psi-plus/tune");
        file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
        file.write(data);
        file.close();
    }

}

void Notifier::showVolume(int l, int r)
{
    if (((m_l != l) || (m_r != r)) && m_showVolume)
    {
        if (m_l >= 0)
        {
            if (!m_popupWidget)
                m_popupWidget = new PopupWidget();
            m_popupWidget->showVolume(qMax(l,r));
        }
        m_l = l;
        m_r = r;
    }
}
