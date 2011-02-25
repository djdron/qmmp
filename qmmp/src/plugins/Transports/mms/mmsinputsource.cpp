/***************************************************************************
 *   Copyright (C) 2009-2011 by Ilya Kotov                                 *
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

#include "mmsstreamreader.h"
#include "mmsinputsource.h"

MMSInputSource::MMSInputSource(const QString &url, QObject *parent) : InputSource(url,parent)
{
    m_reader = new MMSStreamReader(url, this);
    connect(m_reader, SIGNAL(ready()),SLOT(activate()));
}

QIODevice *MMSInputSource::ioDevice()
{
    return m_reader;
}

bool MMSInputSource::initialize()
{
    m_reader->downloadFile();
    return true;
}

bool MMSInputSource::isReady()
{
    return m_reader->isOpen();
}

QString  MMSInputSource::contentType() const
{
    return "audio/x-ms-wma";
}

void MMSInputSource::activate()
{
    m_reader->open(QIODevice::ReadOnly);
    emit ready();
}
