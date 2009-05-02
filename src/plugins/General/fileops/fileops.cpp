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

#include <QAction>
#include <QApplication>

#include <qmmp/soundcore.h>
#include <qmmpui/generalhandler.h>
#include <qmmpui/playlistmodel.h>
#include <qmmpui/playlistitem.h>
#include <qmmpui/mediaplayer.h>
#include "fileops.h"

FileOps::FileOps(QObject *parent)
        : General(parent)
{
    m_copyAction = new QAction(tr("Copy"), this);
    m_moveAction = new QAction(tr("Move"), this);
    m_removeAction = new QAction(tr("Remove"), this);
    //separators
    QAction *separator1 = new QAction(this);
    separator1->setSeparator (TRUE);
    QAction *separator2 = new QAction(this);
    separator2->setSeparator (TRUE);

    GeneralHandler::instance()->addAction(separator1, GeneralHandler::PLAYLIST_MENU);
    GeneralHandler::instance()->addAction(m_copyAction, GeneralHandler::PLAYLIST_MENU);
    GeneralHandler::instance()->addAction(m_moveAction, GeneralHandler::PLAYLIST_MENU);
    GeneralHandler::instance()->addAction(m_removeAction, GeneralHandler::PLAYLIST_MENU);
    GeneralHandler::instance()->addAction(separator2, GeneralHandler::PLAYLIST_MENU);
}

FileOps::~FileOps()
{}

