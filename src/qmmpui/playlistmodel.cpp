/***************************************************************************
 *   Copyright(C) 2006-2013 by Ilya Kotov                                  *
 *   forkotov02@hotmail.ru                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *  (at your option) any later version.                                    *
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
#include <QWidget>
#include <QFile>
#include <QDir>
#include <QtAlgorithms>
#include <QFileInfo>
#include <QTextStream>
#include <QPluginLoader>
#include <QApplication>
#include <QTimer>
#include <QBuffer>
#include <QMetaType>
#include <QDateTime>
#include <time.h>
#include <qmmp/metadatamanager.h>
#include "playlistparser.h"
#include "playlistformat.h"
#include "fileloader_p.h"
#include "playstate_p.h"
#include "detailsdialog.h"
#include "tagupdater_p.h"
#include "playlistmodel.h"

#define INVALID_ROW -1

PlayListModel::PlayListModel(const QString &name, QObject *parent)
        : QObject(parent) , m_selection()
{
    qsrand(time(0));
    m_name = name;
    m_shuffle = 0;
    m_total_length = 0;
    m_current = 0;
    m_is_repeatable_list = false;
    m_stop_track = 0;
    m_play_state = new NormalPlayState(this);
    m_loader = new FileLoader(this);
    connect(m_loader, SIGNAL(newPlayListTrack(PlayListTrack*)),
            SLOT(add(PlayListTrack*)), Qt::QueuedConnection);
    connect(m_loader, SIGNAL(finished()), SLOT(preparePlayState()));
    connect(m_loader, SIGNAL(finished()), SIGNAL(loaderFinished()));
}

PlayListModel::~PlayListModel()
{
    blockSignals(true);
    clear();
    delete m_play_state;
    m_loader->finish();
}

QString PlayListModel::name() const
{
    return m_name;
}

void PlayListModel::setName(const QString &name)
{
    if(m_name != name)
    {
        m_name = name;
        emit nameChanged(name);
    }
}

void PlayListModel::add(PlayListTrack *track)
{
    if (m_container.isEmpty())
    {
        m_current_track = track;
        m_current = 1; //0
    }
    m_container.addTrack(track);
    m_total_length += track->length();

    emit itemAdded(track);
    emit listChanged();
    emit countChanged();
}

void PlayListModel::add(QList<PlayListTrack *> tracks)
{
    if(tracks.isEmpty())
        return;
    if (m_container.isEmpty())
    {
        m_current_track = tracks.at(0);
        m_current = 1; //0
    }

    foreach(PlayListTrack *track, tracks)
    {
        m_container.addTrack(track);
        m_total_length += track->length();
        emit itemAdded(track);
    }
    emit listChanged();
    emit countChanged();
}

void PlayListModel::add(const QString &path)
{
    QFileInfo f_info(path);
    if (f_info.isDir())
        m_loader->loadDirectory(path);
    else
    {
        m_loader->loadFile(path);
        loadPlaylist(path);
    }

}

void PlayListModel::add(const QStringList &paths)
{
    foreach(QString str, paths)
    {
        add(str);
    }
}

int PlayListModel::count() const
{
    return m_container.count();
}

int PlayListModel::trackCount() const
{
    return m_container.trackCount();
}

PlayListTrack* PlayListModel::currentTrack() const
{
    return m_container.isEmpty() ? 0 : m_current_track;
}

PlayListTrack *PlayListModel::nextTrack() const
{
    if(m_container.isEmpty() || !m_play_state)
        return 0;
    if(m_stop_track && m_stop_track == currentTrack())
        return 0;
    if(!isEmptyQueue())
        return m_queued_songs.at(0);
    int index = m_play_state->nextIndex();
    if(index < 0 || (index + 1 > m_container.count()))
        return 0;
    return m_container.track(index);
}

int PlayListModel::indexOf(PlayListItem* item) const
{
    return m_container.indexOf(item);
}

PlayListItem* PlayListModel::item(int index) const
{
    return m_container.item(index);
}

PlayListTrack* PlayListModel::track(int index) const
{
    return m_container.track(index);
}

int PlayListModel::currentIndex() const
{
    return m_current;
}

bool PlayListModel::setCurrent(int index)
{
    if (index > count()-1 || index < 0)
        return false;
    PlayListItem *item = m_container.item(index);
    if(item->isGroup())
        return false;
    m_current = index;
    m_current_track = dynamic_cast<PlayListTrack*> (item);
    emit currentChanged();
    emit listChanged();
    return true;
}

bool PlayListModel::setCurrent(PlayListTrack *track)
{
    if(!m_container.contains(track))
        return false;
    return setCurrent(m_container.indexOf(track));
}

bool PlayListModel::isTrack(int index) const
{
    if (index > count()-1 || index < 0)
        return false;
    return !m_container.item(index)->isGroup();
}

bool PlayListModel::next()
{
    if(m_stop_track == currentTrack())
    {
        m_stop_track = 0;
        emit listChanged();
        return false;
    }
    if (!isEmptyQueue())
    {
        setCurrentToQueued();
        return true;
    }

    if(m_loader->isRunning())
        m_play_state->prepare();
    return m_play_state->next();
}

bool PlayListModel::previous()
{
    if (m_loader->isRunning())
        m_play_state->prepare();
    return m_play_state->previous();
}

void PlayListModel::clear()
{
    m_loader->finish();
    m_current = 0;
    m_stop_track = 0;
    m_container.clear();
    m_queued_songs.clear();
    m_total_length = 0;
    m_play_state->resetState();
    emit listChanged();
    emit countChanged();
}

void PlayListModel::clearSelection()
{
    m_container.clearSelection();
    emit listChanged();
}

QList<PlayListItem *> PlayListModel::mid(int pos, int count) const
{
    return m_container.mid(pos, count);
}

bool PlayListModel::isSelected(int index) const
{
    return m_container.isSelected(index);
}

bool PlayListModel::contains(const QString &url)
{
    /*foreach (PlayListItem *item, m_items)
    {
        if(item->url() == url)
            return true;
    }*/
    return false;
}

