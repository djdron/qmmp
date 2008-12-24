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
    QFile::remove(QDir::homePath()+"/.psi/tune");
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("Notifier");
    m_desktop = settings.value("desktop_notification", TRUE).toBool();
    m_psi = settings.value("psi_notification", FALSE).toBool();
    settings.endGroup();
    m_core = SoundCore::instance();
    connect (m_core, SIGNAL(metaDataChanged ()), SLOT(showMetaData()));
    connect (m_core, SIGNAL(stateChanged (Qmmp::State)), SLOT(setState(Qmmp::State)));
}

Notifier::~Notifier()
{}

void Notifier::setState(Qmmp::State state)
{
   switch ((uint) state)
    {
    case Qmmp::Playing:
    {
        break;
    }
    case Qmmp::Paused:
    {
        break;
    }
    case Qmmp::Stopped:
    {
        QFile::remove(QDir::homePath()+"/.psi/tune");
        break;
    }
    }
}

void Notifier::showMetaData()
{
    if (m_popupWidget)
        delete m_popupWidget;
    if(m_desktop)
        m_popupWidget = new PopupWidget();
    if(!m_psi)
        return;
    QFile file(QDir::homePath()+"/.psi/tune");   //psi file
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    file.write(m_core->metaData(Qmmp::TITLE).toUtf8()+"\n");
    file.write(m_core->metaData(Qmmp::ARTIST).toUtf8()+"\n");
    file.write(m_core->metaData(Qmmp::ALBUM).toUtf8()+"\n");
    file.write(m_core->metaData(Qmmp::TRACK).toUtf8()+"\n");
    file.write(QString("%1").arg(m_core->length()).toUtf8()+"\n");
    file.close();
}
