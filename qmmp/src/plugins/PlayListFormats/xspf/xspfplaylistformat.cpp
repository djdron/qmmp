/***************************************************************************
 *   Copyright (C) 2008-2013 by Ilya Kotov                                 *
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

#include <QDomDocument>
#include <QDomElement>
#include <QFileInfo>
#include <QUrl>
#include <QtPlugin>
#include <qmmp/qmmp.h>
#include "xspfplaylistformat.h"

// Needs more work - it's better use libSpiff there and put it as plugin.

const PlayListFormatProperties XSPFPlaylistFormat::XSPFPlaylistFormat::properties() const
{
    PlayListFormatProperties p;
    p.filters << "*.xspf";
    p.shortName = "xspf";
    p.contentTypes << "application/xspf+xml";
    return p;
}

QStringList XSPFPlaylistFormat::decode(const QString & contents)
{
    QStringList out;
    QDomDocument doc;
    QString errorMsg;
    int errorCol;
    int errorRow;
    bool ok = doc.setContent(contents, &errorMsg, &errorRow, &errorCol);

    if (!ok)
        qDebug("Parse Error: %s\tRow:%d\tCol%d",
               qPrintable(errorMsg), errorRow, errorCol );

    QDomElement rootElement = doc.firstChildElement("playlist");
    if (rootElement.isNull())
        qWarning("Error parsing XSPF: can't find 'playlist' element");

    QDomElement tracklistElement = rootElement.firstChildElement("trackList");
    if (tracklistElement.isNull())
        qWarning("Error parsing XSPF: can't find 'trackList' element");

    QDomElement child = tracklistElement.firstChildElement("track");

    while (!child.isNull())
    {
        QUrl url (child.firstChildElement("location").text());
        if (url.scheme() == "file")  //remove scheme for local files only
            out << QUrl::fromPercentEncoding(url.toString(QUrl::RemoveScheme).toAscii());
        else
            out << url.toString();
        child = child.nextSiblingElement();
    }

    return out;
}

// Needs more work - it's better use libSpiff there and put it as plugin.

QString XSPFPlaylistFormat::encode(const QList<PlayListTrack*> & files)
{
    QDomDocument doc;
    QDomElement root = doc.createElement("playlist");
    root.setAttribute("version",QString("1"));
    root.setAttribute("xmlns",QString("http://xspf.org/ns/0/"));

    QDomElement creator = doc.createElement("creator");
    QDomText text = doc.createTextNode("qmmp-" + Qmmp::strVersion());
    creator.appendChild(text);
    root.appendChild(creator);

    QDomElement tracklist = doc.createElement("trackList");

    int counter = 1;
    foreach(PlayListTrack* f,files)
    {
        QDomElement track = doc.createElement("track");

        QDomElement ch = doc.createElement("location");
        QDomText text;
        if (f->url().contains("://"))
            text = doc.createTextNode(QUrl::toPercentEncoding(f->url(), ":/"));
        else  //append protocol
            text = doc.createTextNode(QUrl::toPercentEncoding(QString("file://") +
                                      QFileInfo(f->url()).absoluteFilePath(), ":/"));
        ch.appendChild(text);
        track.appendChild(ch);

        ch = doc.createElement("title");
        text = doc.createTextNode(f->value(Qmmp::TITLE));
        ch.appendChild(text);
        track.appendChild(ch);

        ch = doc.createElement("creator");
        text = doc.createTextNode(f->value(Qmmp::ARTIST));
        ch.appendChild(text);
        track.appendChild(ch);

        ch = doc.createElement("annotation");
        text = doc.createTextNode(f->value(Qmmp::COMMENT));
        ch.appendChild(text);
        track.appendChild(ch);

        ch = doc.createElement("album");
        text = doc.createTextNode(f->value(Qmmp::ALBUM));
        ch.appendChild(text);
        track.appendChild(ch);

        ch = doc.createElement("trackNum");
        text = doc.createTextNode(QString::number(counter));
        ch.appendChild(text);
        track.appendChild(ch);

        ch = doc.createElement("meta");
        ch.setAttribute("rel", "year");
        text = doc.createTextNode(f->value(Qmmp::YEAR));
        ch.appendChild(text);
        track.appendChild(ch);

        tracklist.appendChild(track);
        counter ++;
    }

    root.appendChild(tracklist);
    doc.appendChild( root );
    QString xml_header("<?xml version='1.0' encoding='UTF-8'?>\n");
    return doc.toString().prepend(xml_header);
}

Q_EXPORT_PLUGIN2(xspfplaylistformat,XSPFPlaylistFormat)