int PlayListModel::numberOfTrack(int index) const
{
    return m_container.numberOfTrack(index);
}

void PlayListModel::setSelected(int index, bool selected)
{
    m_container.setSelected(index, selected);
    emit listChanged();
}

void PlayListModel::removeSelected()
{
    removeSelection(false);
}

void PlayListModel::removeUnselected()
{
    removeSelection(true);
}

void PlayListModel::removeTrack (int i)
{
    if ((i < count()) && (i >= 0))
    {
        PlayListTrack* track = m_container.track(i);
        if(!track)
            return;
        m_queued_songs.removeAll(track);
        if(m_stop_track == track)
            m_stop_track = 0;
        m_total_length -= track->length();
        m_total_length = qMax(0, m_total_length);

        m_container.removeTrack(i);

        if(m_container.isEmpty())
        {
            m_current = 0;
            m_current_track = 0;
        }
        else
        {
            if (m_current >= i && m_current > 0)
                m_current--;

            if(m_current > 0 && m_container.item(m_current)->isGroup())
                m_current--;

            m_current_track = m_container.track(m_current);
        }

        m_play_state->prepare();
        emit listChanged();
        emit countChanged();
    }
}

void PlayListModel::removeTrack (PlayListItem *track)
{
    if(m_container.contains(track))
        removeTrack (m_container.indexOf(track));
}

void PlayListModel::removeSelection(bool inverted)
{
    int i = 0;

    int select_after_delete = -1;

    while (!m_container.isEmpty() && i < m_container.count())
    {
        PlayListItem *item = m_container.item(i);
        if (!item->isGroup() && item->isSelected() ^ inverted)
        {
            PlayListTrack *track = dynamic_cast<PlayListTrack *> (item);
            m_container.removeTrack(track);
            m_queued_songs.removeAll(track);
            if(track == m_stop_track)
                m_stop_track = 0;
            m_total_length -= track->length();
            if (m_total_length < 0)
                m_total_length = 0;

            if (track->flag() == PlayListTrack::FREE)
            {
                delete track;
                track = NULL;
            }
            else if (track->flag() == PlayListTrack::EDITING)
                track->setFlag(PlayListTrack::SCHEDULED_FOR_DELETION);

            select_after_delete = i;

            if (m_current >= i && m_current > 0)
            {
                if(m_current != 1 || !m_container.item(0)->isGroup())
                {
                    if(!m_container.item(m_current-1)->isGroup())
                        m_current--;
                    else
                        m_current-=2;
                }
            }
        }
        else
            i++;
    }

    if (!m_container.isEmpty())
    {
        m_current_track = m_container.track(m_current);
    }
    else
        m_current_track = 0;


    if (select_after_delete >= m_container.count())
        select_after_delete = m_container.count() - 1;

    if(select_after_delete != -1)
        m_container.setSelected(select_after_delete, true);

    m_play_state->prepare();

    emit listChanged();
    emit countChanged();
}

