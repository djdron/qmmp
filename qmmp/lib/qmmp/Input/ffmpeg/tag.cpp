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
#include "tag.h"

Tag::Tag(const QString &source)
        : FileTag()
{
    m_length = 0;
    m_year = 0;
    m_track = 0;
    m_empty = TRUE;

    avcodec_init();
    avcodec_register_all();
    av_register_all();

    if (av_open_input_file(&m_in, source.toLocal8Bit(), NULL,0, NULL) < 0)
        return;
    av_find_stream_info(m_in);
    m_album = QString::fromUtf8(m_in->album).trimmed();
    m_artist = QString::fromUtf8(m_in->author).trimmed();
    m_comment = QString::fromUtf8(m_in->comment).trimmed();
    m_genre = QString::fromUtf8(m_in->genre).trimmed();
    m_title = QString::fromUtf8(m_in->title).trimmed();
    m_year = m_in->year;
    m_track = m_in->track;
    m_length = int(m_in->duration/AV_TIME_BASE);
    m_empty = FALSE;

    av_close_input_file(m_in);
}


Tag::~Tag()
{}


bool Tag::isEmpty()
{
    return m_empty;
}

QString Tag::album()
{
    return m_album;
}

QString Tag::artist()
{
    return m_artist;
}

QString Tag::comment()
{
    return m_comment;
}

QString Tag::genre()
{
    return m_genre;
}

QString Tag::title()
{
    return m_title;
}

uint Tag::length()
{
    return m_length;
}

uint Tag::track()
{
    return m_track;
}

uint Tag::year()
{
    return m_year;
}
