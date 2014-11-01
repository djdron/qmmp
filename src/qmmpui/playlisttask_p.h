/***************************************************************************
 *   Copyright (C) 2014 by Ilya Kotov                                      *
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

#ifndef PLAYLISTTASK_P_H
#define PLAYLISTTASK_P_H

#include <QThread>
#include <QObject>
#include <QList>
#include "playlistmodel.h"

class PlayListTrack;
struct TrackField;


class PlayListTask : public QThread
{
    Q_OBJECT
public:
    explicit PlayListTask(QObject *parent);

    ~PlayListTask();

    void sort(QList<PlayListTrack *> tracks, int mode);
    void sortSelection(QList<PlayListTrack *> tracks, int mode);
    //void removeInvalidTracks(QList<PlayListTrack *> tracks);
    //void removeDuplicates();

    void run();

    QList<PlayListTrack *> takeResults();

    enum TaskType
    {
        EMPTY = -1,
        SORT = 0,
        SORT_SELECTION
    };

private:
    QList <TrackField *> m_fields;
    QList <PlayListTrack *> m_tracks;
    QList<int> m_indexes;
    int m_sort_mode;
    TaskType m_task;
    bool m_reverted;

};

#endif // PLAYLISTTASK_P_H
