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
#ifndef PLAYEROBJECT_H
#define PLAYEROBJECT_H

#include <QtDBus>
#include <QString>
#include <QVariant>

class SoundCore;

/**
	@author Ilya Kotov <forkotov02@hotmail.ru>
*/

struct PlayerStatus
{
    int state;  // 0 = Playing, 1 = Paused, 2 = Stopped.
    int random; // 0 = Playing linearly, 1 = Playing randomly.
    int repeat; // 0 = Go to the next element once the current has finished playing, 1 = Repeat the current element
    int repeatPlayList; // 0 = Stop playing once the last element has been played, 1 = Never give up playing
};


class PlayerObject : public QDBusAbstractAdaptor
{
Q_OBJECT
Q_CLASSINFO("D-Bus Interface", "org.freedesktop.MediaPlayer")

public:
    PlayerObject(QObject *parent = 0);

    ~PlayerObject();

public slots:
    void Next();
    void Prev();
    void Pause();
    void Stop();
    void Play();
    //void Repeat();
    PlayerStatus GetStatus();
    QVariantMap GetMetadata();
    //GetCaps
    void VolumeSet(int in0);
    int VolumeGet();
    void PositionSet(int in0);
    qint64 PositionGet();

signals:
    //TrackChange 
    //StatusChange 
    //CapsChange 

private:
    SoundCore *m_core;

};

#endif
