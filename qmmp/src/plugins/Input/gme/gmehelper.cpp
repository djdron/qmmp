/***************************************************************************
 *   Copyright (C) 2010-2011 by Ilya Kotov                                 *
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

#include "gmehelper.h"

#define FADE_LENGTH 8000

GmeHelper::GmeHelper()
{
     m_emu = 0;
}

GmeHelper::~GmeHelper()
{
    if(m_emu)
        gme_delete(m_emu);
    m_emu = 0;
}

Music_Emu *GmeHelper::load(const QString &url, int sample_rate)
{
    if(m_emu)
        gme_delete(m_emu);
    m_emu = 0;
    QString path = url;
    if(url.contains("://"))
    {
        path = QUrl(url).path();
        path.replace(QString(QUrl::toPercentEncoding("#")), "#");
        path.replace(QString(QUrl::toPercentEncoding("?")), "?");
        path.replace(QString(QUrl::toPercentEncoding("%")), "%");
        path.replace(QString(QUrl::toPercentEncoding(":")), ":");
    }
    const char *err = 0;
    gme_type_t file_type;
    if((err = gme_identify_file(qPrintable(path),&file_type)))
    {
        qWarning("GmeHelper: %s", err);
        return 0;
    }
    if(!file_type)
    {
        qWarning("DecoderGme: unsupporetd music type");
        return 0;
    }
    if(!(m_emu = gme_new_emu(file_type, sample_rate)))
    {
        qWarning("GmeHelper: out of memory");
        return 0;
    }
    if((err = gme_load_file(m_emu, qPrintable(path))))
    {
        qWarning("GmeHelper: %s", err);
        return 0;
    }
    QString m3u_path = path.left(path.lastIndexOf("."));
    m3u_path.append(".m3u");
    gme_load_m3u(m_emu, qPrintable(m3u_path));
    m_path = path;
    return m_emu;
}

QList <FileInfo*> GmeHelper::createPlayList(bool meta)
{
    QList <FileInfo*> list;
    if(!m_emu)
        return list;
    int count = gme_track_count(m_emu);
    gme_info_t *track_info;
    for(int i = 0; i < count; ++i)
    {
        FileInfo *info = new FileInfo();
        if(!gme_track_info(m_emu, &track_info, i))
        {
            if(track_info->length <= 0)
                track_info->length = track_info->intro_length + track_info->loop_length * 2;
        }
        if(track_info->length <= 0)
            track_info->length = (long) (2.5 * 60 * 1000);
        if(track_info->length < FADE_LENGTH)
            track_info->length += FADE_LENGTH;
        if(meta)
        {
            info->setMetaData(Qmmp::TITLE, track_info->song);
            info->setMetaData(Qmmp::ARTIST, track_info->author);
            info->setMetaData(Qmmp::COMMENT, track_info->comment);
            info->setMetaData(Qmmp::TRACK, i+1);
        }
        QString path = m_path;
        path.replace("%", QString(QUrl::toPercentEncoding("%"))); //replace special symbols
        path.replace("#", QString(QUrl::toPercentEncoding("#")));
        path.replace("?", QString(QUrl::toPercentEncoding("?")));
        path.replace(":", QString(QUrl::toPercentEncoding(":")));
        info->setPath("gme://"+path+QString("#%1").arg(i+1));
        info->setLength(track_info->length/1000);
        gme_free_info(track_info);
        list << info;
    }
    return list;
}

int GmeHelper::fadeLength()
{
    return FADE_LENGTH;
}
