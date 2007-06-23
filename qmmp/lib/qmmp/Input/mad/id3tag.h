/***************************************************************************
 *   Copyright (C) 2006 by Ilya Kotov                                      *
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
#ifndef ID3TAG_H
#define ID3TAG_H

#include <QString>
#include <filetag.h>
#include <taglib/tag.h>
#include <taglib/fileref.h>
#include <taglib/id3v1tag.h>
#include <taglib/id3v2tag.h>
#include <taglib/tfile.h>
#include <taglib/mpegfile.h>

#include <filetag.h>

/**
	@author Ilya Kotov <forkotov02@hotmail.ru>
*/
class QTextCodec;

class ID3Tag : public FileTag
{
public:
    ID3Tag(const QString &source);

    ~ID3Tag();

    uint length();
    uint track();
    uint year();
    QString album();
    QString artist();
    QString comment();
    QString genre();
    QString title();
    bool isEmpty ();

private:
    TagLib::Tag *m_tag;
    QString m_album;
    QString m_artist;
    QString m_comment;
    QString m_genre;
    QString m_title;
    uint m_length;
    uint m_year;
    uint m_track;
    bool m_empty;

};

#endif
