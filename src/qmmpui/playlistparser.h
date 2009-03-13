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
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef PLAYLISTPARSER_H
#define PLAYLISTPARSER_H

#include <QObject>

class PlaylistFormat;

/*! @brief The PlaylistParser class provides a simple api to access playlist format plugins.
 * @author Ilya Kotov <forkotov02@hotmail.ru>
 */
class PlaylistParser  : public QObject
{
    Q_OBJECT
public:
    /*!
    * Object constructor,
    * @param parent Parent object
    */
    PlaylistParser(QObject *parent);
    /*!
     * Destructor
     */
    ~PlaylistParser();
    /*!
     * Returns a list of supported file extensions.
     */
    QStringList getExtensions();
    /*!
     * Returns \b true if file \b filePath is supported, otherwise returns \b false
     */
    bool supports(const QString &filePath);
    /*!
     * Returns a list of the installed playlist formats.
     */
    QList<PlaylistFormat*> formats();
    /*!
     * Returns a pointer to the object's instance.
     */
    static PlaylistParser* instance();
    /*!
     * Finds playlist format by file path \b filePath
     * Returns \b 0 if file \b filePath is unsupported.
     */
    PlaylistFormat *findByPath(const QString &filePath);

private:
    void loadExternalPlaylistFormats();
    QList<PlaylistFormat*> m_formats;
    static PlaylistParser* m_instance;

};

#endif
