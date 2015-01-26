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

#ifndef METADATAFORMATTER2_H
#define METADATAFORMATTER2_H

#include <QString>
#include <QMap>
#include <qmmpui/playlisttrack.h>
#include <qmmp/qmmp.h>

/*! @brief The MetaDataFormatter formats metadata using templates.
 * @author Ilya Kotov <forkotov02@hotmail.ru>
 */
class MetaDataFormatter2
{
public:
    /*!
     * Constructor.
     * @param format Metadata template.
     * Syntax:
     * %p - artist,
     * %a - album,
     * %aa - album artist,
     * %t - title,
     * %n - track number,
     * %NN - 2-digit track number,
     * %g - genre,
     * %c - comment,
     * %C - composer,
     * %D - disc number,
     * %f - file name,
     * %F - full path,
     * %y - year,
     * %l - duration,
     * %if(A,B,C) or %if(A&B&C,D,E) - condition.
     */
    MetaDataFormatter2(const QString &format = QString());
    /*!
     * Converts metadata of item \b item to one string using template.
     */
    QString parse(const PlayListTrack *item);
    /*!
     * Converts metadata to one string using template.
     * @param metaData Metadata array.
     * @param length Length in seconds.
     */
    QString parse(const QMap<Qmmp::MetaData, QString> &metaData, qint64 length = 0);
    /*!
     * Returns formatted length (example: 05:02:03).
     * \param length Length in seconds.
     */
    QString formatLength(qint64 length) const;

private:
    struct Node;
    struct Param
    {
        enum {
            KEY = 0,
            TEXT,
            NODES
        } type;

        Qmmp::MetaData key;
        QString text;
        QList<Node> children;
    };
    struct Node
    {
        enum {
            PRINT_TEXT = 0,
            IF_KEYWORD
        } command;

        QList<Param> params;
    };

    bool processKey(QList<Node> *nodes, QString::const_iterator i, QString::const_iterator end);
    bool processIf(QList<Node> *nodes, QString::const_iterator i, QString::const_iterator end);
    void processText(QList<Node> *nodes, QString::const_iterator i, QString::const_iterator end);

    QList<MetaDataFormatter2::Node> compile(const QString &format);
    QString m_format;
    QList<Node> m_nodes;
    //QString processIfKeyWord(QString title);
};

#endif // METADATAFORMATTER2_H
