/***************************************************************************
 *   Copyright (C) 2008-2010 by Ilya Kotov                                 *
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

#include <QApplication>
#include <QString>
#include <QTranslator>
#include <QLocale>
#include "playlistitem.h"
#include "mediaplayer.h"

#define MAX_SKIPS 5

MediaPlayer *MediaPlayer::m_instance = 0;

MediaPlayer::MediaPlayer(QObject *parent)
        : QObject(parent)
{
    m_instance = this;
    m_pl_manager = 0;
    m_core = 0;
    m_skips = 0;
    m_repeat = false;
    m_noPlaylistAdvance = false;
    QTranslator *translator = new QTranslator(parent);
    QString locale = Qmmp::systemLanguageID();
    translator->load(QString(":/libqmmpui_") + locale);
    qApp->installTranslator(translator);
}

MediaPlayer::~MediaPlayer()
{}

MediaPlayer* MediaPlayer::instance()
{
    return m_instance;
}

void MediaPlayer::initialize(SoundCore *core, PlayListManager *pl_manager)
{
    Q_CHECK_PTR(core);
    Q_CHECK_PTR(pl_manager);
    m_core = core;
    m_pl_manager = pl_manager;
    m_repeat = false;
    m_noPlaylistAdvance = false;
    connect(m_core, SIGNAL(nextTrackRequest()), SLOT(updateNextUrl()));
    connect(m_core, SIGNAL(finished()), SLOT(playNext()));
}

PlayListManager *MediaPlayer::playListManager()
{
    return m_pl_manager;
}

bool MediaPlayer::isRepeatable() const
{
    return m_repeat;
}

bool MediaPlayer::isNoPlaylistAdvance() const
{
    return m_noPlaylistAdvance;
}

void MediaPlayer::play(qint64 offset)
{
    m_pl_manager->currentPlayList()->doCurrentVisibleRequest();
    if (m_core->state() == Qmmp::Paused)
    {
        m_core->pause();
        return;
    }

    if (m_pl_manager->currentPlayList()->count() == 0)
        return;

    QString s = m_pl_manager->currentPlayList()->currentItem()->url();
    if (s.isEmpty())
    {
        m_nextUrl.clear();
        return;
    }
    if(m_nextUrl == s)
    {
        m_nextUrl.clear();
        return;
    }

    if (!m_core->play(s, false, offset))
    {
        //find out the reason why playback failed
        switch ((int) m_core->state())
        {
        case Qmmp::FatalError:
        {
            stop();
            return; //unrecovable error in output, so abort playing
        }
        case Qmmp::NormalError:
        {
            //error in decoder, so we should try to play next song
            m_skips++;
            if (m_skips > MAX_SKIPS)
            {
                stop();
                qWarning("MediaPlayer: skip limit exceeded");
                break;
            }
            qApp->processEvents();
            if (!m_pl_manager->currentPlayList()->isEmptyQueue())
            {
                m_pl_manager->currentPlayList()->setCurrentToQueued();
            }
            else if (!m_pl_manager->currentPlayList()->next())
            {
                stop();
                return;
            }
            play();
            break;
        }
        }
    }
    else
        m_skips = 0;
}

void MediaPlayer::stop()
{
    m_core->stop();
    m_nextUrl.clear();
}

void MediaPlayer::next()
{
    if (!m_pl_manager->currentPlayList()->next())
    {
        stop();
        return;
    }
    if (m_core->state() != Qmmp::Stopped)
    {
        stop();
        play();
    }
}

void MediaPlayer::previous()
{
    if (!m_pl_manager->currentPlayList()->previous())
    {
        stop();
        return;
    }

    if (m_core->state() != Qmmp::Stopped)
    {
        if (m_core->state() == Qmmp::Paused)
            stop();
        play();
    }
}

void MediaPlayer::setRepeatable(bool r)
{
    if (r != m_repeat)
    {
        if(r)
        {
            disconnect(m_core, SIGNAL(finished()), this, SLOT(playNext()));
            connect(m_core, SIGNAL(finished()), SLOT(play()));
        }
        else
        {
            disconnect(m_core, SIGNAL(finished()), this, SLOT(play()));
            connect(m_core, SIGNAL(finished()), SLOT(playNext()));
        }
        m_repeat = r;
        emit repeatableChanged(r);
    }
}

void MediaPlayer::playNext()
{
    if(m_noPlaylistAdvance)
    {
        stop();
        return;
    }
    if (!m_pl_manager->currentPlayList()->next())
    {
        stop();
        return;
    }
    play();
}

void MediaPlayer::setNoPlaylistAdvance(bool enabled)
{
    if (enabled != m_noPlaylistAdvance)
    {
        m_noPlaylistAdvance = enabled;
        emit noPlaylistAdvanceChanged(enabled);
    }
}

void MediaPlayer::updateNextUrl()
{
    m_nextUrl.clear();
    PlayListItem *item = 0;
    if(isRepeatable())
        item = m_pl_manager->currentPlayList()->currentItem();
    else if(!m_noPlaylistAdvance)
        item = m_pl_manager->currentPlayList()->nextItem();

    if(item)
    {
        bool ok = m_core->play(item->url(), true);
        if(ok)
        {
            m_nextUrl = m_pl_manager->currentPlayList()->nextItem()->url();
            qDebug("MediaPlayer: next track state: received");
        }
        else
            qDebug("MediaPlayer: next track state: error");
    }
    else
        qDebug("MediaPlayer: next track state: unknown");

}
