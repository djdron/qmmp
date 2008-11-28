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
#ifndef ABSTRACTPLAYLISTITEM_H
#define ABSTRACTPLAYLISTITEM_H

#include <QMap>
#include <QString>
#include <qmmp/qmmp.h>

/**
    @author Ilya Kotov <forkotov02@hotmail.ru>
*/
class AbstractPlaylistItem
{
public:
    AbstractPlaylistItem();

    ~AbstractPlaylistItem();

    const QString title () const;
    const QString artist () const;
    const QString album () const;
    const QString comment () const;
    const QString genre () const;
    const QString track () const;
    const QString year () const;
    const QString url () const;

    qint64 length ();
    bool isEmpty();
    void clear();

    virtual void setMetaData(const QMap <Qmmp::MetaData, QString> &metaData);
    virtual void setMetaData(Qmmp::MetaData key, const QString &value);
    virtual void setLength(qint64 length);

private:
    QMap <Qmmp::MetaData, QString> m_metaData;
    qint64 m_length;
};

#endif
