/***************************************************************************
 *   Copyright (C) 2007-2008 by Ilya Kotov                                 *
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
#ifndef FILETAG_H
#define FILETAG_H

#include <QString>
#include <QMap>

/**
    @author Ilya Kotov <forkotov02@hotmail.ru>
*/
class FileTag
{
public:
    FileTag();
    FileTag(const FileTag &other);

    ~FileTag();

    enum Type
    {
        TITLE = 0,
        ARTIST,
        ALBUM,
        COMMENT,
        GENRE,
        YEAR,
        TRACK,
        LENGTH
    };

    void operator=(const FileTag &tag);
    void setValue(uint name, const QString &value);
    void setValue(uint name, const uint &value);
    const QString title () const;
    const QString artist () const;
    const QString album () const;
    const QString comment () const;
    const QString genre () const;
    const uint year () const;
    const uint track () const;
    const uint length () const;
    const bool isEmpty () const;

private:
    QMap <uint, QString> m_strValues;
    QMap <uint, uint> m_numValues;
};

#endif
