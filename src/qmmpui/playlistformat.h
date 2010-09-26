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

#ifndef _PALYLISTFORMAT_H
#define _PALYLISTFORMAT_H
#include <QStringList>

class PlayListItem;

/*! @brief Abstract interface for playlist formats.
 * @author Vladimir Kuznetsov <vovanec@gmail.com>
 */
class PlaylistFormat
{
public:
    /*!
     * Object destructor
     */
    virtual ~PlaylistFormat()
    {
        ;
    }
    /*!
     * Takes raw contents of playlist file, should return string list of
     * ready file pathes to fill the playlist.
     */
    virtual QStringList decode(const QString& contents) = 0;
    /*!
     * Takes the list of AbstractPlaylistItem objects, should return string of
     * encoded playlist file
     */
    virtual QString encode(const QList<PlayListItem*>& contents) = 0;
    /*!
     * Returns list of file extensions that current format supports
     */
    virtual QStringList getExtensions()const = 0;
    /*!
     * Verifies is the \b ext file extension supported by current playlist format.
     */
    virtual bool hasFormat(const QString& ext) = 0;
    /*!
     * Unique name of playlist format.
     */
    virtual QString name() const = 0;
};

Q_DECLARE_INTERFACE(PlaylistFormat,"PlaylistFormatInterface/1.0");

#endif
