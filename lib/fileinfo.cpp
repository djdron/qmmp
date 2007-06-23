/***************************************************************************
 *   Copyright (C) 2006 by for_kotov                                       *
 *   for_kotov@mail.ru                                                     *
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
#include "fileinfo.h"

FileInfo::FileInfo(const QString &path, const QString &name, const int &time)
{
    m_path = path;
    m_name = name;
    m_length = time;
}


FileInfo::~FileInfo()
{
   //qDebug("FileInfo deleted");
}

const QString FileInfo::path()
{
    return m_path;
}

const QString FileInfo::name()
{
    return m_name;
}

const int FileInfo::length()
{
    return m_length;
}
