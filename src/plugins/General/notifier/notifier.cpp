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

#include "popupwidget.h"
#include "notifier.h"

Notifier::Notifier(QObject *parent)
        : General(parent)
{
    m_popupWidget = 0;
    QFile::remove(QDir::homePath()+"/.psi/tune");
    QSettings settings(QDir::homePath()+"/.qmmp/qmmprc", QSettings::IniFormat);
    settings.beginGroup("Notifier");
    m_desktop = settings.value("desktop_notification", TRUE).toBool();
    m_psi = settings.value("psi_notification", FALSE).toBool();
    settings.endGroup();
}

Notifier::~Notifier()
{}

void Notifier::setState(const uint &state)
{
    switch ((uint) state)
    {
    case General::Playing:
    {
        break;
    }
    case General::Paused:
    {
        break;
    }
    case General::Stopped:
    {
        QFile::remove(QDir::homePath()+"/.psi/tune");
        break;
    }
    }
}

void Notifier::setSongInfo(const SongInfo &song)
{
    if (m_popupWidget)
        delete m_popupWidget;
    if(m_desktop)
        m_popupWidget = new PopupWidget(song);
    if(!m_psi)
        return;
    QFile file(QDir::homePath()+"/.psi/tune");   //psi file
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    file.write(song.title().toUtf8()+"\n");
    file.write(song.artist().toUtf8()+"\n");
    file.write(song.album().toUtf8()+"\n");
    file.write(QString("%1").arg(song.track()).toUtf8()+"\n");
    file.write(QString("%1").arg(song.length()).toUtf8()+"\n");
    file.close();
}
