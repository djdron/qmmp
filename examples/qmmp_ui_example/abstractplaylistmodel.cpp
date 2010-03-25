/***************************************************************************
 *   Copyright (C) 2009 by Ilya Kotov                                      *
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

#include <QFont>
#include <QChar>
#include <qmmpui/playlistitem.h>
#include "abstractplaylistmodel.h"

AbstractPlaylistModel::AbstractPlaylistModel(PlayListModel *pl, QObject *parent) :  QAbstractListModel(parent)
{
    m_pl = pl;
}

AbstractPlaylistModel::~AbstractPlaylistModel(){}

int AbstractPlaylistModel::columnCount (const QModelIndex &) const
{
    return 4;
}

QVariant AbstractPlaylistModel::data (const QModelIndex &index, int role) const
{
    if(role == Qt::DisplayRole && index.row () < m_pl->count())
    {
        PlayListItem *item = m_pl->item(index.row ());
        if(index.column() == 0)
            return QString::number(index.row () + 1);
        else if(index.column() == 1)
            return item->artist();
        else if(index.column() == 2)
            return item->title();
        else if(index.column() == 3)
            return formatTime(item->length());
        else
            return QVariant();
    }
    else if(role == Qt::FontRole)
    {
        QFont font;
        if(index.row () == m_pl->currentRow())
            font.setBold(TRUE);
        return font;
    }
    else
        return QVariant();
}

int AbstractPlaylistModel::rowCount(const QModelIndex &parent) const
{
    return m_pl->count();
}

QString AbstractPlaylistModel::formatTime(qint64 time) const
{
    return QString("%1:%2").arg(time/60).arg(time%60,2,10,QChar('0'));
}

QVariant AbstractPlaylistModel::headerData (int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
        switch(section)
        {
        case 0: return tr("#");
        case 1: return tr("Artist");
        case 2: return tr("Title");
        case 3: return tr("Length");
        default: return QVariant();
        }

    return QString::number(section);
}
