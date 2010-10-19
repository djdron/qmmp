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

#include "tagmodel.h"

TagModel::TagModel(int f)
{
    m_f = f;
}

TagModel::~TagModel()
{}

void TagModel::setValue(Qmmp::MetaData key, int value)
{
    setValue(key, QString::number(value));
}

QList<Qmmp::MetaData> TagModel::keys()
{
    QList<Qmmp::MetaData> list;
    list << Qmmp::TITLE;
    list << Qmmp::ARTIST;
    list << Qmmp::ALBUM;
    list << Qmmp::COMMENT;
    list << Qmmp::GENRE;
    list << Qmmp::COMPOSER;
    list << Qmmp::YEAR;
    list << Qmmp::TRACK;
    list << Qmmp::DISCNUMBER;
    return list;
}

int TagModel::caps()
{
    return m_f;
}

bool TagModel::exists()
{
    return true;
}

void TagModel::create()
{}

void TagModel::remove()
{}

void TagModel::save()
{}
