/***************************************************************************
 *   Copyright (C) 2009-2010 by Ilya Kotov                                 *
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
#include <QMessageBox>
#include <qmmpui/generalhandler.h>
#include <qmmpui/playlistmodel.h>
#include <qmmpui/playlistitem.h>
#include <qmmpui/mediaplayer.h>
#include <qmmpui/generalhandler.h>
#include <qmmpui/metadataformatter.h>
#include <qmmp/metadatamanager.h>
#include "coverwidget.h"
#include "covermanager.h"

CoverManager::CoverManager(QObject *parent) : General(parent)
{
    m_action = new QAction(tr("Show Cover"), this);
    m_action->setShortcut(tr("Ctrl+M"));
    GeneralHandler::instance()->addAction(m_action, GeneralHandler::PLAYLIST_MENU);
    connect (m_action, SIGNAL(triggered ()), SLOT(showWindow()));
}

void CoverManager::showWindow()
{
    QList <PlayListItem *> items = MediaPlayer::instance()->playListManager()->selectedPlayList()->getSelectedItems();
    if (!items.isEmpty())
    {
        CoverWidget *w = new CoverWidget(qApp->activeWindow ());
        QPixmap pix = MetaDataManager::instance()->getCover(items.at(0)->url());
        if(pix.isNull())
            pix = QPixmap(":/cm_no_cover.png");
        w->setPixmap(pix);
        MetaDataFormatter formatter("%p%if(%p&%t, - ,)%if(%t,%t,%f)");
        w->setWindowTitle(formatter.parse(items.at(0)->metaData()));
        w->show();
    }
}
