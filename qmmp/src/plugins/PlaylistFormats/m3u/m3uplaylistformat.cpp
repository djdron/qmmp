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

#include <QFileInfo>
#include <QtPlugin>
#include "m3uplaylistformat.h"

bool M3UPlaylistFormat::hasFormat(const QString &f)
{
    return m_supported_formats.contains(f);
}

QStringList M3UPlaylistFormat::getExtensions() const
{
    return m_supported_formats;
}

M3UPlaylistFormat::M3UPlaylistFormat()
{
    m_supported_formats << "m3u";
}

QStringList M3UPlaylistFormat::decode(const QString & contents)
{
    QStringList out;
    QStringList splitted = contents.split("\n");
    if(splitted.isEmpty())
        return QStringList();

    foreach(QString str, splitted)
    {
        str = str.trimmed ();
        if (str.startsWith("#EXTM3U") || str.startsWith("#EXTINF:") || str.isEmpty())
            continue;//TODO: Let's skip it for now..
        else if (str.startsWith("#") || str.isEmpty())
            continue;
        else
            out << str;
    }
    return out;
}

QString M3UPlaylistFormat::encode(const QList<PlayListItem*> & contents)
{
    QStringList out;
    out << QString("#EXTM3U");
    foreach(PlayListItem* f,contents)
    {
        QString info = "#EXTINF:" + QString::number(f->length()) + "," + f->value(Qmmp::TITLE);
        out.append(info);
        out.append(f->url());
    }
    return out.join("\n");
}

QString M3UPlaylistFormat::name() const
{
    return "M3UPlaylistFormat";
}

Q_EXPORT_PLUGIN2(m3uplaylistformat,M3UPlaylistFormat)
