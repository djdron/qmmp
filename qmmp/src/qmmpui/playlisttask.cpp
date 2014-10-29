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

#include <QFileInfo>
#include <QDateTime>
#include "playlisttrack.h"
#include "playlisttask_p.h"

struct TrackField
{
    PlayListTrack *track;
    QString value;
};

////===============THE BEGINNING OF SORT IMPLEMENTATION =======================////

// First we'll implement bundle of static compare procedures
// to sort items in different ways

//by string
static bool _stringLessComparator(TrackField* s1, TrackField* s2)
{
    return QString::localeAwareCompare (s1->value, s2->value) < 0;
}

static bool _stringGreaterComparator(TrackField* s1, TrackField* s2)
{
    return QString::localeAwareCompare (s1->value, s2->value) > 0;
}
//by number
static bool _numberLessComparator(TrackField* s1, TrackField* s2)
{
    return s1->value.toInt() < s2->value.toInt();
}

static bool _numberGreaterComparator(TrackField* s1, TrackField* s2)
{
    return s1->value.toInt() > s2->value.toInt();
}
//by file creation date
static bool _fileCreationDateLessComparator(TrackField* s1, TrackField* s2)
{
    return QFileInfo(s1->value).created() < QFileInfo(s2->value).created();
}

static bool _fileCreationDateGreaterComparator(TrackField* s1, TrackField* s2)
{
    return QFileInfo(s1->value).created() > QFileInfo(s2->value).created();
}
//by file modification date
static bool _fileModificationDateLessComparator(TrackField* s1, TrackField* s2)
{
    return QFileInfo(s1->value).lastModified() < QFileInfo(s2->value).lastModified();
}

static bool _fileModificationDateGreaterComparator(TrackField* s1, TrackField* s2)
{
    return QFileInfo(s1->value).lastModified() > QFileInfo(s2->value).lastModified();
}
//by file name
static bool _filenameLessComparator(TrackField* s1, TrackField* s2)
{
    QFileInfo i_s1(s1->value);
    QFileInfo i_s2(s2->value);
    return QString::localeAwareCompare (i_s1.baseName(), i_s2.baseName()) < 0;
}

static bool _filenameGreaterComparator(TrackField* s1, TrackField* s2)
{
    QFileInfo i_s1(s1->value);
    QFileInfo i_s2(s2->value);
    return QString::localeAwareCompare (i_s1.baseName(), i_s2.baseName()) > 0;
}
////=============== THE END OF SORT IMPLEMENTATION =======================////

PlayListTask::PlayListTask(QObject *parent) : QThread(parent)
{
    m_reverted = true;
}

PlayListTask::~PlayListTask()
{
    qDebug("%s", Q_FUNC_INFO);
}

void PlayListTask::sort(QList<PlayListTrack *> tracks, PlayListModel::SortMode mode)
{
    if(isRunning())
        return;
    m_fields.clear();
    m_reverted = !m_reverted;
    m_sort_mode = mode;
    Qmmp::MetaData key = Qmmp::TITLE;

    switch (mode)
    {
    case PlayListModel::TITLE:
        key = Qmmp::TITLE;
        break;
    case PlayListModel::DISCNUMBER:
        key = Qmmp::DISCNUMBER;
        break;
    case PlayListModel::ALBUM:
        key = Qmmp::ALBUM;
        break;
    case PlayListModel::ARTIST:
        key = Qmmp::ARTIST;
        break;
    case PlayListModel::ALBUMARTIST:
        key = Qmmp::ALBUMARTIST;
        break;
    case PlayListModel::FILENAME:
    case PlayListModel::PATH_AND_FILENAME:
        key = Qmmp::URL;
        break;
    case PlayListModel::DATE:
        key = Qmmp::YEAR;
        break;
    case PlayListModel::TRACK:
        key = Qmmp::TRACK;
        break;
    case PlayListModel::FILE_CREATION_DATE:
    case PlayListModel::FILE_MODIFICATION_DATE:
        key = Qmmp::URL;
        break;
    default:
        ;
    }

    if(mode == PlayListModel::GROUP)
    {
        foreach (PlayListTrack *t, tracks)
        {
            TrackField *f = new TrackField;
            f->track = t;
            f->value = t->groupName();
            m_fields.append(f);
        }
    }
    else
    {
        foreach (PlayListTrack *t, tracks)
        {
            TrackField *f = new TrackField;
            f->track = t;
            f->value = t->value(key);
            m_fields.append(f);
        }
    }
    start();
}

void PlayListTask::run()
{
    qDebug("started");
    bool(*compareLessFunc)(TrackField*, TrackField*) = 0;
    bool(*compareGreaterFunc)(TrackField*, TrackField*) = 0;

    QList<TrackField*>::iterator begin = m_fields.begin();
    QList<TrackField*>::iterator end = m_fields.end();

    if(m_sort_mode == PlayListModel::FILE_CREATION_DATE)
    {
        compareLessFunc = _fileCreationDateLessComparator;
        compareGreaterFunc = _fileCreationDateGreaterComparator;
    }
    else if(m_sort_mode == PlayListModel::FILE_MODIFICATION_DATE)
    {
        compareLessFunc = _fileModificationDateLessComparator;
        compareGreaterFunc = _fileModificationDateGreaterComparator;
    }
    else if(m_sort_mode == PlayListModel::TRACK || m_sort_mode == PlayListModel::DATE)
    {
        compareLessFunc = _numberLessComparator;
        compareGreaterFunc = _numberGreaterComparator;
    }
    else if(m_sort_mode == PlayListModel::FILENAME)
    {
        compareLessFunc = _filenameLessComparator;
        compareGreaterFunc = _filenameGreaterComparator;
    }
    else
    {
        compareLessFunc = _stringLessComparator;
        compareGreaterFunc = _stringGreaterComparator;
    }

    if(m_reverted)
        qStableSort(begin,end,compareGreaterFunc);
    else
        qStableSort(begin,end,compareLessFunc);
    qDebug("finished");
}

QList<PlayListTrack *> PlayListTask::takeResults()
{
    QList<PlayListTrack *> tracks;
    foreach (TrackField *f, m_fields)
    {
        if(f->track->flag() == PlayListTrack::SCHEDULED_FOR_DELETION)
            delete f->track;
        else
            tracks.append(f->track);
    }
    qDeleteAll(m_fields);
    m_fields.clear();
    return tracks;
}
