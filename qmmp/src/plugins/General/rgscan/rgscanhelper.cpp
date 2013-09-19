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
#include <QApplication>
#include <QProgressDialog>
#include <qmmp/soundcore.h>
#include <qmmpui/uihelper.h>
#include <qmmpui/playlistmanager.h>
#include <qmmpui/playlistitem.h>
#include <qmmpui/mediaplayer.h>
#include "rgscaner.h"
#include "rgscandialog.h"
#include "rgscanhelper.h"

RGScanHelper::RGScanHelper(QObject *parent) : QObject(parent)
{
    m_action = new QAction(tr("Calculate ReplayGain"), this);
    m_action->setShortcut(tr("Meta+R"));
    UiHelper::instance()->addAction(m_action, UiHelper::PLAYLIST_MENU);
    connect (m_action, SIGNAL(triggered ()), SLOT(openRGScaner()));
    //m_scaner = new RGScaner(this);
    m_progress = new QProgressDialog();
    m_progress->setRange(0,100);
    m_progress->setWindowTitle(tr("Scaning..."));
    m_progress->setCancelButtonText(tr("Cancel"));
    //connect(m_scaner,SIGNAL(progress(int)),m_progress,SLOT(setValue(int)));
    //connect(m_scaner, SIGNAL(finished()), m_progress, SLOT(reset()));
    //connect(m_scaner, SIGNAL(desriptionChanged(QString)), m_progress, SLOT(setLabelText(QString)));
    //connect(m_progress, SIGNAL(canceled()), m_scaner, SLOT(stop()));
}

RGScanHelper::~RGScanHelper()
{
    delete m_progress;
}

void RGScanHelper::openRGScaner()
{
    PlayListManager *pl_manager = MediaPlayer::instance()->playListManager();
    QList <PlayListTrack *> tracks = pl_manager->selectedPlayList()->selectedTracks();
    if (tracks.isEmpty())
        return;

    RGScanDialog *d = new RGScanDialog(tracks, qApp->activeWindow ());
    d->exec();
    d->deleteLater();
}
