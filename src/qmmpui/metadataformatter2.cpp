/***************************************************************************
 *   Copyright (C) 2015 by Ilya Kotov                                      *
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

MetaDataFormatter2::MetaDataFormatter2(const QString &format2)
{
    m_format = format2;
    //m_nodes = compile("%if(%if(%p,%a,%t),%a,%t) + %a");
    m_nodes = compile("%if(%a,%p,%t) + %a");

    foreach (Node n, m_nodes)
    {
        qDebug("=>%s", qPrintable(nodeToString(n)));
    }

    QMap<Qmmp::MetaData, QString> metaData;
    metaData[Qmmp::ARTIST] = "ARTIST";
    metaData[Qmmp::ALBUM] = "ALBUM";
    metaData[Qmmp::TITLE] = "TITLE";
    QString out = format(metaData);
    qDebug("=%s=", qPrintable(out));
}

QString MetaDataFormatter2::format(const PlayListTrack *item)
{
    return format(*item, item->length());
}

QString MetaDataFormatter2::format(const QMap<Qmmp::MetaData, QString> &metaData, qint64 length)
{
    QString title = execute(&m_nodes, &metaData, length);


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
    return title;
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

bool MetaDataFormatter2::processKey(QList<Node> *nodes, QString::const_iterator *i, QString::const_iterator end)
{
    QString token_name;
    Qmmp::MetaData key = Qmmp::UNKNOWN;
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

    if((*i) + 1 != end)
    {
        token_name.append((**i));
        token_name.append(*((*i)+1));
        key = key_map.value(token_name, Qmmp::UNKNOWN);
    }

    if(key == Qmmp::UNKNOWN)
    {
        token_name.clear();
        token_name.append((**i));
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
        (*i) += token_name.size() - 1;
        return true;
    }
    return false;
}

bool MetaDataFormatter2::processIf(QList<MetaDataFormatter2::Node> *nodes, QString::const_iterator *i, QString::const_iterator end)
{
    if((*i) + 1 == end || (*i) + 2 == end)
        return false;

    if((**i) != QChar('i') || *((*i)+1) != QChar('f'))
        return false;

    (*i)+=2;

    Node node;
    node.command = Node::IF_KEYWORD;

    int brackets_tracker = 0;
    QString var1, var2, var3;

    enum {
        STARTING = 0,
        READING_VAR1,
        READING_VAR2,
        READING_VAR3,
        FINISHED,

    } state = STARTING;

    while((*i) != end)
    {
        if((**i) == QChar('('))
        {
            brackets_tracker++;
            if(state == STARTING)
            {
                state = READING_VAR1;
                (*i)++;
                continue;
            }
        }
        else if((**i) == QChar(')'))
            brackets_tracker--;

        switch (state)
        {
        case STARTING:
        {
            break;
        }
        case READING_VAR1:
        {
            if((**i) == QChar(',') && brackets_tracker == 1)
            {
                state = READING_VAR2;
                break;
            }
            var1.append((**i));
            break;
        }
        case READING_VAR2:
        {
            if((**i) == QChar(',') && brackets_tracker == 1)
            {
                state = READING_VAR3;
                break;
            }
            var2.append((**i));
            break;
        }
        case READING_VAR3:
        {
            if((**i) == QChar(')') && brackets_tracker == 0)
            {
                state = FINISHED;
                break;
            }
            var3.append((**i));
            break;
        }
        default:
            break;
        }

        if(state == FINISHED)
            break;

        (*i)++;
    }

    //qDebug("%s|%s|%s", qPrintable(var1), qPrintable(var2), qPrintable(var3));

    //i--;
    //*n = i;
    Param param1, param2, param3;
    param1.type = Param::NODES, param2.type = Param::NODES, param3.type = Param::NODES;
    param1.children = compile(var1);
    param2.children = compile(var2);
    param3.children = compile(var3);
    node.params << param1 << param2 << param3;
    nodes->append(node);
    return true;
}

void MetaDataFormatter2::processText(QList<MetaDataFormatter2::Node> *nodes, QString::const_iterator *i, QString::const_iterator end)
{
    Node node;
    node.command = Node::PRINT_TEXT;
    Param param;
    param.type = Param::TEXT;
    node.params.append(param);

    forever
    {
        if((*i) == end || (**i) == QChar('%'))
        {
            (*i)--;
            break;
        }
        node.params[0].text.append(**i);
        (*i)++;
    }
    if(!node.params[0].text.isEmpty())
        nodes->append(node);
}

QString MetaDataFormatter2::execute(QList<Node> *nodes, const QMap<Qmmp::MetaData, QString> *metaData, qint64 length)
{
    QString out;
    for(int i = 0; i < nodes->count(); ++i)
    {
        Node node = nodes->at(i);
        if(node.command == Node::PRINT_TEXT)
        {
            Param p = node.params.first();
            out.append(printParam(&p, metaData, length));

        }
        else if(node.command == Node::IF_KEYWORD)
        {
            QString var1 = printParam(&node.params[0], metaData, length);
            if(var1.isEmpty())
                out.append(printParam(&node.params[2], metaData, length));
            else
                out.append(printParam(&node.params[1], metaData, length));
        }
    }
    return out;
}

QString MetaDataFormatter2::printParam(MetaDataFormatter2::Param *p, const QMap<Qmmp::MetaData, QString> *metaData, qint64 length)
{
    switch (p->type)
    {
    case Param::KEY:
        return metaData->value(p->key);
        break;
    case Param::TEXT:
        return p->text;
        break;
    case Param::NODES:
        return execute(&p->children, metaData, length);
        break;
    default:
        break;
    }
    return QString();
}

QString MetaDataFormatter2::nodeToString(MetaDataFormatter2::Node node)
{
    QString str;
    QStringList params;
    if(node.command == Node::PRINT_TEXT)
        str += "PRINT_TEXT";
    else if(node.command == Node::IF_KEYWORD)
        str += "IF_KEYWORD";
    str += "(";
    foreach (Param p, node.params)
    {
        if(p.type == Param::KEY)
            params.append(QString("KEY:%1").arg(p.key));
        else if(p.type == Param::TEXT)
            params.append(QString("TEXT:%1").arg(p.text));
        else if(p.type == Param::NODES)
        {
            QStringList nodeStrList;
            foreach (Node n, p.children)
            {
                nodeStrList.append(nodeToString(n));
            }
            params.append(QString("NODES:%1").arg(nodeStrList.join(",")));
        }
    }
    str.append(params.join(","));
    str.append(")");
    return str;
}

QList<MetaDataFormatter2::Node> MetaDataFormatter2::compile(const QString &format)
{
    //qDebug("=%s=", qPrintable(format));
    QList <Node> nodes;
    QString::const_iterator i = format.constBegin();

    while (i != format.constEnd())
    {
        if((*i) == QChar('%'))
        {
            i++;
            if(i == format.constEnd())
                continue;

            if(processKey(&nodes, &i, format.constEnd()))
            {
                i++;
                continue;
            }

            if(processIf(&nodes, &i, format.constEnd()))
            {
                i++;
                continue;
            }
            //i++;
            continue;
        }
        else
        {
            processText(&nodes, &i, format.constEnd());
            i++;
        }
    }
    return nodes;
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
