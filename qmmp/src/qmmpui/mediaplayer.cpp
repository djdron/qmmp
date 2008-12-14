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

#include <QApplication>
#include <QString>
#include <qmmp/soundcore.h>

#include "playlistmodel.h"
#include "playlistitem.h"

#include "mediaplayer.h"

MediaPlayer *MediaPlayer::m_instance = 0;

MediaPlayer::MediaPlayer(QObject *parent)
        : QObject(parent)
{
    m_instance = this;
    m_model = 0;
    m_core = 0;
}


MediaPlayer::~MediaPlayer()
{
}

MediaPlayer* MediaPlayer::instance()
{
    return m_instance;
}

void MediaPlayer::initialize(SoundCore *core, PlayListModel *model)
{
    Q_CHECK_PTR(core);
    Q_CHECK_PTR(model);
    m_core = core;
    m_model = model;
    connect(m_core, SIGNAL(finished()), SLOT(next()));
}

void MediaPlayer::play()
{
    m_model->doCurrentVisibleRequest();
    if (m_core->state() == Qmmp::Paused)
    {
        m_core->pause();
        return;
    }

    if (m_model->count() == 0)
        return;

    QString s = m_model->currentItem()->url();
    if (s.isEmpty())
        return;
    if (!m_core->play(s))
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
            qApp->processEvents();
            if (!m_model->isEmptyQueue())
            {
                m_model->setCurrentToQueued();
            }
            else if (!m_model->next())
            {
                stop();
                //display->hideTimeDisplay();
                return;
            }
            play();
            break;
        }
        }
    }
}

void MediaPlayer::stop()
{
    m_core->stop();
}

void MediaPlayer::next()
{
    if (!m_model->isEmptyQueue())
    {
        m_model->setCurrentToQueued();
    }
    else if (!m_model->next())
    {
        stop();
        //display->hideTimeDisplay();
        return;
    }
    //m_playlist->update();
    if (m_core->state() != Qmmp::Stopped)
    {
        if (m_core->state() == Qmmp::Paused)
            stop();
        play();
    }
    /*else
        display->hideTimeDisplay();*/
}

void MediaPlayer::previous()
{
    if (!m_model->previous())
    {
        stop();
        //display->hideTimeDisplay();
        return;
    }

    //m_playlist->update();
    if (m_core->state() != Qmmp::Stopped)
    {
        if (m_core->state() == Qmmp::Paused)
            stop();
        play();
    }
    /*else
        display->hideTimeDisplay();*/
}

