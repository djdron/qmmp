/***************************************************************************
 *   Copyright (C) 2008 by Ilya Kotov                                      *
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
#ifndef PLAYLISTPARSER_H
#define PLAYLISTPARSER_H

#include <QObject>
#include "playlistformat.h"

/*! @brief The PlaylistParser class provides a simple api to access playlist format plugins.
 * @author Ilya Kotov <forkotov02@hotmail.ru>
 */
class PlayListParser  : public QObject
{
    Q_OBJECT
public:
    /*!
    * Object constructor,
    * @param parent Parent object
    */
    PlayListParser(QObject *parent);
    /*!
     * Destructor
     */
    ~PlayListParser();
    /*!
     * Returns a list of supported file extensions.
     */
    QStringList extensions();
    /*!
     * Returns \b true if file \b filePath is supported, otherwise returns \b false
     */
    bool supports(const QString &filePath);
    /*!
     * Returns a list of the installed playlist formats.
     */
    QList<PlayListFormat*> formats();
    /*!
     * Returns a pointer to the object's instance.
     */
    static PlayListParser* instance();
    /*!
     * Finds playlist format by file path \b filePath
     * Returns \b 0 if file \b filePath is unsupported.
     */
    PlayListFormat *findByPath(const QString &filePath);

private:
    void loadExternalPlaylistFormats();
    QList<PlayListFormat*> m_formats;
    static PlayListParser* m_instance;

};

#endif
