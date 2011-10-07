/***************************************************************************
 *   Copyright (C) 2011 by Ilya Kotov                                      *
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

#include <QAction>
#include <QApplication>
#include <QProgressDialog>
#include <qmmp/soundcore.h>
#include <qmmpui/uihelper.h>
#include <qmmpui/playlistmanager.h>
#include <qmmpui/playlistitem.h>
#include <qmmpui/mediaplayer.h>
#include "converter.h"
#include "converterdialog.h"
#include "converterhelper.h"

ConverterHelper::ConverterHelper(QObject *parent) : QObject(parent)
{
    m_action = new QAction(tr("Convert"), this);
    m_action->setShortcut(tr("Meta+C"));
    UiHelper::instance()->addAction(m_action, UiHelper::PLAYLIST_MENU);
    connect (m_action, SIGNAL(triggered ()), SLOT(openConverter()));
    m_converter = new Converter(this);
    m_progress = new QProgressDialog();
    m_progress->setRange(0,100);
    m_progress->setWindowTitle(tr("Converting..."));
    m_progress->setCancelButtonText(tr("Cancel"));
    connect(m_converter,SIGNAL(progress(int)),m_progress,SLOT(setValue(int)));
    connect(m_converter, SIGNAL(finished()), m_progress, SLOT(reset()));
    connect(m_converter, SIGNAL(desriptionChanged(QString)), m_progress, SLOT(setLabelText(QString)));
    connect(m_progress, SIGNAL(canceled()), m_converter, SLOT(stop()));
}

ConverterHelper::~ConverterHelper()
{
    m_progress->deleteLater();
}

void ConverterHelper::openConverter()
{
    PlayListManager *pl_manager = MediaPlayer::instance()->playListManager();
    QList <PlayListItem *> items = pl_manager->selectedPlayList()->selectedItems();
    if (items.isEmpty())
        return;

    ConverterDialog *d = new ConverterDialog(items, qApp->activeWindow ());
    if(d->exec() == QDialog::Accepted)
    {
        QStringList urls = d->selectedUrls();
        QVariantMap preset = d->preset();
        if(preset.isEmpty())
        {
            d->deleteLater();
            return;
        }
        m_converter->add(urls, preset);
        if(!m_converter->isRunning())
            m_converter->start();

    }
    d->deleteLater();
}