void PlayListModel::invertSelection()
{
    for (int i = 0; i < m_container.count(); ++i)
        m_container.setSelected(i, !m_container.isSelected(i));
    emit listChanged();
}

void PlayListModel::selectAll()
{
    for (int i = 0; i < m_container.count(); ++i)
        m_container.setSelected(i, true);
    emit listChanged();
}

void PlayListModel::showDetails(QWidget *parent)
{
    for (int i = 0; i < m_container.count(); ++i)
    {
        if (m_container.isSelected(i) && m_container.track(i))
        {
            QDialog *d = new DetailsDialog(m_container.track(i), parent);
            TagUpdater *updater = new TagUpdater(d, m_container.track(i));
            connect(updater, SIGNAL(destroyed(QObject *)),SIGNAL(listChanged()));
            d->show();
            return;
        }
    }
}

int PlayListModel::firstSelectedUpper(int row)
{
    for (int i = row - 1;i >= 0;i--)
    {
        if (isSelected(i))
            return i;
    }
    return -1;
}

int PlayListModel::firstSelectedLower(int row)
{
    for (int i = row + 1;i < count() ;i++)
    {
        if (isSelected(i))
            return i;
    }
    return -1;
}

int PlayListModel::totalLength() const
{
    return m_total_length;
}

void PlayListModel::moveItems(int from, int to)
{
    // Get rid of useless work
    /*if (from == to)
        return;

    QList<int> selected_rows = selectedIndexes();

    if (!(bottommostInSelection(from) == INVALID_ROW ||
            from == INVALID_ROW ||
            topmostInSelection(from) == INVALID_ROW)
       )
    {
        if (from > to)
            foreach(int i, selected_rows)
            if (i + to - from < 0)
                break;
            else
                m_items.move(i,i + to - from);
        else
            for (int i = selected_rows.count() - 1; i >= 0; i--)
                if (selected_rows[i] + to -from >= m_items.count())
                    break;
                else
                    m_items.move(selected_rows[i],selected_rows[i] + to - from);

        m_current = m_items.indexOf(m_currentItem);

        emit listChanged();
    }*/
}

int PlayListModel::topmostInSelection(int row)
{
    if (row == 0)
        return 0;

    for (int i = row - 1;i >= 0;i--)
    {
        if (isSelected(i))
            continue;
        else
            return i + 1;
    }
    return 0;
}

int PlayListModel::bottommostInSelection(int row)
{
    if (row >= count() - 1)
        return row;

    for (int i = row + 1; i < count(); i++)
    {
        if (isSelected(i))
            continue;
        else
            return i - 1;
    }
    return count() - 1;
}

const SimpleSelection& PlayListModel::getSelection(int row)
{
    m_selection.m_top = topmostInSelection(row);
    m_selection.m_anchor = row;
    m_selection.m_bottom = bottommostInSelection(row);
    m_selection.m_selected_indexes = selectedIndexes();
    return m_selection;
}

QList<int> PlayListModel::selectedIndexes() const
{
    QList<int> selected_rows;
    for (int i = 0; i < m_container.count(); i++)
    {
        if (m_container.item(i)->isSelected())
        {
            selected_rows.append(i);
        }
    }
    return selected_rows;
}

QList<PlayListTrack *> PlayListModel::selectedTracks()
{
    QList<PlayListTrack*> selected_tracks;
    foreach(PlayListItem *item, m_container.items())
    {
        if(!item->isGroup() && item->isSelected())
            selected_tracks.append(dynamic_cast<PlayListTrack *>(item));
    }
    return selected_tracks;
}

QList<PlayListItem *> PlayListModel::items() const
{
    return m_container.items();
}

void PlayListModel::addToQueue()
{
    QList<PlayListTrack*> selected_tracks = selectedTracks();
    foreach(PlayListTrack* track, selected_tracks)
        setQueued(track);
    emit listChanged();
}

void PlayListModel::setQueued(PlayListTrack *item)
{
    if (isQueued(item))
        m_queued_songs.removeAll(item);
    else
        m_queued_songs.enqueue(item);
    emit listChanged();
}

bool PlayListModel::isQueued(PlayListTrack *f) const
{
    return m_queued_songs.contains(f);
}

