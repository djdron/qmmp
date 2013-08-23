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

#include <QFileInfo>
#include <QDateTime>
#include <QtAlgorithms>
#include "playlistmodel.h"
#include "playlistcontainer_p.h"

void PlayListContainer::addTracks(QList<PlayListTrack *> tracks)
{
    foreach(PlayListTrack *t, tracks)
        addTrack(t);
}

////===============THE BEGINNING OF SORT IMPLEMENTATION =======================////

// First we'll implement bundle of static compare procedures
// to sort items in different ways

//by title
static bool _titleLessComparator(PlayListTrack* s1,PlayListTrack* s2)
{
    return QString::localeAwareCompare (s1->value(Qmmp::TITLE), s2->value(Qmmp::TITLE)) < 0;
}

static bool _titleGreaterComparator(PlayListTrack* s1,PlayListTrack* s2)
{
    return QString::localeAwareCompare (s1->value(Qmmp::TITLE), s2->value(Qmmp::TITLE)) > 0;
}
//by album+disc
static bool _discnumberLessComparator(PlayListTrack* s1,PlayListTrack* s2)
{
    return QString::localeAwareCompare (s1->value(Qmmp::DISCNUMBER), s2->value(Qmmp::DISCNUMBER)) < 0;
}

static bool _discnumberGreaterComparator(PlayListTrack* s1,PlayListTrack* s2)
{
    return QString::localeAwareCompare (s1->value(Qmmp::DISCNUMBER), s2->value(Qmmp::DISCNUMBER)) > 0;
}
//by album
static bool _albumLessComparator(PlayListTrack* s1,PlayListTrack* s2)
{
    return QString::localeAwareCompare (s1->value(Qmmp::ALBUM), s2->value(Qmmp::ALBUM)) < 0;
}

static bool _albumGreaterComparator(PlayListTrack* s1,PlayListTrack* s2)
{
    return QString::localeAwareCompare (s1->value(Qmmp::ALBUM), s2->value(Qmmp::ALBUM)) > 0;
}
//by artist
static bool _artistLessComparator(PlayListTrack* s1,PlayListTrack* s2)
{
    return QString::localeAwareCompare (s1->value(Qmmp::ARTIST), s2->value(Qmmp::ARTIST)) < 0;
}

static bool _artistGreaterComparator(PlayListTrack* s1,PlayListTrack* s2)
{
    return QString::localeAwareCompare (s1->value(Qmmp::ARTIST), s2->value(Qmmp::ARTIST)) > 0;
}
//by path
static bool _pathAndFilenameLessComparator(PlayListTrack* s1,PlayListTrack* s2)
{
    return QString::localeAwareCompare (s1->url(), s2->url()) < 0;
}

static bool _pathAndFilenameGreaterComparator(PlayListTrack* s1,PlayListTrack* s2)
{
    return QString::localeAwareCompare (s1->url(), s2->url()) > 0;
}
//by file name
static bool _filenameLessComparator(PlayListTrack* s1,PlayListTrack* s2)
{
    QFileInfo i_s1(s1->url());
    QFileInfo i_s2(s2->url());
    return QString::localeAwareCompare (i_s1.baseName(), i_s2.baseName()) < 0;
}

static bool _filenameGreaterComparator(PlayListTrack* s1,PlayListTrack* s2)
{
    QFileInfo i_s1(s1->url());
    QFileInfo i_s2(s2->url());
    return QString::localeAwareCompare (i_s1.baseName(), i_s2.baseName()) > 0;
}
//by date
static bool _dateLessComparator(PlayListTrack* s1,PlayListTrack* s2)
{
    return s1->value(Qmmp::YEAR).toInt() < s2->value(Qmmp::YEAR).toInt();
}

static bool _dateGreaterComparator(PlayListTrack* s1,PlayListTrack* s2)
{
    return s1->value(Qmmp::YEAR).toInt() > s2->value(Qmmp::YEAR).toInt();
}
//by track
static bool _trackLessComparator(PlayListTrack* s1,PlayListTrack* s2)
{
    return s1->value(Qmmp::TRACK).toInt() < s2->value(Qmmp::TRACK).toInt();
}

