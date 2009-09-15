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
#include "streamreader.h"
#include "inputsource.h"

InputSource::InputSource(const QString &source, QObject *parent) : QObject(parent)
{
    m_device = 0;
    m_isValid = FALSE;
    m_url = source;
    QUrl url;
    if (source.contains("://")) //url
        url = source;
    else if (QFile::exists(source))
    {
        url = QUrl::fromLocalFile(source);
    }
    else
    {
        qDebug("InputSource: file doesn't exist");
        return;
    }
    m_url = url;
    if (url.scheme() == "file")
    {
        m_device = new QFile(source, this);
        /*if (!m_device->open(QIODevice::ReadOnly))
        {
            qDebug("InputSource: cannot open input");
            return FALSE;
        }*/
    }
    if (url.scheme() == "http")
    {
        m_device = new StreamReader(source, this);
        //connect(m_input, SIGNAL(bufferingProgress(int)), SIGNAL(bufferingProgress(int)));
        connect(m_device, SIGNAL(readyRead()),SIGNAL(readyRead()));
        qobject_cast<StreamReader *>(m_device)->downloadFile();
        return;
    }
    m_isValid = FALSE;
}

const QUrl InputSource::url()
{
    return m_url;
}

QIODevice *InputSource::ioDevice()
{
    return m_device;
}

