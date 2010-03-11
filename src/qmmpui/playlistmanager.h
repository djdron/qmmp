/***************************************************************************
 *   Copyright (C) 2009-2010 by Ilya Kotov                                 *
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
#ifndef PLAYLISTMANAGER_H
#define PLAYLISTMANAGER_H

#include <QObject>
#include "playlistmodel.h"

/*!
 * @author Ilya Kotov <forkotov02@hotmail.ru>
 */
class PlayListManager : public QObject
{
Q_OBJECT
public:
    PlayListManager(QObject *parent);
    ~PlayListManager();

    QList <PlayListModel *> playLists() const;
    QStringList playListNames() const;
    PlayListModel *selectedPlayList() const;
    PlayListModel *currentPlayList() const;
    int count();
    int indexOf(PlayListModel *model);
    PlayListModel *playListAt(int i);
    /*!
     * Returns state of the "Convert underscores to blanks" option (\b true - enabled, \b false - disabled).
     */
    bool convertUnderscore();
    /*!
     * Returns state of the "Convert %20 to blanks" option (\b true - enabled, \b false - disabled).
     */
    bool convertTwenty();
    /*!
     * Returns the state of metadata usage (\b true - use, \b false - not use).
     */
    bool useMetadata();
    /*!
     * Returns title format string.
     */
    const QString format() const;
    /*!
     * Sets the "Convert underscores to blanks" option state to \b enabled
     * @param enabled Option state (\b true - enabled, \b false - disabled)
     */
    void setConvertUnderscore(bool enabled);
    /*!
     * Sets the "Convert %20 to blanks" option state to \b enabled
     * @param enabled Option state (\b true - enabled, \b false - disabled)
     */
    void setConvertTwenty(bool enabled);
    /*!
     * Sets metadata usage option state to \b enabled
     * @param enabled Option state (\b true - enabled, \b false - disabled)
     */
    void setUseMetadata(bool enabled);
    /*!
     * Sets short title format
     * @param format title format. (Expressions: "%p" - artist, "%a" - album, "%t" - title, "%n" - track,
     * "%g" - genre, "%c" - comment, "%C" - composer, "%D" - disc number "%f" - file name, "
     * %F" - full path, "%y" - year)
     */
    void setFormat(const QString &format);
    /*!
     * Returns state of "Repeat All" option.
     */
    bool isRepeatableList() const;
    /*!
     * Returns state of "Shuffle" option.
     */
    bool isShuffle() const;

signals:
    void currentPlayListChanged (PlayListModel *current, PlayListModel *previous);
    void selectedPlayListChanged (PlayListModel *selected, PlayListModel *previous);
    void playListAdded(int index);
    void playListRemoved(int index);
    void playListsChanged();
    /*!
     * Emitted when state of the "Repeat All" option has changed.
     * @param state New state of the "Repeat All" option (\b true - enabled, \b false disabled)
     */
    void repeatableListChanged(bool state);
    /*!
     * Emitted when state of the "Shuffle" option has changed.
     * @param state New state of the "Shuffle" option (\b true - enabled, \b false disabled)
     */
    void shuffleChanged(bool state);
    /*!
     * Emitted when other settings (format, metadata, etc) have changed.
     */
    void settingsChanged();

public slots:
    void selectPlayList(PlayListModel *model);
    void selectPlayList(int index);
    void selectNextPlayList();
    void selectPreviousPlayList();
    void activatePlayList(PlayListModel *model);
    PlayListModel *createPlayList(const QString &name = QString());
    void removePlayList(PlayListModel *model);
    void move(int i, int j);
    void setRepeatableList(bool r);
    void setShuffle(bool s);
    /*!
     * This is a convenience function and is the same as calling \b selectedPlayList()->clear()
     */
    void clear();
    /*!
     * This is a convenience function and is the same as calling \b selectedPlayList()->clearSelection()
     */
    void clearSelection();
    /*!
     * This is a convenience function and is the same as calling \b selectedPlayList()->removeSelected()
     */
    void removeSelected();
    /*!
     * This is a convenience function and is the same as calling \b selectedPlayList()->removeUnselected()
     */
    void removeUnselected();
    /*!
     * This is a convenience function and is the same as calling \b selectedPlayList()->removeAt(i)
     */
    void removeAt (int i);
    /*!
     * This is a convenience function and is the same as calling \b selectedPlayList()->removeItem(item)
     */
    void removeItem (PlayListItem *item);
    /*!
     * This is a convenience function and is the same as calling \b selectedPlayList()->invertSelection()
     */
    void invertSelection();
    /*!
     * This is a convenience function and is the same as calling \b selectedPlayList()->selectAll()
     */
    void selectAll();
    /*!
     * This is a convenience function and is the same as calling \b selectedPlayList()->showDetails()
     */
    void showDetails();
    /*!
     * This is a convenience function and is the same as calling \b selectedPlayList()->addFile(path)
     */
    void addFile(const QString &path);
    /*!
     * This is a convenience function and is the same as calling \b selectedPlayList()->addFiles(l)
     */
    void addFiles(const QStringList& l);
    /*!
     * This is a convenience function and is the same as calling \b selectedPlayList()->addDirectory(dir)
     */
    void addDirectory(const QString& dir);
    /*!
     * This is a convenience function and is the same as calling \b selectedPlayList()->randomizeList()
     */
    void randomizeList();
    /*!
     * This is a convenience function and is the same as calling \b selectedPlayList()->reverseList()
     */
    void reverseList();
    /*!
     * This is a convenience function and is the same as calling \b selectedPlayList()->sortSelection(mode)
     */
    void sortSelection(int mode);
    /*!
     * This is a convenience function and is the same as calling \b selectedPlayList()->sort(mode)
     */
    void sort(int mode);
    /*!
     * This is a convenience function and is the same as calling \b selectedPlayList()->addToQueue()
     */
    void addToQueue();
    /*!
     * This is a convenience function and is the same as calling \b selectedPlayList()->clearInvalidItems()
     */
    void removeInvalidItems();
    /*!
     * This is a convenience function and is the same as calling \b selectedPlayList()->removeDuplicates()
     */
    void removeDuplicates();


private:
    void readPlayLists();
    void writePlayLists();
    QList <PlayListModel *> m_models;
    PlayListModel *m_current;
    PlayListModel *m_selected;
    bool m_repeatable, m_shuffle;
};

#endif // PLAYLISTMANAGER_H
