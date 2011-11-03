/***************************************************************************
 *   Copyright(C) 2006-2011 by Ilya Kotov                                  *
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
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
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
#include <time.h>
#include <qmmp/metadatamanager.h>
#include "playlistparser.h"
#include "playlistformat.h"
#include "fileloader_p.h"
#include "playlistitem.h"
#include "playstate_p.h"
#include "detailsdialog.h"
#include "playlistsettings_p.h"
#include "playlistmodel.h"


#define INVALID_ROW -1

TagUpdater::TagUpdater(QObject* o,PlayListItem* item):m_observable(o),m_item(item)
{
    m_item->setFlag(PlayListItem::EDITING);
    connect(m_observable, SIGNAL(destroyed(QObject *)),SLOT(updateTag()));
    connect(m_observable, SIGNAL(destroyed(QObject *)),SLOT(deleteLater()));
}

void TagUpdater::updateTag()
{
    if (m_item->flag() == PlayListItem::SCHEDULED_FOR_DELETION)
    {
        delete m_item;
        m_item = NULL;
    }
    else
    {
        m_item->updateTags();
        m_item->setFlag(PlayListItem::FREE);
    }
}

PlayListModel::PlayListModel(const QString &name, QObject *parent)
        : QObject(parent) , m_selection()
{
    qsrand(time(0));
    m_name = name;
    m_shuffle = 0;
    m_total_length = 0;
    m_current = 0;
    is_repeatable_list = false;
    m_stop_item = 0;
    m_play_state = new NormalPlayState(this);
    m_loader = new FileLoader(this);
    connect(m_loader, SIGNAL(newPlayListItem(PlayListItem*)),
            SLOT(add(PlayListItem*)), Qt::QueuedConnection);
    connect(m_loader, SIGNAL(finished()), SLOT(preparePlayState()));
    connect(m_loader, SIGNAL(finished()), SIGNAL(loaderFinished()));
}

PlayListModel::~PlayListModel()
{
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

void PlayListModel::add(PlayListItem *item)
{
    if (m_items.isEmpty())
        m_currentItem = item;

    m_total_length += item->length();
    m_items << item;
    m_current = m_items.indexOf(m_currentItem);

    emit itemAdded(item);
    emit listChanged();
}

void PlayListModel::add(QList <PlayListItem *> items)
{
    if(items.isEmpty())
        return;
    if (m_items.isEmpty())
        m_currentItem = items.at(0);

    m_items << items;
    m_current = m_items.indexOf(m_currentItem);
    foreach(PlayListItem *item, items)
    {
        m_total_length += item->length();
        emit itemAdded(item);
    }
    emit listChanged();
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

int PlayListModel::count()
{
    return m_items.size();
}

PlayListItem* PlayListModel::currentItem()
{
    return m_items.isEmpty() ? 0 : m_items.at(qMin(m_items.size() - 1, m_current));
}

PlayListItem* PlayListModel::nextItem()
{
    if(m_items.isEmpty() || !m_play_state)
        return 0;
    if(m_stop_item && m_stop_item == currentItem())
        return 0;
    if(!isEmptyQueue())
        return m_queued_songs.at(0);
    int index = m_play_state->nextIndex();
    if(index < 0 || (index + 1 > m_items.count()))
        return 0;
    return m_items.at(index);
}

PlayListItem* PlayListModel::item(int row) const
{
    return (row <  m_items.size() && row >= 0) ? m_items.at(row) : 0;
}

int PlayListModel::currentRow()
{
    return m_current;
}

bool PlayListModel::setCurrent(int c)
{
    if (c > count()-1 || c < 0)
        return false;
    m_current = c;
    m_currentItem = m_items.at(c);
    emit currentChanged();
    emit listChanged();
    return true;
}

bool PlayListModel::setCurrent(PlayListItem *item)
{
    if(!m_items.contains(item))
        return false;
    return setCurrent(m_items.indexOf(item));
}

bool PlayListModel::next()
{
    if(m_stop_item == currentItem())
    {
        m_stop_item = 0;
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
    m_stop_item = 0;
    while (!m_items.isEmpty())
    {
        PlayListItem* mf = m_items.takeFirst();

        if (mf->flag() == PlayListItem::FREE)
        {
            delete mf;
        }
        else if (mf->flag() == PlayListItem::EDITING)
        {
            mf->setFlag(PlayListItem::SCHEDULED_FOR_DELETION);
        }
    }
    m_queued_songs.clear();

    m_total_length = 0;
    m_play_state->resetState();
    emit listChanged();
}

void PlayListModel::clearSelection()
{
    for (int i = 0; i<m_items.size(); ++i)
        m_items.at(i)->setSelected(false);
    emit listChanged();
}

QStringList PlayListModel::getTitles(int b,int l)
{
    QList <QString> m_titles;
    for (int i = b;(i < b + l) &&(i < m_items.size()); ++i)
        m_titles << m_items.at(i)->text();
    return m_titles;
}

QStringList PlayListModel::getTimes(int b,int l)
{
    QList <QString> m_times;
    int d = 0;
    QString str;
    for (int i = b; (i < b + l) && (i < m_items.size()); ++i)
    {
        d = m_items.at(i)->length();
        if(d > 3600)
            str += QString("%1:%2").arg(d/3600).arg(d%3600/60, 2, 10, QChar('0'));
        else
            str += QString("%1").arg(d%3600/60);
        str += QString(":%1").arg(d%60, 2, 10, QChar('0'));
        if(!d)
            str.clear();
        m_times << str;
        str.clear();
    }
    return m_times;
}

bool PlayListModel::isSelected(int row)
{
    if (m_items.count() > row && row >= 0)
        return m_items.at(row)->isSelected();

    return false;
}

bool PlayListModel::contains(const QString &url)
{
    foreach (PlayListItem *item, m_items)
    {
        if(item->url() == url)
            return true;
    }
    return false;
}

void PlayListModel::setSelected(int row, bool selected)
{
    if (m_items.count() > row && row >= 0)
    {
        m_items.at(row)->setSelected(selected);
        emit listChanged();
    }
}

void PlayListModel::removeSelected()
{
    removeSelection(false);
}

void PlayListModel::removeUnselected()
{
    removeSelection(true);
}

void PlayListModel::removeAt (int i)
{
    if ((i < count()) && (i >= 0))
    {
        PlayListItem* f = m_items.takeAt(i);
        if(m_stop_item == f)
            m_stop_item = 0;
        m_total_length -= f->length();
        if (m_total_length < 0)
            m_total_length = qMin(0, m_total_length);

        if (f->flag() == PlayListItem::FREE)
        {
            delete f;
            f = NULL;
        }
        else if (f->flag() == PlayListItem::EDITING)
            f->setFlag(PlayListItem::SCHEDULED_FOR_DELETION);

        if (m_current >= i && m_current != 0)
            m_current--;

        if (!m_items.isEmpty())
            m_currentItem = m_items.at(m_current);

        m_play_state->prepare();
        emit listChanged();
    }
}

void PlayListModel::removeItem (PlayListItem *item)
{
    if(m_items.contains(item))
        removeAt (m_items.indexOf(item));
}

void PlayListModel::removeSelection(bool inverted)
{
    int i = 0;

    int select_after_delete = -1;

    while (!m_items.isEmpty() && i<m_items.size())
    {
        if (m_items.at(i)->isSelected() ^ inverted)
        {
            PlayListItem* f = m_items.takeAt(i);
            if(f == m_stop_item)
                m_stop_item = 0;
            m_total_length -= f->length();
            if (m_total_length < 0)
                m_total_length = 0;

            if (f->flag() == PlayListItem::FREE)
            {
                delete f;
                f = NULL;
            }
            else if (f->flag() == PlayListItem::EDITING)
                f->setFlag(PlayListItem::SCHEDULED_FOR_DELETION);

            select_after_delete = i;

            if (m_current >= i && m_current!=0)
                m_current--;
        }
        else
            i++;
    }

    if (!m_items.isEmpty())
        m_currentItem = m_items.at(m_current);

    if (select_after_delete >= m_items.count())
        select_after_delete = m_items.count() - 1;

    if(select_after_delete != -1)
        m_items.at(select_after_delete)->setSelected(true);

    m_play_state->prepare();

    emit listChanged();
}

void PlayListModel::invertSelection()
{
    for (int i = 0; i<m_items.size(); ++i)
        m_items.at(i)->setSelected(!m_items.at(i)->isSelected());
    emit listChanged();
}

void PlayListModel::selectAll()
{
    for (int i = 0; i<m_items.size(); ++i)
        m_items.at(i)->setSelected(true);
    emit listChanged();
}

void PlayListModel::showDetails(QWidget *parent)
{
    for (int i = 0; i<m_items.size(); ++i)
    {
        if (m_items.at(i)->isSelected())
        {
            QDialog *d = new DetailsDialog(m_items.at(i), parent);
            TagUpdater *updater = new TagUpdater(d, m_items.at(i));
            m_editing_items.append(m_items.at(i));
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

void PlayListModel::moveItems(int from, int to)
{
    // Get rid of useless work
    if (from == to)
        return;

    QList<int> selected_rows = selectedRows();

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
    if (row >= m_items.count() - 1)
        return row;

    for (int i = row + 1;i < count() ;i++)
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
    m_selection.m_selected_rows = selectedRows();
    return m_selection;
}

QList<int> PlayListModel::selectedRows() const
{
    QList<int>selected_rows;
    for (int i = 0;i<m_items.count();i++)
    {
        if (m_items[i]->isSelected())
        {
            selected_rows.append(i);
        }
    }
    return selected_rows;
}

QList< PlayListItem * > PlayListModel::selectedItems() const
{
    QList<PlayListItem*>selected_items;
    for (int i = 0;i<m_items.count();i++)
    {
        if (m_items[i]->isSelected())
        {
            selected_items.append(m_items[i]);
        }
    }
    return selected_items;
}

void PlayListModel::addToQueue()
{
    QList<PlayListItem*> selected_items = selectedItems();
    foreach(PlayListItem* file,selected_items)
        setQueued(file);
    emit listChanged();
}

void PlayListModel::setQueued(PlayListItem* file)
{
    if (isQueued(file))
        m_queued_songs.removeAll(file);
    else
        m_queued_songs.enqueue(file);
    emit listChanged();
}

bool PlayListModel::isQueued(PlayListItem* f) const
{
    return m_queued_songs.contains(f);
}

void PlayListModel::setCurrentToQueued()
{
    setCurrent(row(m_queued_songs.dequeue()));
}

bool PlayListModel::isEmptyQueue() const
{
    return m_queued_songs.isEmpty();
}

int PlayListModel::queuedIndex(PlayListItem* item) const
{
    return m_queued_songs.indexOf(item);
}

int PlayListModel::queueSize() const
{
    return m_queued_songs.size();
}

bool PlayListModel::isStopAfter(PlayListItem* item) const
{
    return m_stop_item == item;
}

void PlayListModel::randomizeList()
{
    for (int i = 0;i < m_items.size();i++)
        m_items.swap(qrand()%m_items.size(),qrand()%m_items.size());

    m_current = m_items.indexOf(m_currentItem);
    emit listChanged();
}

void PlayListModel::reverseList()
{
    for (int i = 0;i < m_items.size()/2;i++)
        m_items.swap(i,m_items.size() - i - 1);

    m_current = m_items.indexOf(m_currentItem);
    emit listChanged();
}

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
    return QString::localeAwareCompare (s1->value(Qmmp::ALBUM), s2->value(Qmmp::ALBUM)) < 0;
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

// This is main sort method
void PlayListModel::doSort(int sort_mode,QList<PlayListItem*>& list_to_sort)
{
    QList<PlayListItem*>::iterator begin;
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
    default:
        compareLessFunc = _titleLessComparator;
        compareGreaterFunc = _titleGreaterComparator;
    }

    static bool sorted_asc = false;
    if (!sorted_asc)
    {
        qSort(begin,end,compareLessFunc);
        sorted_asc = true;
    }
    else
    {
        qSort(begin,end,compareGreaterFunc);
        sorted_asc = false;
    }

    m_current = m_items.indexOf(m_currentItem);
}

void PlayListModel::sortSelection(int mode)
{
    QList<PlayListItem*>selected_items = selectedItems();
    QList<int>selected_rows = selectedRows();

    doSort(mode,selected_items);

    for (int i = 0;i < selected_rows.count();i++)
        m_items.replace(selected_rows[i],selected_items[i]);

    m_current = m_items.indexOf(m_currentItem);
    emit listChanged();
}

void PlayListModel::sort(int mode)
{
    doSort(mode,m_items);
    emit listChanged();
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
    is_repeatable_list = val;
}

void PlayListModel::doCurrentVisibleRequest()
{
    emit currentChanged();
    emit listChanged();
}

void PlayListModel::loadPlaylist(const QString &f_name)
{
    PlaylistFormat* prs = PlaylistParser::instance()->findByPath(f_name);
    if(!prs)
    {
        qWarning("PlayListModel: unsupported playlist format");
        return;
    }

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
    PlaylistFormat* prs = PlaylistParser::instance()->findByPath(f_name);
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
    }
}

bool PlayListModel::isRepeatableList() const
{
    return is_repeatable_list;
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

void PlayListModel::removeInvalidItems()
{
    foreach(PlayListItem *item, m_items)
    {
        bool ok = false;
        if(!item->url().contains("://"))
            ok = MetaDataManager::instance()->supports(item->url());
        else
            ok = MetaDataManager::instance()->protocols().contains(item->url().section("://",0,0));
        if(!ok)
            removeItem(item);
    }
}

void PlayListModel::removeDuplicates()
{
    for(int i = 0; i < m_items.size(); ++i)
    {
        for(int j = i + 1; j < m_items.size(); ++j)
        {
            if(m_items.at(i)->url() == m_items.at(j)->url())
                removeItem(m_items.at(j));
        }
    }
}

void PlayListModel::clearQueue()
{
     m_queued_songs.clear();
     m_stop_item = 0;
     emit listChanged();
}

void PlayListModel::stopAfterSelected()
{
    QList<PlayListItem*> selected_items = selectedItems();
    if(!m_queued_songs.isEmpty())
    {
        m_stop_item = m_stop_item != m_queued_songs.last() ? m_queued_songs.last() : 0;
    }
    else if(selected_items.count() == 1)
    {
        m_stop_item = m_stop_item != selected_items.at(0) ? selected_items.at(0) : 0;
    }
    else if(selected_items.count() > 1)
    {
        addToQueue();
        m_stop_item = m_queued_songs.last();
    }
    else
        return;
    emit listChanged();
}
