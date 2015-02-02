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

struct MetaDataFormatter2::Param
{
    enum {
        FIELD = 0,
        TEXT,
        NODES
    } type;

    //extra fields
    enum
    {
        TWO_DIGIT_TRACK = Qmmp::URL + 1,
        DURATION,
        FILE_NAME
    };

    int field;
    QString text;
    QList<Node> children;
};

struct MetaDataFormatter2::Node
{
    enum {
        PRINT_TEXT = 0,
        IF_KEYWORD
    } command;

    QList<Param> params;
};

MetaDataFormatter2::MetaDataFormatter2(const QString &pattern)
{
    m_fieldNames.insert("t", Qmmp::TITLE);
    m_fieldNames.insert("p", Qmmp::ARTIST);
    m_fieldNames.insert("aa", Qmmp::ALBUMARTIST);
    m_fieldNames.insert("a", Qmmp::ALBUM);
    m_fieldNames.insert("c", Qmmp::COMMENT);
    m_fieldNames.insert("g", Qmmp::GENRE);
    m_fieldNames.insert("C", Qmmp::COMPOSER);
    m_fieldNames.insert("y", Qmmp::YEAR);
    m_fieldNames.insert("n", Qmmp::TRACK);
    m_fieldNames.insert("D", Qmmp::DISCNUMBER);
    m_fieldNames.insert("F", Qmmp::URL);
    m_fieldNames.insert("NN", Param::TWO_DIGIT_TRACK);
    m_fieldNames.insert("l", Param::DURATION);
    m_fieldNames.insert("f", Param::FILE_NAME);

    setPattern(pattern);
}

void MetaDataFormatter2::setPattern(const QString &pattern)
{
    m_pattern = pattern;
    m_nodes.clear();

    qDebug("MetaDataFormatter: pattern: %s", qPrintable(pattern));
    m_nodes = compile(pattern);
    qDebug("MetaDataFormatter: dump of nodes");
    foreach (Node n, m_nodes)
    {
        qDebug("=>%s", qPrintable(dumpNode(n)));
    }
    qDebug("MetaDataFormatter: end of dump");
}

const QString MetaDataFormatter2::pattern() const
{
    return m_pattern;
}

QString MetaDataFormatter2::format(const PlayListTrack *item)
{
    return format(*item, item->length());
}

QString MetaDataFormatter2::format(const QMap<Qmmp::MetaData, QString> &metaData, qint64 length)
{
    return evalute(&m_nodes, &metaData, length).trimmed();
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

bool MetaDataFormatter2::parseField(QList<Node> *nodes, QString::const_iterator *i, QString::const_iterator end)
{
    QString fieldName;
    int field = Qmmp::UNKNOWN;

    //try to find field with 2 symbols
    if((*i) + 1 != end)
    {
        fieldName.append((**i));
        fieldName.append(*((*i)+1));
        field = m_fieldNames.value(fieldName, Qmmp::UNKNOWN);
    }
    //try to find field with 1 symbol
    if(field == Qmmp::UNKNOWN)
    {
        fieldName.clear();
        fieldName.append((**i));
        field = m_fieldNames.value(fieldName, Qmmp::UNKNOWN);
    }

    if(field != Qmmp::UNKNOWN)
    {
        Node node;
        node.command = Node::PRINT_TEXT;
        Param param;
        param.type = Param::FIELD;
        param.field = field;
        node.params.append(param);
        nodes->append(node);
        (*i) += fieldName.size() - 1;
        return true;
    }
    return false;
}

bool MetaDataFormatter2::parseIf(QList<MetaDataFormatter2::Node> *nodes, QString::const_iterator *i, QString::const_iterator end)
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

    if(state != FINISHED)
    {
        qWarning("MetaDataFormatter: syntax error");
        return false;
    }

    Param param1, param2, param3;
    param1.type = Param::NODES, param2.type = Param::NODES, param3.type = Param::NODES;
    param1.children = compile(var1);
    param2.children = compile(var2);
    param3.children = compile(var3);
    node.params << param1 << param2 << param3;
    nodes->append(node);
    return true;
}

void MetaDataFormatter2::parseText(QList<MetaDataFormatter2::Node> *nodes, QString::const_iterator *i, QString::const_iterator end)
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

QString MetaDataFormatter2::evalute(QList<Node> *nodes, const QMap<Qmmp::MetaData, QString> *metaData, qint64 length)
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
    case Param::FIELD:
        return printField(p->field, metaData, length);
        break;
    case Param::TEXT:
        return p->text;
        break;
    case Param::NODES:
        return evalute(&p->children, metaData, length);
        break;
    default:
        break;
    }
    return QString();
}

QString MetaDataFormatter2::printField(int field, const QMap<Qmmp::MetaData, QString> *metaData, qint64 length)
{
    if(field >= Qmmp::TITLE && field <= Qmmp::URL)
    {
        if(field == Qmmp::TITLE)
        {
            QString title = metaData->value(Qmmp::TITLE);
            if(title.isEmpty()) //using file name if title is empty
            {
                title = metaData->value(Qmmp::URL).section('/',-1);
                title = title.left(title.lastIndexOf('.'));
            }
            return title;
        }
        return metaData->value((Qmmp::MetaData) field);
    }
    else if(field == Param::TWO_DIGIT_TRACK)
    {
        return QString("%1").arg(metaData->value(Qmmp::TRACK),2,'0');
    }
    else if(field == Param::DURATION)
    {
        return formatLength(length);
    }
    else if(field == Param::FILE_NAME)
    {
        return metaData->value(Qmmp::URL).section('/',-1);
    }
    return QString();
}

QString MetaDataFormatter2::dumpNode(MetaDataFormatter2::Node node)
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
        if(p.type == Param::FIELD)
            params.append(QString("FIELD:%1").arg(p.field));
        else if(p.type == Param::TEXT)
            params.append(QString("TEXT:%1").arg(p.text));
        else if(p.type == Param::NODES)
        {
            QStringList nodeStrList;
            foreach (Node n, p.children)
            {
                nodeStrList.append(dumpNode(n));
            }
            params.append(QString("NODES:%1").arg(nodeStrList.join(",")));
        }
    }
    str.append(params.join(","));
    str.append(")");
    return str;
}

QList<MetaDataFormatter2::Node> MetaDataFormatter2::compile(const QString &expr)
{
    QList <Node> nodes;
    QString::const_iterator i = expr.constBegin();

    while (i != expr.constEnd())
    {
        if((*i) == QChar('%'))
        {
            i++;
            if(i == expr.constEnd())
                continue;

            if(parseField(&nodes, &i, expr.constEnd()))
            {
                i++;
                continue;
            }

            if(parseIf(&nodes, &i, expr.constEnd()))
            {
                i++;
                continue;
            }
            continue;
        }
        else
        {
            parseText(&nodes, &i, expr.constEnd());
            i++;
        }
    }
    return nodes;
}
