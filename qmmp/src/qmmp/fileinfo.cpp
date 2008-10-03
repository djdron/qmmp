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
#include "fileinfo.h"

FileInfo::FileInfo()
{
    m_length = 0;
    m_count = 1;
}

FileInfo::FileInfo(const FileInfo &other)
{
    *this = other;
}

FileInfo::~FileInfo()
{}

void FileInfo::operator=(const FileInfo &info)
{
    setLength(info.length());
    setMetaData(info.metaData());
    setUrl(info.url());
}

bool FileInfo::operator==(const FileInfo &info)
{
    return metaData () == info.metaData () &&
           length () == info.length ();
    url() == info.url();
}

bool FileInfo::operator!=(const FileInfo &info)
{
    return !operator==(info);
}

const qint64 FileInfo::length () const
{
    return m_length;
}

const QString FileInfo::metaData (Qmmp::MetaData key) const
{
    return m_metaData[key];
}

const QMap<Qmmp::MetaData, QString>  FileInfo::metaData () const
{
    return m_metaData;
}

void FileInfo::setMetaData(const QMap<Qmmp::MetaData,  QString> &metaData)
{
    m_metaData = metaData;
}

bool FileInfo::isEmpty() const
{
    return m_metaData.isEmpty(); //TODO add correct test
}

const QUrl FileInfo::url() const
{
    return m_url;
}

void FileInfo::setLength(qint64 length)
{
    m_length = length;
}

void FileInfo::setMetaData(Qmmp::MetaData key, const QString &value)
{
    m_metaData.insert(key, value);
}

void FileInfo::setMetaData(Qmmp::MetaData key, int value)
{
    m_metaData.insert(key, QString::number(value));
}

void FileInfo::setUrl(const QUrl &url)
{
    m_url = url;
}
