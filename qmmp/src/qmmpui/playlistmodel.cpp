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
#include <QtAlgorithms>
#include <QTextStream>
#include <time.h>
#include <qmmp/metadatamanager.h>
#include "playlistparser.h"
#include "playlistformat.h"
#include "playlistcontainer_p.h"
#include "groupedcontainer_p.h"
#include "normalcontainer_p.h"
#include "fileloader_p.h"
#include "playstate_p.h"
#include "detailsdialog.h"
#include "tagupdater_p.h"
#include "playlistmodel.h"

#define INVALID_INDEX -1

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
    m_container = new NormalContainer;
    connect(m_loader, SIGNAL(newPlayListTrack(PlayListTrack*)),
            SLOT(add(PlayListTrack*)), Qt::QueuedConnection);
    connect(m_loader, SIGNAL(finished()), SLOT(preparePlayState()));
    connect(m_loader, SIGNAL(finished()), SIGNAL(loaderFinished()));
}

PlayListModel::~PlayListModel()
{
    blockSignals(true);
    m_loader->finish();
    clear();
    delete m_play_state;
    delete m_container;
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
    m_container->addTrack(track);
    m_total_length += track->length();

    if(m_container->trackCount() == 1)
    {
        m_current_track = track;
        m_current = m_container->indexOf(track);
        emit currentChanged();
    }
    emit itemAdded(track);
    emit listChanged();
    emit countChanged();
}

void PlayListModel::add(QList<PlayListTrack *> tracks)
{
    if(tracks.isEmpty())
        return;

    foreach(PlayListTrack *track, tracks)
    {
        m_container->addTrack(track);
        m_total_length += track->length();
        if(m_container->trackCount() == 1)
        {
            m_current_track = track;
            m_current = m_container->indexOf(track);
            emit currentChanged();
        }
        emit itemAdded(track);
    }
    preparePlayState();
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
    return m_container->count();
}

int PlayListModel::trackCount() const
{
    return m_container->trackCount();
}

PlayListTrack* PlayListModel::currentTrack() const
{
    return m_container->isEmpty() ? 0 : m_current_track;
}

PlayListTrack *PlayListModel::nextTrack() const
{
    if(m_container->isEmpty() || !m_play_state)
        return 0;
    if(m_stop_track && m_stop_track == currentTrack())
        return 0;
    if(!isEmptyQueue())
        return m_queued_songs.at(0);
    int index = m_play_state->nextIndex();
    if(index < 0 || (index + 1 > m_container->count()))
        return 0;
    return m_container->track(index);
}

int PlayListModel::indexOf(PlayListItem* item) const
{
    return m_container->indexOf(item);
}

PlayListItem* PlayListModel::item(int index) const
{
    return m_container->item(index);
}

PlayListTrack* PlayListModel::track(int index) const
{
    return m_container->track(index);
}

PlayListGroup* PlayListModel::group(int index) const
{
    return m_container->group(index);
}

int PlayListModel::currentIndex() const
{
    return m_current;
}

bool PlayListModel::setCurrent(int index)
{
    if (index > count()-1 || index < 0)
        return false;
    PlayListItem *item = m_container->item(index);
    if(item->isGroup())
    {
        index++;
        item = m_container->item(index);
    }
    m_current = index;
    m_current_track = dynamic_cast<PlayListTrack*> (item);
    emit currentChanged();
    emit listChanged();
    return true;
}

bool PlayListModel::setCurrent(PlayListTrack *track)
{
    if(!m_container->contains(track))
        return false;
    return setCurrent(m_container->indexOf(track));
}

bool PlayListModel::isTrack(int index) const
{
    if (index > count()-1 || index < 0)
        return false;
    return !m_container->item(index)->isGroup();
}

bool PlayListModel::isGroup(int index) const
{
    if (index > count()-1 || index < 0)
        return false;
    return !m_container->item(index)->isGroup();
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
    m_container->clear();
    m_queued_songs.clear();
    m_total_length = 0;
    m_play_state->resetState();
    emit listChanged();
    emit countChanged();
}

void PlayListModel::clearSelection()
{
    m_container->clearSelection();
    emit listChanged();
}

QList<PlayListItem *> PlayListModel::mid(int pos, int count) const
{
    return m_container->mid(pos, count);
}

bool PlayListModel::isSelected(int index) const
{
    return m_container->isSelected(index);
}

bool PlayListModel::contains(const QString &url)
{
    for(int i = 0; i < m_container->count(); ++i)
    {
        PlayListTrack *t = track(i);
        if(!t)
            continue;
        if(t->url() == url)
            return true;
    }
    return false;
}

int PlayListModel::numberOfTrack(int index) const
{
    return m_container->numberOfTrack(index);
}

void PlayListModel::setSelected(int index, bool selected)
{
    m_container->setSelected(index, selected);
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
    bool current_changed = false;
    if ((i < count()) && (i >= 0))
    {
        PlayListTrack* track = m_container->track(i);
        if(!track)
            return;
        m_queued_songs.removeAll(track);
        m_container->removeTrack(track);
        if(m_stop_track == track)
            m_stop_track = 0;
        m_total_length -= track->length();
        m_total_length = qMax(0, m_total_length);

        if(m_current_track == track)
        {
            if(m_container->isEmpty())
                m_current_track = 0;
            else
            {
                current_changed = true;
                int current = qMin(i - 1, m_container->count() - 2);
                current = qMax(current, 0);
                m_current_track = m_container->track(current);
                if(!m_current_track)
                {
                    m_current_track = current > 0 ? m_container->track(current-1) :
                                                    m_container->track(1);
                }
            }
        }

        if (track->flag() == PlayListTrack::FREE)
        {
            delete track;
            track = NULL;
        }
        else if (track->flag() == PlayListTrack::EDITING)
            track->setFlag(PlayListTrack::SCHEDULED_FOR_DELETION);


        m_current = m_current_track ? m_container->indexOf(m_current_track) : -1;
        m_play_state->prepare();

        if(current_changed)
            emit currentChanged();

        emit listChanged();
        emit countChanged();
    }
}

