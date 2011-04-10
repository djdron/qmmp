/***************************************************************************
 *   Copyright (C) 2006-2011 by Ilya Kotov                                 *
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
#ifndef FILELOADER_H
#define FILELOADER_H

#include <QObject>
#include <QDir>
#include <QQueue>
#include <QThread>

class PlayListItem;

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
     * Sets files to load
     */
    void finish();
    /*!
     * Sets file to load
     */
    void loadFile(const QString &path);
    /*!
     * Sets files to load
     */
    void loadFiles(const QStringList &paths);
    /*!
     * Sets directory to load
     */
    void loadDirectory(const QString &path);

signals:
    /*!
     * Emitted when new playlist item is available.
     * @param item Pointer of the new PlayListItem object.
     */
    void newPlayListItem(PlayListItem *item);

protected:
    virtual void run();
    void addFile(const QString &path);
    void addDirectory(const QString &s);

private:
    QQueue <QString> m_files;
    QQueue <QString> m_directories;
    QStringList m_filters;
    bool m_finished;
};

#endif
