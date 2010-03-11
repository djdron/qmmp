/***************************************************************************
 *   Copyright (C) 2006-2009 by Ilya Kotov                                 *
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
#include "playlistitem.h"

class FileLoader;
class PlayListItem;
class PlayState;
class PlaylistFormat;
class PlayListModel;

/*! @brief Helper class that keeps track of a view's selected items.
 *
 * @author Vladimir Kuznetsov <vovanec@gmail.com>
 */
struct SimpleSelection
{
    /*!
     * Constructs an empty selection model.
     */
    SimpleSelection()
    {
        ;
    }
    /*!
     * Returns \p true if this selection is valid; otherwise returns returns \p false.
     */
    inline bool isValid()const
    {
        return (m_bottom != -1) && (m_anchor != -1) && (m_top != -1);
    }
    /*!
     * Prints object state.
     */
    inline void dump()const
    {
        qWarning("top: %d\tbotom: %d\tanchor: %d",m_top,m_bottom,m_anchor);
    }
    /*!
     * Returns number of selected items.
     */
    inline int count()const
    {
        return m_bottom - m_top + 1;
    }
    int m_bottom;               /*!<    */
    int m_top;                  /*!<    */
    int m_anchor;               /*!<    */
    QList<int>m_selected_rows;  /*!< Selected rows numbers */
};
/*! @internal
 * @brief Helper class used for tags update after details dialog closing.
 *
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
/*! @brief The PlayListModel class provides a data model for the playlist.
 *
 * @author Vladimir Kuznetsov <vovanec@gmail.com>
 * @author Ilya Kotov <forkotov02@hotmail.ru>
 */
