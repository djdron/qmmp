/***************************************************************************
 *   Copyright (C) 2013-2014 by Ilya Kotov                                 *
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
#ifndef PLAYLISTTRACK_H
#define PLAYLISTTRACK_H

#include <QMap>
#include <qmmp/fileinfo.h>
#include <qmmp/qmmp.h>
#include "playlistitem.h"

class QmmpUiSettings;

/** @brief The PlayListTrack class provides a track for use with the PlayListModel class.
 * @author Ilya Kotov <forkotov02@hotmail.ru>
 */
class PlayListTrack : public QMap <Qmmp::MetaData, QString>, public PlayListItem
{
public:
    /*!
     * Current state of playlist item.
     * FREE - instance is free and may be deleted
     * EDITING - instance is currently busy in some kind of operation(tags editing etc.)
     * and can't be deleted at the moment. Set flag SCHEDULED_FOR_DELETION for it
     * instead of delete operator call.
     */
    enum FLAGS
    {
        FREE = 0,              /*!< instance is free and may be deleted */
        EDITING,               /*!< instance is currently busy */
        SCHEDULED_FOR_DELETION /*!< instance is sheduled for deletion */
    };
    /*!
     * Constructs an empty plalist item.
     */
    PlayListTrack();
    /*!
     * Constructs a new PlayListTrack that is a copy of the given \b item
     */
    PlayListTrack(const PlayListTrack &item);
    /*!
     * Constructs plalist item with given metadata.
     * @param info Media file information.
     */
    PlayListTrack(FileInfo *info);
    /*!
     * Object destructor.
     */
    virtual ~PlayListTrack();
    /*!
     * Returns formatted title of the item.
     */
    const QString formattedTitle();
    /*!
     *  Returns formatted length of the item.
     */
    const QString formattedLength();
    /*!
     * Returns song length in seconds.
     */
    qint64 length() const;
    /*!
     * Sets length in seconds.
     */
    void setLength(qint64 length);
    /*!
     * Same as url()
     */
    const QString url() const;
    /*!
     * Updates current metadata.
     * @param metaData Map with metadata values.
     */
    void updateMetaData(const QMap <Qmmp::MetaData, QString> &metaData);
    /*!
     * Gets new metadata from file (works for local files only).
     */
    void updateMetaData();
    /*!
     * Returns name of the parent group.
     */
    const QString groupName();
    /*!
     * Returns \b false.
     */
    bool isGroup() const;
    /*!
     * Returns current state of the playlist item.
     */
    FLAGS flag() const;
    /*!
     * Sets state of the playlist item.
     */
    void setFlag(FLAGS);

private:
    void formatTitle();
    void formatGroup();
    QString m_formattedTitle;
    QString m_formattedLength;
    QString m_group;
    QString m_titleFormat;
    QString m_groupFormat;
    QmmpUiSettings *m_settings;
    qint64 m_length;
    FLAGS m_flag;
};

#endif
