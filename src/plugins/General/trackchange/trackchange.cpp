/***************************************************************************
 *   Copyright (C) 2013 by Ilya Kotov                                      *
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

#include <QAction>
#include <QSettings>
#include <QApplication>
#include <QSignalMapper>
#include <QProgressDialog>
#include <QMessageBox>
#include <QFile>
#include <QDir>
#include <QProcess>
#include <qmmp/soundcore.h>
#include <qmmpui/uihelper.h>
#include <qmmpui/playlistmodel.h>
#include <qmmpui/playlistmanager.h>
#include <qmmpui/playlistitem.h>
#include <qmmpui/mediaplayer.h>
#include <qmmpui/metadataformatter.h>
#include "songchange.h"

SongChange::SongChange(QObject *parent) : QObject(parent)
{
    m_core = SoundCore::instance();
    m_plManager = PlayListManager::instance();
    connect(m_core, SIGNAL(stateChanged(Qmmp::State)), SLOT(onStateChanged(Qmmp::State)));
    connect(m_core, SIGNAL(metaDataChanged()), SLOT(onMetaDataChanged()));
    connect(m_core, SIGNAL(finished()), SLOT(onFinised()));
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    m_newTrackCommand = settings.value("SongChange/new_track_command").toString();
    m_endOfTrackCommand = settings.value("SongChange/end_of_track_command").toString();
    m_endOfPlCommand = settings.value("SongChange/end_of_pl_command").toString();
    m_titleChangeCommand = settings.value("SongChange/title_change_command").toString();
}

SongChange::~SongChange()
{}

void SongChange::onStateChanged(Qmmp::State state)
{
    switch (state)
    {
    case Qmmp::Playing:
        break;
    default:
        m_prevMetaData.clear();
    }
}

void SongChange::onMetaDataChanged()
{
    QMap <Qmmp::MetaData, QString> metaData = m_core->metaData();
    if(m_prevMetaData != metaData)
    {
        if(m_prevMetaData[Qmmp::URL] == metaData[Qmmp::URL])
        {
            if(!m_titleChangeCommand.isEmpty())
            {
                qDebug("SongChange: startig title change command..");
                executeCommand(metaData, m_titleChangeCommand);
            }
        }
        else
        {
            if(!m_newTrackCommand.isEmpty())
            {
                qDebug("SongChange: startig new track command..");
                executeCommand(metaData, m_newTrackCommand);
            }
        }
    }
    m_prevMetaData = metaData;
}

void SongChange::onFinised()
{
    if(!m_endOfTrackCommand.isEmpty())
    {
        qDebug("SongChange: startig end of track command..");
        executeCommand(m_prevMetaData, m_endOfTrackCommand);
    }
    if(!m_endOfPlCommand.isEmpty() && !m_plManager->currentPlayList()->nextItem())
    {
        qDebug("SongChange: startig end of playlist command..");
        executeCommand(m_prevMetaData, m_endOfPlCommand);
    }
}

bool SongChange::executeCommand(const QMap<Qmmp::MetaData, QString> &metaData, const QString &format)
{
    MetaDataFormatter formatter(format);
    QString command = formatter.parse(metaData);
#ifdef Q_OS_WIN
    bool ok = QProcess::startDetached(QString("cmd.exe \"%1\"").arg(command));
#else
    bool ok = QProcess::startDetached(QString("sh -c \"%1\"").arg(command));
#endif
    if(!ok)
        qWarning("SongChange: unable to start command '%s'", qPrintable(command));
    return ok;
}
