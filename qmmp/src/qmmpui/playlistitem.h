/***************************************************************************
 *   Copyright (C) 2008-2013 by Ilya Kotov                                 *
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
#ifndef PLAYLISTITEM_H
#define PLAYLISTITEM_H

#include <QMap>
#include <qmmp/fileinfo.h>
#include <qmmp/qmmp.h>


/** @brief The PlayListItem class provides an item for use with the PlayListModel class.
 * @author Ilya Kotov <forkotov02@hotmail.ru>
 */
class PlayListItem
{
public:
    /*!
     * Constructs an empty plalist item.
     */
    PlayListItem();
    /*!
     * Object destructor.
     */
    virtual ~PlayListItem();
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
     * Returns formatted title of the item.
     * @param column Number of column.
     */
    virtual const QString formattedTitle(int column) = 0;

    virtual const QStringList formattedTitles() = 0;
    /*!
     *  Returns formatted length of the item.
     */
    virtual const QString formattedLength() = 0;
    /*!
     * Returns \b true if the \b PlayListItem is group separator. Otherwise returns \b false.
     */
    virtual bool isGroup() const = 0;

    virtual void setTrackIndex(int number);

    virtual int trackIndex() const;

private:
    bool m_selected;
};

#endif