void PlayListModel::setCurrentToQueued()
{
    setCurrent(indexOf(m_queued_songs.dequeue()));
}

bool PlayListModel::isEmptyQueue() const
{
    return m_queued_songs.isEmpty();
}

int PlayListModel::queuedIndex(PlayListTrack *track) const
{
    return m_queued_songs.indexOf(track);
}

int PlayListModel::queueSize() const
{
    return m_queued_songs.size();
}

bool PlayListModel::isStopAfter(PlayListItem* item) const
{
    return m_stop_track == item;
}

void PlayListModel::randomizeList()
{
    /*for (int i = 0;i < m_items.size();i++)
        m_items.swap(qrand()%m_items.size(),qrand()%m_items.size());

    m_current = m_items.indexOf(m_currentItem);
    emit listChanged();*/
}

void PlayListModel::reverseList()
{
    /*for (int i = 0;i < m_items.size()/2;i++)
        m_items.swap(i,m_items.size() - i - 1);

    m_current = m_items.indexOf(m_currentItem);
    emit listChanged();*/
}
/*
////===============THE BEGINNING OF SORT IMPLEMENTATION =======================////

// First we'll implement bundle of static compare procedures
// to sort items in different ways

//by title
static bool _titleLessComparator(PlayListItem* s1,PlayListItem* s2)
{
    return QString::localeAwareCompare (s1->value(Qmmp::TITLE), s2->value(Qmmp::TITLE)) < 0;
}

static bool _titleGreaterComparator(PlayListItem* s1,PlayListItem* s2)
{
    return QString::localeAwareCompare (s1->value(Qmmp::TITLE), s2->value(Qmmp::TITLE)) > 0;
}

//by album+disc
static bool _discnumberLessComparator(PlayListItem* s1,PlayListItem* s2)
{
    return QString::localeAwareCompare (s1->value(Qmmp::DISCNUMBER), s2->value(Qmmp::DISCNUMBER)) < 0;
}

static bool _discnumberGreaterComparator(PlayListItem* s1,PlayListItem* s2)
{
    return QString::localeAwareCompare (s1->value(Qmmp::DISCNUMBER), s2->value(Qmmp::DISCNUMBER)) > 0;
}
//by album
static bool _albumLessComparator(PlayListItem* s1,PlayListItem* s2)
{
    return QString::localeAwareCompare (s1->value(Qmmp::ALBUM), s2->value(Qmmp::ALBUM)) < 0;
}

static bool _albumGreaterComparator(PlayListItem* s1,PlayListItem* s2)
{
    return QString::localeAwareCompare (s1->value(Qmmp::ALBUM), s2->value(Qmmp::ALBUM)) > 0;
}
//by artist
static bool _artistLessComparator(PlayListItem* s1,PlayListItem* s2)
{
    return QString::localeAwareCompare (s1->value(Qmmp::ARTIST), s2->value(Qmmp::ARTIST)) < 0;
}

static bool _artistGreaterComparator(PlayListItem* s1,PlayListItem* s2)
{
    return QString::localeAwareCompare (s1->value(Qmmp::ARTIST), s2->value(Qmmp::ARTIST)) > 0;
}
//by path
static bool _pathAndFilenameLessComparator(PlayListItem* s1,PlayListItem* s2)
{
    return QString::localeAwareCompare (s1->url(), s2->url()) < 0;
}

static bool _pathAndFilenameGreaterComparator(PlayListItem* s1,PlayListItem* s2)
{
    return QString::localeAwareCompare (s1->url(), s2->url()) > 0;
}
//by file name
static bool _filenameLessComparator(PlayListItem* s1,PlayListItem* s2)
{
    QFileInfo i_s1(s1->url());
    QFileInfo i_s2(s2->url());
    return QString::localeAwareCompare (i_s1.baseName(), i_s2.baseName()) < 0;
}

static bool _filenameGreaterComparator(PlayListItem* s1,PlayListItem* s2)
{
    QFileInfo i_s1(s1->url());
    QFileInfo i_s2(s2->url());
    return QString::localeAwareCompare (i_s1.baseName(), i_s2.baseName()) > 0;
}
//by date
static bool _dateLessComparator(PlayListItem* s1,PlayListItem* s2)
{
    return s1->value(Qmmp::YEAR).toInt() < s2->value(Qmmp::YEAR).toInt();
}

static bool _dateGreaterComparator(PlayListItem* s1,PlayListItem* s2)
{
    return s1->value(Qmmp::YEAR).toInt() > s2->value(Qmmp::YEAR).toInt();
}
//by track
static bool _trackLessComparator(PlayListItem* s1,PlayListItem* s2)
{
    return s1->value(Qmmp::TRACK).toInt() < s2->value(Qmmp::TRACK).toInt();
}

static bool _trackGreaterComparator(PlayListItem* s1,PlayListItem* s2)
{
    return s1->value(Qmmp::TRACK).toInt() > s2->value(Qmmp::TRACK).toInt();
}
//by file creation date
static bool _fileCreationDateLessComparator(PlayListItem* s1,PlayListItem* s2)
{
    return QFileInfo(s1->value(Qmmp::URL)).created() < QFileInfo(s2->value(Qmmp::URL)).created();
}

static bool _fileCreationDateGreaterComparator(PlayListItem* s1,PlayListItem* s2)
{
    return QFileInfo(s1->value(Qmmp::URL)).created() > QFileInfo(s2->value(Qmmp::URL)).created();
}
//by file modification date
static bool _fileModificationDateLessComparator(PlayListItem* s1,PlayListItem* s2)
{
    return QFileInfo(s1->value(Qmmp::URL)).lastModified() < QFileInfo(s2->value(Qmmp::URL)).lastModified();
}

static bool _fileModificationDateGreaterComparator(PlayListItem* s1,PlayListItem* s2)
{
    return QFileInfo(s1->value(Qmmp::URL)).lastModified() > QFileInfo(s2->value(Qmmp::URL)).lastModified();
}*/
// This is main sort method
void PlayListModel::doSort(int sort_mode,QList<PlayListItem*>& list_to_sort)
{
   /* QList<PlayListItem*>::iterator begin;
    QList<PlayListItem*>::iterator end;

    begin = list_to_sort.begin();
    end = list_to_sort.end();

    bool(*compareLessFunc)(PlayListItem*,PlayListItem*) = 0;
    bool(*compareGreaterFunc)(PlayListItem*,PlayListItem*) = 0;

    switch (sort_mode)
    {
    case TITLE:
        compareLessFunc = _titleLessComparator;
        compareGreaterFunc = _titleGreaterComparator;
        break;
    case DISCNUMBER:
        compareLessFunc = _discnumberLessComparator;
        compareGreaterFunc = _discnumberGreaterComparator;
        break;
    case ALBUM:
        compareLessFunc = _albumLessComparator;
        compareGreaterFunc = _albumGreaterComparator;
        break;
    case ARTIST:
        compareLessFunc = _artistLessComparator;
        compareGreaterFunc = _artistGreaterComparator;
        break;
    case FILENAME:
        compareLessFunc = _filenameLessComparator;
        compareGreaterFunc = _filenameGreaterComparator;
        break;
    case PATH_AND_FILENAME:
        compareLessFunc = _pathAndFilenameLessComparator;
        compareGreaterFunc = _pathAndFilenameGreaterComparator;
        break;
    case DATE:
        compareLessFunc = _dateLessComparator;
        compareGreaterFunc = _dateGreaterComparator;
        break;
        //qWarning("TODO Sort by Date: %s\t%d",__FILE__,__LINE__);
    case TRACK:
        compareLessFunc = _trackLessComparator;
        compareGreaterFunc = _trackGreaterComparator;
        break;
    case FILE_CREATION_DATE:
        compareLessFunc = _fileCreationDateLessComparator;
        compareGreaterFunc = _fileCreationDateGreaterComparator;
        break;
    case FILE_MODIFICATION_DATE:
        compareLessFunc = _fileModificationDateLessComparator;
        compareGreaterFunc = _fileModificationDateGreaterComparator;
        break;
    default:
        compareLessFunc = _titleLessComparator;
        compareGreaterFunc = _titleGreaterComparator;
    }

    static bool sorted_asc = false;
    if (!sorted_asc)
    {
        qStableSort(begin,end,compareLessFunc);
        sorted_asc = true;
    }
    else
    {
        qStableSort(begin,end,compareGreaterFunc);
        sorted_asc = false;
    }

    m_current = m_items.indexOf(m_currentItem);*/
}

