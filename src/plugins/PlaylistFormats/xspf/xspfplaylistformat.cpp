/***************************************************************************
 *   Copyright (C) 2008-2012 by Ilya Kotov                                 *
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

#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QFileInfo>
#include <QUrl>
#include <QtPlugin>

#include <qmmp/qmmp.h>

#include "xspfplaylistformat.h"

// Needs more work - it's better use libSpiff there and put it as plugin.

QStringList XSPFPlaylistFormat::decode(const QString &contents)
{
    QStringList out;
    QString currentTag;

    QXmlStreamReader xml(contents);
    while(!xml.atEnd())
    {
        xml.readNext();
        if (xml.isStartElement())
        {
            currentTag = xml.name().toString();

        }
        else if (xml.isCharacters() && !xml.isWhitespace())
        {
            if (currentTag == "location")
            {

                QUrl url(xml.text().toString());
                if (url.scheme() == "file")  //remove scheme for local files only
                    out << QUrl::fromPercentEncoding(url.toString(QUrl::RemoveScheme).toAscii());
                else
                    out << url.toString();
            }
        }
    }

    if(xml.hasError())
    {
        qDebug("XSPFPlaylistFormat: parse error: %s (row:%lld, col:%lld",
               qPrintable(xml.errorString()), xml.lineNumber(), xml.columnNumber());
        out.clear();
    }
    return out;
}

// Needs more work - it's better use libSpiff there and put it as plugin.

QString XSPFPlaylistFormat::encode(const QList<PlayListItem*> &files)
{
    QString out;
    QXmlStreamWriter xml(&out);
    xml.setCodec("UTF-8");
    xml.setAutoFormatting(true);
    xml.writeStartDocument();
    xml.writeStartElement("playlist");
    xml.writeAttribute("version", "1");
    xml.writeAttribute("xmlns", "http://xspf.org/ns/0/");
    xml.writeTextElement("creator", "qmmp-" + Qmmp::strVersion());
    xml.writeStartElement("trackList");

    int counter = 1;
    foreach(PlayListItem* f,files)
    {
        xml.writeStartElement("track");

        QString url;
        if (f->url().contains("://"))
            url = QUrl::toPercentEncoding(f->url(), ":/");
        else  //append protocol
            url = QUrl::toPercentEncoding(QString("file://") +
                                          QFileInfo(f->url()).absoluteFilePath(), ":/");

        xml.writeTextElement("location", url);
        xml.writeTextElement("title", f->value(Qmmp::TITLE));
        xml.writeTextElement("creator", f->value(Qmmp::ARTIST));
        xml.writeTextElement("annotation", f->value(Qmmp::COMMENT));
        xml.writeTextElement("album", f->value(Qmmp::ALBUM));
        xml.writeTextElement("trackNum", QString::number(counter));

        xml.writeStartElement("meta");
        xml.writeAttribute("rel", "year");
        xml.writeCharacters(f->value(Qmmp::YEAR));
        xml.writeEndElement(); // meta
        xml.writeEndElement(); // track

        counter ++;
    }
    xml.writeEndElement(); //trackList
    xml.writeEndElement(); //playlist
    xml.writeEndDocument();
    return out;
}

XSPFPlaylistFormat::XSPFPlaylistFormat()
{
    m_supported_formats << "xspf";
}

bool XSPFPlaylistFormat::hasFormat(const QString & f)
{
    foreach(QString s,m_supported_formats)
    {
        if (f == s)
            return true;
    }
    return false;
}

QStringList XSPFPlaylistFormat::getExtensions() const
{
    return m_supported_formats;
}


QString XSPFPlaylistFormat::name() const
{
    return "XSPFPlaylistFormat";
}

Q_EXPORT_PLUGIN2(xspfplaylistformat,XSPFPlaylistFormat)
