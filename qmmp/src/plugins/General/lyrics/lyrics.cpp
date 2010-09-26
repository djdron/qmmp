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
#include <qmmpui/playlistmanager.h>
#include <qmmpui/playlistitem.h>
#include <qmmpui/mediaplayer.h>
#include "lyricswindow.h"
#include "lyrics.h"

Lyrics::Lyrics(QObject *parent)
        : General(parent)
{
    m_action = new QAction(tr("View Lyrics"), this);
    m_action->setShortcut(tr("Ctrl+L"));
    GeneralHandler::instance()->addAction(m_action, GeneralHandler::PLAYLIST_MENU);
    connect (m_action, SIGNAL(triggered ()), SLOT(showLyrics()));
}

Lyrics::~Lyrics()
{}

void Lyrics::showLyrics()
{
    PlayListManager *pl_manager = MediaPlayer::instance()->playListManager();
    QList <PlayListItem *> items = pl_manager->selectedPlayList()->getSelectedItems();
    if (!items.isEmpty())
    {
        if (items.at(0)->value(Qmmp::ARTIST).isEmpty() || items.at(0)->value(Qmmp::TITLE).isEmpty())
                return;
            LyricsWindow *w = new LyricsWindow(items.at(0)->value(Qmmp::ARTIST),
                                               items.at(0)->value(Qmmp::TITLE), qApp->activeWindow ());
            w->show();
    }
}