void PlayListModel::sortSelection(int mode)
{
    /*QList<PlayListItem*>selected_items = selectedItems();
    QList<int>selected_rows = selectedIndexes();

    doSort(mode,selected_items);

    for (int i = 0;i < selected_rows.count();i++)
        m_items.replace(selected_rows[i],selected_items[i]);

    m_current = m_items.indexOf(m_currentItem);
    emit listChanged();*/
}

void PlayListModel::sort(int mode)
{
    /*doSort(mode,m_items);
    emit listChanged();*/
}

////=============== THE END OF SORT IMPLEMENTATION =======================////

void PlayListModel::prepareForShufflePlaying(bool val)
{
    if (m_play_state)
        delete m_play_state;

    if (val)
        m_play_state = new ShufflePlayState(this);
    else
        m_play_state = new NormalPlayState(this);

    m_shuffle = val;
}

void PlayListModel::prepareForRepeatablePlaying(bool val)
{
    m_is_repeatable_list = val;
}

void PlayListModel::doCurrentVisibleRequest()
{
    emit currentChanged();
    emit listChanged();
}

void PlayListModel::loadPlaylist(const QString &f_name)
{
    PlayListFormat* prs = PlayListParser::findByPath(f_name);
    if(!prs || !QFile::exists(f_name))
        return;

    QFile file(f_name);
    if (!file.open(QIODevice::ReadOnly))
    {
        qWarning("PlayListModel: %s", qPrintable(file.errorString()));
        return;
    }

    //clear();
    QStringList list = prs->decode(QTextStream(&file).readAll());
    if(list.isEmpty())
    {
        qWarning("PlayListModel: error opening %s",qPrintable(f_name));
    }
    for (int i = 0; i < list.size(); ++i)
    {
        if(list.at(i).contains("://"))
            continue;

        if (QFileInfo(list.at(i)).isRelative())
            list[i].prepend(QFileInfo(f_name).canonicalPath () + QDir::separator ());
    }
    m_loader->loadFiles(list);
    file.close();
}

