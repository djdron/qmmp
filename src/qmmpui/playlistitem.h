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
#ifndef PLAYLISTITEM_H
#define PLAYLISTITEM_H

#include <qmmp/qmmp.h>
#include <qmmpui/abstractplaylistitem.h>

class FileInfo;
class QSettings;

/** @brief The PlayListItem class provides an item for use with the PlayListModel class.
 * @author Ilya Kotov <forkotov02@hotmail.ru>
 */
class PlayListItem : public AbstractPlaylistItem
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
    PlayListItem();
    /*!
     * Constructs plalist item with given metadata.
     * @param info Media file information.
     */
    PlayListItem(FileInfo *info);
    /*!
     * Object destructor.
     */
    ~PlayListItem();
    /*!
     * Sets item selection flag to \b select
     * @param select State of selection (\b true select, \b false unselect)
     */
    void setSelected(bool select);
    /*!
     * Return \b true if item is selected, otherwise returns \b false.
     */
    bool isSelected() const;
    /*!
     * It is used by PlayListModel class.
     */
    void setCurrent(bool yes);
    /*!
     * Returns \b true if the item is the current item; otherwise returns returns \b false.
     */
    bool isCurrent() const;
    /*!
     * Returns current state of the playlist item.
     */
    FLAGS flag() const;
    /*!
     * Sets state of the playlist item.
     */
    void setFlag(FLAGS);
    /*!
     * Returns item short title.
     */
    const QString text();
    /*!
     * Direct access to the item short title.
     * @param title New short title.
     */
    void setText(const QString &title);
    /*!
     * Updates current metadata.
     * @param metaData Map with metadata values.
     */
    void updateMetaData(const QMap <Qmmp::MetaData, QString> &metaData);
    /*!
     * Gets new metadata from file (works for local files only).
     */
    void updateTags();

private:
    void readMetadata();
    QString m_title;
    FileInfo *m_info;
    bool m_selected;
    bool m_current;
    FLAGS m_flag;
};

#endif
