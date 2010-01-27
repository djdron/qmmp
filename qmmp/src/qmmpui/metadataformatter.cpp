/***************************************************************************
 *   Copyright (C) 2009-2010 by Ilya Kotov                                 *
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


/*
Syntax:
%p - artist
%a - album
%t - title
%n - track,
%NN - 2-digit track
%g - genre
%c - comment
%C - composer
%D - disc number
%f - file name
%F - full path
%y - year
%l - duration
%if(A,B,C)
%if(A&B&C,D,E)
*/

#include <QStringList>
#include <QUrl>
#include "metadataformatter.h"

MetaDataFormatter::MetaDataFormatter(const QString &format)
{
    m_format = format;
}

QString MetaDataFormatter::parse(AbstractPlaylistItem *item)
{
    return parse(item->metaData(), item->length());
}

QString MetaDataFormatter::parse(const QMap<Qmmp::MetaData, QString> metaData, qint64 length)
{
    QString title = m_format;
    title.replace("\\(", "%28");
    title.replace("\\)", "%29");
    title.replace(")", "%)");
    title.replace("&", "%&");
    title.replace(",", "%,");
    title.replace("%p", metaData[Qmmp::ARTIST]);
    title.replace("%a", metaData[Qmmp::ALBUM]);
    title.replace("%t", metaData[Qmmp::TITLE]);
    title.replace("%n", metaData[Qmmp::TRACK]);
    title.replace("%NN", QString("%1").arg(metaData[Qmmp::TRACK],2,'0'));
    title.replace("%g", metaData[Qmmp::GENRE]);
    title.replace("%c", metaData[Qmmp::COMMENT]);
    title.replace("%C", metaData[Qmmp::COMPOSER]);
    title.replace("%D", metaData[Qmmp::DISCNUMBER]);
    title.replace("%f", metaData[Qmmp::URL].section('/',-1));
    title.replace("%F", metaData[Qmmp::URL]);
    title.replace("%y", metaData[Qmmp::YEAR]);
    if(title.contains("l"))
    {
        if (length)
        {
            QString time;
            int l = length;
            if(l > 3600)
                time == QString("%1:%2:%3").arg(l/3600,2,10,QChar('0'))
                .arg(l%3600/60,2,10,QChar('0')).arg(l%60,2,10,QChar('0'));
            else
                time = QString("%1:%2").arg(l/60,2,10,QChar('0')).arg(l%60,2,10,QChar('0'));
            title.replace("%l",time);
        }
        else
            title.replace("%l","");
    }

    if(title.contains("%if"))
        title = processIfKeyWord(title);
    title.replace("%28", "(");
    title.replace("%29", ")");
    return title;
}

QString MetaDataFormatter::processIfKeyWord(QString title)
{
    int pos = title.indexOf("%if(");
    int size = title.indexOf("%)",pos) - pos;

    QStringList args = title.mid (pos + 4, size - 4).split("%,");
    if(args.count() < 3)
    {
        qWarning("TitleFormatter: invalid title format");
        return title;
    }
    //process condition
    bool cond = TRUE;
    foreach(QString arg, args.at(0).split("%&"))
    {
        cond &= !arg.isEmpty();
    }
    QString r_str = cond ? args.at(1) : args.at(2);
    title.replace (pos, size + 2, r_str);
    if(title.contains("%if"))
        return processIfKeyWord(title);
    return title;
}