void PlayListModel::savePlaylist(const QString & f_name)
{
    /*PlayListFormat* prs = PlayListParser::findByPath(f_name);
    if (prs)
    {
        QFile file(f_name);
        if (file.open(QIODevice::WriteOnly))
        {
            QTextStream ts(&file);
            QList <PlayListItem *> songs;
            foreach(PlayListItem* item, m_items)
            songs << item;
            ts << prs->encode(songs);
            file.close();
        }
        else
            qWarning("Error opening %s",f_name.toLocal8Bit().data());
    }*/
}

bool PlayListModel::isRepeatableList() const
{
    return m_is_repeatable_list;
}

bool PlayListModel::isShuffle() const
{
    return m_shuffle;
}

bool PlayListModel::isLoaderRunning() const
{
    return m_loader->isRunning();
}

void PlayListModel::preparePlayState()
{
    m_play_state->prepare();
}

void PlayListModel::removeInvalidTracks()
{
    bool ok = false;

    int i = 0;
    while(i < m_container.count())
    {
        PlayListTrack *track = m_container.track(i);
        if(!track)
        {
            i++;
            continue;
        }

        if(track->url().contains("://"))
            ok = MetaDataManager::instance()->protocols().contains(track->url().section("://",0,0));
        else
            ok = MetaDataManager::instance()->supports(track->url());
        if(!ok)
            removeTrack(i);
        else
            i++;
    }
}

void PlayListModel::removeDuplicates()
{
    for(int i = 0; i < m_container.count(); ++i)
    {
        if(!isTrack(i))
            continue;
        int j = m_container.count() - 1;
        while(j > i)
        {
            if(j < m_container.count() && isTrack(j))
            {
                if(track(i)->url() == track(j)->url())
                    removeTrack(j);
            }
            j--;
        }
    }
}

void PlayListModel::clearQueue()
{
     m_queued_songs.clear();
     m_stop_track = 0;
     emit listChanged();
}

void PlayListModel::stopAfterSelected()
{
    QList<PlayListTrack*> selected_tracks = selectedTracks();
    if(!m_queued_songs.isEmpty())
    {
        m_stop_track = m_stop_track != m_queued_songs.last() ? m_queued_songs.last() : 0;
    }
    else if(selected_tracks.count() == 1)
    {
        m_stop_track = m_stop_track != selected_tracks.at(0) ? selected_tracks.at(0) : 0;
    }
    else if(selected_tracks.count() > 1)
    {
        addToQueue();
        m_stop_track = m_queued_songs.last();
    }
    else
        return;
    emit listChanged();
}
