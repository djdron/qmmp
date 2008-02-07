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

#include <QFileInfo>

#ifndef XSPF_PLUGIN
	#include <QDomDocument>
	#include <QDomElement>
	#include <QUrl>
	#include "version.h"
#endif

#include "playlistformat.h"

#include "mediafile.h"

bool PLSPlaylistFormat::hasFormat(const QString & f)
{
	foreach(QString s,m_supported_formats)
		if(f == s)
			return true;

	return false;
}

QStringList PLSPlaylistFormat::getExtensions() const
{
	return m_supported_formats;
}

PLSPlaylistFormat::PLSPlaylistFormat()
{
	m_supported_formats << "pls";
}

QString PLSPlaylistFormat::name() const
{
	return "PLSPlaylistFormat";
}


QStringList PLSPlaylistFormat::decode(const QString & contents)
{
	QStringList out;
	QStringList splitted = contents.split("\n");
	if(!splitted.isEmpty())
	{
		if(splitted.takeAt(0).toLower().contains("[playlist]"))
		{
			foreach(QString str, splitted)
			{
				if(str.startsWith("File"))
				{
					QString unverified = str.remove(0,str.indexOf(QChar('=')) + 1);
					if(QFileInfo(unverified).exists())
						out << QFileInfo(unverified).absoluteFilePath();
					else
						qWarning("File %s does not exist",unverified.toLocal8Bit().data());
				}
			}
			return out;
		}
	}
	else
		qWarning("Error parsing PLS format");

	return QStringList();
}

QString PLSPlaylistFormat::encode(const QList< MediaFile * > & contents)
{
	QStringList out;
	out << QString("[playlist]");
	int counter = 1;
	foreach(MediaFile* f,contents)
	{
		QString begin = "File" + QString::number(counter) + "=";
		out.append(begin + f->path());
		begin = "Title" + QString::number(counter) + "=";
		out.append(begin + f->title());
		begin = "Length" + QString::number(counter) + "=";
		out.append(begin + QString::number(f->length()));
		counter ++;
	}
	out << "NumberOfEntries=" + QString::number(contents.count());
	return out.join("\n");
}




bool M3UPlaylistFormat::hasFormat(const QString & f)
{
	foreach(QString s,m_supported_formats)
		if(f == s)
			return true;

	return false;
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
	if(!splitted.isEmpty())
	{
		if(splitted.takeAt(0).contains("#EXTM3U"))
		{
			foreach(QString str, splitted)
			{
				if(str.startsWith("#EXTINF:"))
					;//TODO: Let's skip it for now...
				else if(QFileInfo(str).exists())
					out << QFileInfo(str).absoluteFilePath();
				else
					qWarning("File %s does not exist",str.toLocal8Bit().data());
			}
			return out;
		}
	}
	else
		qWarning("Error parsing M3U format");

	return QStringList();
}

QString M3UPlaylistFormat::encode(const QList< MediaFile * > & contents)
{
	QStringList out;
	out << QString("#EXTM3U");
	foreach(MediaFile* f,contents)
	{
		QString info = "#EXTINF:" + QString::number(f->length()) + "," + f->title();
		out.append(info);
		out.append(f->path());
	}
	return out.join("\n");
}

QString M3UPlaylistFormat::name() const
{
	return "M3UPlaylistFormat";
}


// Needs more work - it's better use libSpiff there and put it as plugin.

QStringList XSPFPlaylistFormat::decode(const QString & contents)
{
	QStringList out;
	QDomDocument doc;
	QString errorMsg;
	int errorCol;
	int errorRow;
	bool ok = doc.setContent(contents, &errorMsg, &errorRow, &errorCol);

	if(!ok)
		qDebug("Parse Error: %s\tRow:%d\tCol%d", 
				 qPrintable(errorMsg), errorRow, errorCol );

	QDomElement rootElement = doc.firstChildElement("playlist");
	if(rootElement.isNull()) 
		qWarning("Error parsing XSPF: can't find 'playlist' element");
	
	QDomElement tracklistElement = rootElement.firstChildElement("trackList");
	if(tracklistElement.isNull()) 
		qWarning("Error parsing XSPF: can't find 'trackList' element");
	
	QDomElement child = tracklistElement.firstChildElement("track");

	while (!child.isNull())
	{	
		QString str = QUrl(child.firstChildElement("location").text()).toString(QUrl::RemoveScheme);
		out << str;
		child = child.nextSiblingElement();
	}
	
	return out;
}

// Needs more work - it's better use libSpiff there and put it as plugin.

QString XSPFPlaylistFormat::encode(const QList< MediaFile * > & files)
{
	QDomDocument doc;
	QDomElement root = doc.createElement("playlist");
	root.setAttribute("version",QString("1"));
	root.setAttribute("xmlns",QString("http://xspf.org/ns/0"));
	
	QDomElement creator = doc.createElement("creator");
	QDomText text = doc.createTextNode("qmmp-" + QString(QMMP_STR_VERSION));
	creator.appendChild(text);
	root.appendChild(creator);
	
	QDomElement tracklist = doc.createElement("trackList");

	int counter = 1;
	foreach(MediaFile* f,files)
	{	
		QDomElement track = doc.createElement("track");

		QDomElement ch = doc.createElement("location");
		QDomText text = doc.createTextNode(/*QString("file://") + */QFileInfo(f->path()).absoluteFilePath());
		ch.appendChild(text);
		track.appendChild(ch);
		
		ch = doc.createElement("title");
		text = doc.createTextNode(f->title());
		ch.appendChild(text);
		track.appendChild(ch);
		
		ch = doc.createElement("trackNum");
		text = doc.createTextNode(QString::number(counter));
		ch.appendChild(text);
		track.appendChild(ch);
		
		ch = doc.createElement("year");
		text = doc.createTextNode(QString::number(f->year()));
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

XSPFPlaylistFormat::XSPFPlaylistFormat()
{
	m_supported_formats << "xspf";
}

bool XSPFPlaylistFormat::hasFormat(const QString & f)
{
	foreach(QString s,m_supported_formats)
		if(f == s)
			return true;

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





