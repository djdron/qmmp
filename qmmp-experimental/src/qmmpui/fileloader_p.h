/***************************************************************************
 *   Copyright (C) 2006-2012 by Ilya Kotov                                 *
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
#ifndef FILELOADER_H
#define FILELOADER_H

#include <QObject>
#include <QDir>
#include <QQueue>
#include <QFileInfo>
#include <QThread>

class PlayListTrack;
class PlayListItem;
class QmmpUiSettings;

/*! @internal
 * @brief File loader class.
 *
 * This class represents fileloader object that
 * processes file list in separate thread and emits
 * \b newPlayListItem(PlayListItem*) signal for every newly
 * created media file.
 * @author Ilya Kotov <forkotov02@hotmail.ru>
 */
class FileLoader : public QThread
{
    Q_OBJECT
public:
    /*!
     * Constructs FileLoader object.
     * @param parent QObject parent
     */
    FileLoader(QObject *parent = 0);
    /*!
     * Object destructor.
     */
    ~FileLoader();
    /*!
     * Removes files and directories from queue and waits until thread is finished
     */
    void finish();
    /*!
     * Sets file/directory to load
     */
    void add(const QString &path);
    /*!
     * Sets files/directories to load
     */
    void add(const QStringList &paths);

    void insert(PlayListItem *before, const QString &path);
    void insert(PlayListItem *before, const QStringList &paths);

signals:
    /*!
     * Emitted when new playlist track is available.
     * @param item Pointer of the new PlayListTrack object.
     */
    void newTrackToAdd(PlayListTrack *track);
    void newTrackToInsert(PlayListItem *before, PlayListTrack *track);

private:
    virtual void run();
    void addFile(const QString &path, PlayListItem *before = 0);
    void addDirectory(const QString &s, PlayListItem *before = 0);
    bool checkRestrictFilters(const QFileInfo &info);
    bool checkExcludeFilters(const QFileInfo &info);
    struct InsertItem
    {
        PlayListItem *before;
        QString path;

    };
    QQueue <QString> m_paths;
    QQueue <InsertItem> m_insertItems;
    QStringList m_filters;
    QmmpUiSettings *m_settings;
    bool m_finished;
};

#endif