class PlayListModel : public QObject
{
    Q_OBJECT
public:
    /*!
     * Constructs a playlist model.
     * @param parent QObject parent
     */
    PlayListModel(const QString &name, QObject *parent = 0);
    /*!
     * Object destructor.
     */
    ~PlayListModel();
    /*!
     * Returns playlist name.
     */
    QString name() const;
    /*!
     * Sets the name of the playlist to \b name.
     */
    void setName(const QString &name);
    /*!
     * Returns number of items.
     */
    int count();
    /*!
     * Returns the current item.
     */
    PlayListItem* currentItem();
    /*!
     * Returns the next playing item or 0 if next item is unknown.
     */
    PlayListItem* nextItem();
    /*!
     * Returns the row of the \b item
     */
    int row(PlayListItem* item) const
    {
        return m_items.indexOf(item);
    }
    /*!
     * Returns the item of the \b row or 0 if item doesn't exist.
     */
    PlayListItem* item(int row) const;
    /*!
     * Returns current row.
     */
    int currentRow();
    /*!
     * Sets current row number.
     * Returns \b false if item with this number doesn't exist, otherwise returns \b true
     * @param row Number of item.
     */
    bool setCurrent (int row);
    /*!
     * Returns \b true if \b row is selected, otherwise returns \b false
     */
    bool isSelected(int row);
    /*!
     * Sets the selected state of the item to \b select
     * @param row Number of item.
     * @param select Selection state (\b true - select, \b false - unselect)
     */
    void setSelected(int row, bool select);
    /*!
     * Advances to the next item. Returns \b false if next iten doesn't exist,
     * otherwise returns \b true
     */
    bool next();
    /*!
     * Goes back to the previous item. Returns \b false if previous iten doesn't exist,
     * otherwise returns \b true
     */
    bool previous();
    /*!
     * Returns a list of the formated titles.
     * @param first First item.
     * @param last Last item.
     */
    QList <QString> getTitles(int first,int last);
    /*!
     * Returns a list of the formated durations.
     * @param first First item.
     * @param last Last item.
     */
    QList <QString> getTimes(int first,int last);
    /*!
     *  Moves the item at index position \b from to index position \b to
     */
    void moveItems(int from, int to);
    /*!
     * Returns \b true if \b f file is in play queue, else returns \b false
     */
    bool isQueued(PlayListItem* item) const;
    /*!
     * Sets current song to the file that is nex in queue, if queue is empty - does nothing
     */
    void setCurrentToQueued();
    /*!
     * Returns \b true if play queue is empty,otherwise returns - \b false.
     */
    bool isEmptyQueue()const;
    /*!
     * Returns index of \b f file in queue.e
     */
    int queuedIndex(PlayListItem* item) const
    {
        return m_queued_songs.indexOf(item);
    }
    /*!
     * Returns current selection(playlist can contain a lot of selections,
     * this method returns selection which \b row belongs to)
     */
    const SimpleSelection& getSelection(int row);
    /*!
     * Returns list with selected rows indexes.
     */
    QList<int> getSelectedRows() const;
    /*!
     * Returns list of \b PlayListItem pointers that are selected.
     */
    QList<PlayListItem*> getSelectedItems() const;
    /*!
     * Returns list of all \b PlayListItem pointers.
     */
    QList<PlayListItem*> items() const
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
     * Returns state of "Repeat All" option.
     */
    bool isRepeatableList() const;
    /*!
     * Returns state of "Shuffle" option.
     */
    bool isShuffle() const;
    /*!
     * Returns \b true if the playlist contains an item with URL \b url; otherwise returns \b false.
     */
    bool contains(const QString &url);
    /*!
     * Enum of available sort modes
     */
    enum SortMode
    {
        TITLE,             /*!< by title */
        ALBUM,             /*!< by album */
        ARTIST,            /*!< by artist */
        FILENAME,          /*!< by file name */
        PATH_AND_FILENAME, /*!< by path and file name */
        DATE,              /*!< by date */
        TRACK              /*!< by track */
    };

signals:
    /*!
     * Emitted when the state of PlayListModel has changed.
     */
    void listChanged();
    /*!
     * Emitted when current item has changed.
     */
    void currentChanged();
    /*!
     * Emitted when first item has added.
     */
    void firstAdded();
    /*!
     * Emitted when playlist name has chanded.
     * @param name New playlist name.
     */
    void nameChanged(const QString& name);

public slots:
    /*!
     * Adds \b item to the playlist.
     */
    void add(PlayListItem *item);
    /*!
     * Adds a list of items to the playlist.
     * @param items List of items
     */
    void add(QList <PlayListItem *> items);
    /*!
     * Removes all items.
     */
    void clear();
    /*!
     * Clears selection.
     */
    void clearSelection();
    /*!
     * Removes selected items.
     */
    void removeSelected();
    /*!
     * Removes unselected items.
     */
    void removeUnselected();
    /*!
     * Removes items with \b i index.
     */
    void removeAt (int i);
    /*!
     * Removes item \b item from playlist
     */
    void removeItem (PlayListItem *item);
    /*!
     * Inverts selection (selects unselected items and unselects selected items)
     */
    void invertSelection();
    /*!
     * Selects all items.
     */
    void selectAll();
    /*!
     * Shows details for the first selected item.
     */
    void showDetails();
    /*!
     * Emits update signals manually.
     */
    void doCurrentVisibleRequest();
    /*!
     * Adds file \b path to the playlist. File should be supported.
     */
    void addFile(const QString &path);
    /*!
     * Adds the list \b l of files to the model.
     */
    void addFiles(const QStringList& l);
    /*!
     * Adds \b dir to the model.
     */
    void addDirectory(const QString& dir);
    /*!
     * Removes previous items and loads list of files (regular files or directories),
     * returns \b true if at least one file has been successfully loaded,
     * otherwise returns \b false
     */
    bool setFileList(const QStringList &l);
    /*!
     * Loads list of files (regular files or directories)
     */
    void addFileList(const QStringList &l);
    /*!
     * Randomly changes items order.
     */
    void randomizeList();
    /*!
     * Reverces items order.
     */
    void reverseList();
    /*!
     * Prepares model for shuffle playing. \b yes parameter is \b true - model iterates in shuffle mode.
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
    /*!
     * Removes invalid items from playlist
     */
    void removeInvalidItems();
    /*!
     * Removes duplicate items by URL.
     */
    //void removeDuplicates();

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


private slots:
    void preparePlayState();

private:
    QList <PlayListItem*> m_items;
    QList <PlayListItem*> m_editing_items;
    PlayListItem* m_currentItem;
    int m_current;
    /*!
     * This flyweight object represents current selection.
     */
    SimpleSelection m_selection;
    /*!
     * Songs in play queue.
     */
    QList<PlayListItem*> m_queued_songs;
    /*!
     * Is playlist repeatable?
     */
    bool is_repeatable_list;
    /*!
     * Current playing state (Normal or Shuffle)
     */
    PlayState* m_play_state;
    int m_total_length;
    typedef QPointer<FileLoader> GuardedFileLoader;
    /*!
     * Vector of currently running file loaders.
     *  All loaders are automatically sheduled for deletion
     * when finished.
     */
    QVector<GuardedFileLoader> m_running_loaders;
    bool m_shuffle;
    QString m_name;
};


#endif
