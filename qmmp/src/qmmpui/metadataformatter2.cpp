/***************************************************************************
 *   Copyright (C) 2009-2014 by Ilya Kotov                                 *
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
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/


/*
Syntax:
%p - artist,
%a - album,
%aa - album artist,
%t - title,
%n - track number,
%NN - 2-digit track number,
%g - genre,
%c - comment,
%C - composer,
%D - disc number,
%f - file name,
%F - full path,
%y - year,
%l - duration,
%if(A,B,C) or %if(A&B&C,D,E) - condition.
*/

#include <QStringList>
#include <QUrl>
#include "metadataformatter2.h"

MetaDataFormatter2::MetaDataFormatter2(const QString &format)
{
    m_format = format;
    //compile("%p - %a");
}

QString MetaDataFormatter2::parse(const PlayListTrack *item)
{
    return parse(*item, item->length());
}

QString MetaDataFormatter2::parse(const QMap<Qmmp::MetaData, QString> &metaData, qint64 length)
{
    /*QString title = m_format;
    title.replace("\\(", "%28");
    title.replace("\\)", "%29");
    title.replace(")", "%)");
    title.replace("&", "%&");
    title.replace(",", "%,");
    title.replace("%p", metaData[Qmmp::ARTIST]);
    title.replace("%aa", metaData[Qmmp::ALBUMARTIST]);
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
        title.replace("%l",formatLength(length));
    if(title.contains("%if"))
        title = processIfKeyWord(title);
    title.replace("%28", "(");
    title.replace("%29", ")");
    return title.trimmed();*/
    return QString();
}

QString MetaDataFormatter2::formatLength(qint64 length) const
{
    if(length <= 0)
        return QString();
    QString str;
    if(length >= 3600)
        str = QString("%1:%2").arg(length/3600).arg(length%3600/60, 2, 10, QChar('0'));
    else
        str = QString("%1").arg(length%3600/60);
    str += QString(":%1").arg(length%60, 2, 10, QChar('0'));
    return str;
}

bool MetaDataFormatter2::processKey(QList<Node> *nodes, QString::const_iterator i, QString::const_iterator end)
{
    QHash<QString, Qmmp::MetaData> key_map;
    key_map.insert("p", Qmmp::ARTIST);
    key_map.insert("aa", Qmmp::ALBUMARTIST);
    key_map.insert("a", Qmmp::ALBUM);
    key_map.insert("t", Qmmp::TITLE);
    key_map.insert("n", Qmmp::TRACK);
    key_map.insert("g", Qmmp::GENRE);
    key_map.insert("c", Qmmp::COMMENT);
    key_map.insert("C", Qmmp::COMPOSER);
    key_map.insert("D", Qmmp::DISCNUMBER);
    key_map.insert("F", Qmmp::URL);
    key_map.insert("y", Qmmp::YEAR);

    QString token_name = (*i); //2 symbols
    i++; //TODO check end
    token_name.append(*i);

    Qmmp::MetaData key = key_map.value(token_name, Qmmp::UNKNOWN);

    if(key == Qmmp::UNKNOWN)
    {
        i--;
        token_name = (*i); //1 symbol
        key = key_map.value(token_name, Qmmp::UNKNOWN);
    }

    if(key != Qmmp::UNKNOWN)
    {
        Node node;
        node.command = Node::PRINT_TEXT;
        Param param;
        param.type = Param::KEY;
        param.key = key;
        node.params.append(param);
        nodes->append(node);
        return true;
    }
    return false;
}

void MetaDataFormatter2::processText(QList<MetaDataFormatter2::Node> *nodes, QString::const_iterator i, QString::const_iterator end)
{
    Node node;
    node.command = Node::PRINT_TEXT;
    Param param;
    param.type = Param::TEXT;
    node.params.append(param);

    while (i != end)
    {
        node.params[0].text.append(*i);
        i++;
        if((*i) == QChar('%'))
        {
            i--;
            nodes->append(node);
            return;
        }
    }
    nodes->append(node);
}

void MetaDataFormatter2::compile(const QString &format)
{
    QString::const_iterator i = format.constBegin();

    while (i != format.constEnd())
    {
        i++;
        if((*i) == QChar('%'))
        {
            i++;
            if(processKey(&m_nodes, i, format.constEnd()))
            {
                continue;
            }
        }
        else
        {
            processText(&m_nodes, i, format.constEnd());
            continue;
        }
    }
}

/*QString MetaDataFormatter::processIfKeyWord(QString title)
{
    int pos = title.lastIndexOf("%if(");
    int size = title.indexOf("%)",pos) - pos;

    QStringList args = title.mid (pos + 4, size - 4).split("%,");
    if(args.count() < 3)
    {
        qWarning("TitleFormatter: invalid title format");
        return title;
    }
    //process condition
    bool cond = true;
    foreach(QString arg, args.at(0).split("%&"))
    {
        cond &= !arg.isEmpty();
    }
    QString r_str = cond ? args.at(1) : args.at(2);
    title.replace (pos, size + 2, r_str);
    if(title.contains("%if"))
        return processIfKeyWord(title);
    return title;
}*/
