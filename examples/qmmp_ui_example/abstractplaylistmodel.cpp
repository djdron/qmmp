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

AbstractPlaylistModel::AbstractPlaylistModel(PlayListModel *pl, QObject *parent) : QAbstractTableModel(parent)
{
    m_pl = pl;
}

AbstractPlaylistModel::~AbstractPlaylistModel(){}

int AbstractPlaylistModel::columnCount (const QModelIndex &) const
{
    return 2;
}

QVariant AbstractPlaylistModel::data (const QModelIndex &index, int role) const
{
    if(role == Qt::DisplayRole && index.row () < m_pl->count())
    {
        PlayListItem *item = m_pl->item(index.row ());
        return QString("%1. %2 |%3").arg(index.row () + 1).arg(item->text()).arg(formatTime(item->length()));
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

/*QModelIndex AbstractPlaylistModel::index(int row, int column, const QModelIndex &parent) const
{
    return QModelIndex();
}*/

/*QModelIndex AbstractPlaylistModel::parent(const QModelIndex &child) const
{
    return QModelIndex();
}*/

int AbstractPlaylistModel::rowCount(const QModelIndex &parent) const
{
    return m_pl->count();
}

QString AbstractPlaylistModel::formatTime(qint64 time) const
{
    return QString("%1:%2").arg(time/60).arg(time%60,2,10,QChar('0'));
}
