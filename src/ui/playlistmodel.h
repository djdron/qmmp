/***************************************************************************
 *   Copyright (C) 2006-2008 by Ilya Kotov                                 *
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
#ifndef PLAYLISTMODEL_H
#define PLAYLISTMODEL_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QPointer>
#include <QVector>

//#include "fileloader.h"
class FileLoader;

/**
   @author Ilya Kotov <forkotov02@hotmail.ru>
*/

class PlayListItem;
class PlayState;
class PlaylistFormat;
class PlayListModel;

struct SimpleSelection
{
    SimpleSelection()
    {
        ;
    }
    inline bool isValid()const
    {
        return (m_bottom != -1) && (m_anchor != -1) && (m_top != -1);
    }
    inline void dump()const
    {
        qWarning("top: %d\tbotom: %d\tanchor: %d",m_top,m_bottom,m_anchor);
    }
    inline int count()const
    {
        return m_bottom - m_top + 1;
    }
    int m_bottom;
    int m_top;
    int m_anchor;
    QList<int>m_selected_rows;
};

/*!
 * Helper class used for tags update after details dialog closing.
 * @author Vladimir Kuznetsov <vovanec@gmail.com>
 */
class TagUpdater : public QObject
{
    Q_OBJECT
    QObject* m_observable;
    PlayListItem* m_item;
public:
    TagUpdater(QObject* o, PlayListItem* item);
protected slots:
    void updateTag();
};


class PlayListModel : public QObject
{
    Q_OBJECT
public:
    PlayListModel(QObject *parent = 0);

    ~PlayListModel();

    int count();
    PlayListItem* currentItem();
    int row(PlayListItem* item)const
    {
        return m_items.indexOf(item);
    }
    PlayListItem* item(int row)const
    {
        return m_items.at(row);
    }
    int currentRow();
    bool setCurrent (int);
    bool isSelected(int);
    void setSelected(int, bool);

    bool next();
    bool previous();

    QList <QString> getTitles(int,int);
    QList <QString> getTimes(int,int);

    void moveItems(int from,int to);

    /*!
     * Returns \b true if \b f file is in play queue, else return \b false
     */
    bool isQueued(PlayListItem* item) const;

    bool isRepeatableList()const
    {
        return is_repeatable_list;
    }

    /*!
     * Sets current song to the file that is nex in queue, if queue is empty - does nothing
     */
    void setCurrentToQueued();

    /*!
     * Returns \b true if play queue is empty,otherwise - \b false.
     */
    bool isEmptyQueue()const;

    /*!
     * Returns index of \b f file in queue.e
     */
    int queuedIndex(PlayListItem* item)const
    {
        return m_queued_songs.indexOf(item);
    }

    /*!
     * Returns current selection(playlist can contain a lot of selections,
     * this method returns selection which \b row belongs to)
     */
    const SimpleSelection& getSelection(int row);

    /*!
     * Returns vector with selected rows indexes.
     */
    QList<int> getSelectedRows()const;
    /*!
     * Returns vector of \b PlayListItem pointers that are selected.
     */
    QList<PlayListItem*> getSelectedItems()const;

    QList<PlayListItem*> items()const
    {
        return m_items;
    }

    /*!
     * Returns number of first item that selected upper the \b row item.
     */
    int firstSelectedUpper(int row);

    /*!
     * Returns number of first item that selected lower the \b row item.
     */
    int firstSelectedLower(int row);

    /*!
     * Returns total lenght in seconds of all songs.
     */
    int totalLength()const
    {
        return m_total_length;
    }

    /*!
     * Loads playlist with \b f_name name.
     */
    void loadPlaylist(const QString& f_name);

    /*!
     * Saves current songs to the playlist with \b f_name name.
     */
    void savePlaylist(const QString& f_name);

    /*!
     * Enum of available sort modes
     */
    enum SortMode
    {
        TITLE,FILENAME,PATH_AND_FILENAME,DATE,TRACK
    };

signals:
    void listChanged();
    void currentChanged();
    void firstAdded();

public slots:
    void load(PlayListItem *);
    void clear();
    void clearSelection();
    void removeSelected();
    void removeUnselected();
    void invertSelection();
    void selectAll();
    void showDetails();
    void doCurrentVisibleRequest();

    void addFile(const QString&);

    /*!
     * Adds the list \b l of files to the model.
     */
    void addFiles(const QStringList& l);

    /*!
     * Adds \b dir to the model.
     */
    void addDirectory(const QString& dir);

    /*!
     * Loads list of files (regular files or directories),
     * returns \b TRUE if at least one file has been successfully loaded,
     * otherwise \b FALSE
     */
    bool setFileList(const QStringList&);

    void addFileList(const QStringList &l);

    void randomizeList();
    void reverseList();

    /*!
     * Prepares model for shuffle playing. \b yes parameter is true - model iterates in shuffle mode.
     */
    void prepareForShufflePlaying(bool yes);

    /*!
      * Prepares model for shuffle playing. \b yes parameter is true - model iterates in repeat mode.
    */
    void prepareForRepeatablePlaying(bool);

    /*!
     * Sorts selected items in \b mode sort mode.
     */
    void sortSelection(int mode);

    /*!
     * Sorts items in \b mode sort mode.
     */
    void sort(int mode);

    /*!
     * Adds selected items to play queue.
     */
    void addToQueue();

    /*!
     * Sets \b f media file to queue.
     */
    void setQueued(PlayListItem* f);

    void preparePlayState();

private:

    /*!
     * This internal method performs sorting of \b list_to_sort list of items.
     */
    void doSort(int mode,QList<PlayListItem*>& list_to_sort);
    /*!
     * Returns topmost row in current selection
     */
    int topmostInSelection(int);

    /*!
     * Returns bottommost row in current selection
     */
    int bottommostInSelection(int);

    /*!
     * Creates and initializes file loader object.
     */
    FileLoader* createFileLoader();


    /*!
     * Is someone of file loaders is running?
     */
    bool isFileLoaderRunning()const;

    /*!
     * Removes items from model. If \b inverted is \b false -
     * selected items will be removed, else - unselected.
     */
    void removeSelection(bool inverted = false);

private:

    QList <PlayListItem*> m_items;
    QList <PlayListItem*> m_editing_items;
    PlayListItem* m_currentItem;

    int m_current;
    void readSettings();
    void writeSettings();

    void setUpdatesEnabled(bool);

    bool updatesEnabled()const
    {
        return !m_block_update_signals;
    }

    /*!
     * This flyweight object represents current selection.
     */
    SimpleSelection m_selection;

    /*!
     * Songs in play queue.
     */
    QList<PlayListItem*>m_queued_songs;

    /*!
     * Is playlist repeatable?
     */
    bool is_repeatable_list;

    /// Current playing state (Normal or Shuffle)
    PlayState* m_play_state;

    bool m_block_update_signals;

    int m_total_length;

    typedef QPointer<FileLoader> GuardedFileLoader;

    /*! Vector of currently running file loaders.
     *  All loaders are automatically sheduled for deletion
     * when finished.
     */
    QVector<GuardedFileLoader> m_running_loaders;

    friend class MainWindow;
};


#endif
