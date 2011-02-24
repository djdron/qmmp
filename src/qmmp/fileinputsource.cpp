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

#include <QFile>
#include "fileinputsource.h"

FileInputSource::FileInputSource(const QString &url, QObject *parent) : InputSource(url,parent)
{
    m_file = new QFile(url, this);
}

QIODevice *FileInputSource::ioDevice()
{
    return m_file;
}

bool FileInputSource::initialize()
{
    bool ok = m_file->open(QIODevice::ReadOnly);
    if(ok)
        emit ready();
    else
        qWarning("FileInputSource: error: %s", qPrintable(m_file->errorString()));
    return ok;
}

bool FileInputSource::isReady()
{
    return m_file->isOpen();
}
