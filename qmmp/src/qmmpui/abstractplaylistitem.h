/***************************************************************************
 *   Copyright (C) 2008-2009 by Ilya Kotov                                 *
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
#ifndef ABSTRACTPLAYLISTITEM_H
#define ABSTRACTPLAYLISTITEM_H

#include <QMap>
#include <QString>
#include <qmmp/qmmp.h>

/** @brief The AbstractPlaylistItem class provides the basic functionality for the playlist items.
 * @author Ilya Kotov <forkotov02@hotmail.ru>
 */
class AbstractPlaylistItem
{
public:
    /*!
     * Constructs empty AbstractPlaylistItem object.
     */
    AbstractPlaylistItem();
    /*!
     * Object destructor
     */
    ~AbstractPlaylistItem();
    /*!
     * Returns song title.
     */
    const QString title () const;
    /*!
     * Returns song artist.
     */
    const QString artist () const;
    /*!
     * Returns song album.
     */
    const QString album () const;
    /*!
     * Returns comment.
     */
    const QString comment () const;
    /*!
     * Returns gnere.
     */
    const QString genre () const;
    /*!
     * Returns track number.
     */
    const QString track () const;
    /*!
     * Returns year.
     */
    const QString year () const;
    /*!
     * Returns stream url or local file path.
     */
    const QString url () const;
    /*!
     * Returns song length in seconds.
     */
    qint64 length ();
    /*!
     * Returns \b true if item has no metadata.
     */
    bool isEmpty();
    /*!
    * Removes all metadata from item.
    */
    void clear();
    /*!
     * Loads metadata.
     * @param metaData A map with metadata.
     */
    virtual void setMetaData(const QMap <Qmmp::MetaData, QString> &metaData);
    /*!
     * Loads one metadata value.
     * @param key Metadata key.
     * @param key Metadata vlaue.
     */
    virtual void setMetaData(Qmmp::MetaData key, const QString &value);
    /*!
     * Sets length in seconds.
     */
    virtual void setLength(qint64 length);

private:
    QMap <Qmmp::MetaData, QString> m_metaData;
    qint64 m_length;
};

#endif