static bool _trackGreaterComparator(PlayListTrack* s1,PlayListTrack* s2)
{
    return s1->value(Qmmp::TRACK).toInt() > s2->value(Qmmp::TRACK).toInt();
}
//by file creation date
static bool _fileCreationDateLessComparator(PlayListTrack* s1,PlayListTrack* s2)
{
    return QFileInfo(s1->value(Qmmp::URL)).created() < QFileInfo(s2->value(Qmmp::URL)).created();
}

static bool _fileCreationDateGreaterComparator(PlayListTrack* s1,PlayListTrack* s2)
{
    return QFileInfo(s1->value(Qmmp::URL)).created() > QFileInfo(s2->value(Qmmp::URL)).created();
}
//by file modification date
static bool _fileModificationDateLessComparator(PlayListTrack* s1,PlayListTrack* s2)
{
    return QFileInfo(s1->value(Qmmp::URL)).lastModified() < QFileInfo(s2->value(Qmmp::URL)).lastModified();
}

static bool _fileModificationDateGreaterComparator(PlayListTrack* s1,PlayListTrack* s2)
{
    return QFileInfo(s1->value(Qmmp::URL)).lastModified() > QFileInfo(s2->value(Qmmp::URL)).lastModified();
}

//by group
static bool _groupLessComparator(PlayListTrack* s1,PlayListTrack* s2)
{
    return QString::localeAwareCompare (s1->groupName(), s2->groupName()) < 0;
}

static bool _groupGreaterComparator(PlayListTrack* s1,PlayListTrack* s2)
{
    return QString::localeAwareCompare (s1->groupName(), s2->groupName()) > 0;
}

void PlayListContainer::doSort(int sort_mode, QList<PlayListTrack*>& list_to_sort, bool reverted)
{
    QList<PlayListTrack*>::iterator begin;
    QList<PlayListTrack*>::iterator end;

    begin = list_to_sort.begin();
    end = list_to_sort.end();

    bool(*compareLessFunc)(PlayListTrack*,PlayListTrack*) = 0;
    bool(*compareGreaterFunc)(PlayListTrack*,PlayListTrack*) = 0;

    switch (sort_mode)
    {
    case PlayListModel::TITLE:
        compareLessFunc = _titleLessComparator;
        compareGreaterFunc = _titleGreaterComparator;
        break;
    case PlayListModel::DISCNUMBER:
        compareLessFunc = _discnumberLessComparator;
        compareGreaterFunc = _discnumberGreaterComparator;
        break;
    case PlayListModel::ALBUM:
        compareLessFunc = _albumLessComparator;
        compareGreaterFunc = _albumGreaterComparator;
        break;
    case PlayListModel::ARTIST:
        compareLessFunc = _artistLessComparator;
        compareGreaterFunc = _artistGreaterComparator;
        break;
    case PlayListModel::FILENAME:
        compareLessFunc = _filenameLessComparator;
        compareGreaterFunc = _filenameGreaterComparator;
        break;
    case PlayListModel::PATH_AND_FILENAME:
        compareLessFunc = _pathAndFilenameLessComparator;
        compareGreaterFunc = _pathAndFilenameGreaterComparator;
        break;
    case PlayListModel::DATE:
        compareLessFunc = _dateLessComparator;
        compareGreaterFunc = _dateGreaterComparator;
        break;
    case PlayListModel::TRACK:
        compareLessFunc = _trackLessComparator;
        compareGreaterFunc = _trackGreaterComparator;
        break;
    case PlayListModel::FILE_CREATION_DATE:
        compareLessFunc = _fileCreationDateLessComparator;
        compareGreaterFunc = _fileCreationDateGreaterComparator;
        break;
    case PlayListModel::FILE_MODIFICATION_DATE:
        compareLessFunc = _fileModificationDateLessComparator;
        compareGreaterFunc = _fileModificationDateGreaterComparator;
        break;
    case PlayListModel::GROUP:
        compareLessFunc = _groupLessComparator;
        compareGreaterFunc = _groupGreaterComparator;
        break;
    default:
        compareLessFunc = _titleLessComparator;
        compareGreaterFunc = _titleGreaterComparator;
    }

    if(reverted)
        qStableSort(begin,end,compareGreaterFunc);
    else
        qStableSort(begin,end,compareLessFunc);
}

////=============== THE END OF SORT IMPLEMENTATION =======================////
