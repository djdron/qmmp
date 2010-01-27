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

#ifndef METADATAFORMATTER_H
#define METADATAFORMATTER_H

#include <QString>
#include <QMap>
#include <qmmpui/abstractplaylistitem.h>
#include <qmmp/qmmp.h>

/*!
 * @author Ilya Kotov <forkotov02@hotmail.ru>
 */
class MetaDataFormatter
{
public:
    MetaDataFormatter(const QString &format);
    QString parse(AbstractPlaylistItem *item);
    QString parse(const QMap<Qmmp::MetaData, QString> metaData, qint64 length = 0);

private:
    QString m_format;
    QString processIfKeyWord(QString title);
};

#endif // METADATAFORMATTER_H