void PlayListModel::removeTrack (PlayListItem *track)
{
    if(m_container->contains(track))
        removeTrack (m_container->indexOf(track));
}

void PlayListModel::removeSelection(bool inverted)
{
    int i = 0;
    int select_after_delete = -1;
    PlayListTrack *prev_current_track = m_current_track;

    while (!m_container->isEmpty() && i < m_container->count())
    {
        PlayListItem *item = m_container->item(i);
        if (!item->isGroup() && item->isSelected() ^ inverted)
        {
            blockSignals(true);
            removeTrack(i);
            blockSignals(false);

            if(m_container->isEmpty())
                continue;

            select_after_delete = i;
        }
        else
            i++;
    }

    if (select_after_delete >= m_container->count())
        select_after_delete = m_container->count() - 1;

    if(select_after_delete != -1)
        m_container->setSelected(select_after_delete, true);

    m_play_state->prepare();

    if(prev_current_track != m_current_track)
        emit currentChanged();

    emit listChanged();
    emit countChanged();
}

void PlayListModel::invertSelection()
{
    for (int i = 0; i < m_container->count(); ++i)
        m_container->setSelected(i, !m_container->isSelected(i));
    emit listChanged();
}

void PlayListModel::selectAll()
{
    for (int i = 0; i < m_container->count(); ++i)
        m_container->setSelected(i, true);
    emit listChanged();
}

void PlayListModel::showDetails(QWidget *parent)
{
    for (int i = 0; i < m_container->count(); ++i)
    {
        if (m_container->isSelected(i) && m_container->track(i))
        {
            QDialog *d = new DetailsDialog(m_container->track(i), parent);
            TagUpdater *updater = new TagUpdater(d, m_container->track(i));
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
    if (from == to)
        return;

    QList<int> selected_indexes = selectedIndexes();

    if(selected_indexes.isEmpty())
        return;

    foreach(int i, selected_indexes) //do no move groups
    {
        if(!isTrack(i))
            return;
    }

    if (bottommostInSelection(from) == INVALID_INDEX ||
            from == INVALID_INDEX ||
            topmostInSelection(from) == INVALID_INDEX)
        return;

    if(m_container->move(selected_indexes, from, to))
    {
        m_current = m_container->indexOf(m_current_track);
        emit listChanged();
    }
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
    for (int i = 0; i < m_container->count(); i++)
    {
        if (m_container->item(i)->isSelected())
        {
            selected_rows.append(i);
        }
    }
    return selected_rows;
}

QList<PlayListTrack *> PlayListModel::selectedTracks() const
{
    QList<PlayListTrack*> selected_tracks;
    foreach(PlayListItem *item, m_container->items())
    {
        if(!item->isGroup() && item->isSelected())
            selected_tracks.append(dynamic_cast<PlayListTrack *>(item));
    }
    return selected_tracks;
}

QList<PlayListItem *> PlayListModel::items() const
{
    return m_container->items();
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
    if(m_container->isEmpty())
        return;
    m_container->randomizeList();
    m_current = m_container->indexOf(m_current_track);
    emit listChanged();
}

void PlayListModel::reverseList()
{
    if(m_container->isEmpty())
        return;
    m_container->reverseList();
    m_current = m_container->indexOf(m_current_track);
    emit listChanged();
}

void PlayListModel::sortSelection(int mode)
{
    if(m_container->isEmpty())
        return;
    m_container->sortSelection(mode);
    m_current = m_container->indexOf(m_current_track);
    emit listChanged();
}

void PlayListModel::sort(int mode)
{
    if(m_container->isEmpty())
        return;
    m_container->sort(mode);
    m_current = m_container->indexOf(m_current_track);
    emit listChanged();
}

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

void PlayListModel::prepareGroups(bool enabled)
{
    PlayListContainer *container = 0;
    if(enabled)
        container = new GroupedContainer;
    else
        container = new NormalContainer;
    container->addTracks(m_container->takeAllTracks());
    delete m_container;
    m_container = container;
    if(!m_container->isEmpty())
        m_current = m_container->indexOf(m_current_track);
    emit listChanged();
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
    PlayListFormat* prs = PlayListParser::findByPath(f_name);
    if (!prs)
        return;

    QFile file(f_name);
    if (file.open(QIODevice::WriteOnly))
    {
        QTextStream ts(&file);
        QList <PlayListTrack *> songs;
        for(int i = 0; i < m_container->count(); ++i)
        {
            if(!isTrack(i))
                continue;
            songs << m_container->track(i);
        }
        ts << prs->encode(songs);
        file.close();
    }
    else
        qWarning("Error opening %s",f_name.toLocal8Bit().data());

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

    for(int i = m_container->count() - 1; i >= 0; i--)
    {
        if(i >= m_container->count() || !isTrack(i))
            continue;

        PlayListTrack *track = m_container->track(i);

        if(track->url().contains("://"))
            ok = MetaDataManager::instance()->protocols().contains(track->url().section("://",0,0));
        else
            ok = MetaDataManager::instance()->supports(track->url());
        if(!ok)
            removeTrack(i);

    }
}

void PlayListModel::removeDuplicates()
{
    for(int i = 0; i < m_container->count(); ++i)
    {
        if(!isTrack(i))
            continue;
        int j = m_container->count() - 1;
        while(j > i)
        {
            if(j < m_container->count() && isTrack(j))
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
