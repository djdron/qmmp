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

#ifndef _PALYLISTFORMAT_H
#define _PALYLISTFORMAT_H
#include <QStringList>


class MediaFile;
/*!
 * Abstract interface for playlist formats.
 *
 * @author Vladimir Kuznetsov <vovanec@gmail.com>
 */
class PlaylistFormat
{
public:
    virtual ~PlaylistFormat(){;}
	 /*!
	  * Takes raw contents of playlist file, should return string list of 
	  * ready file pathes to fill the playlist.
	  */
    virtual QStringList decode(const QString& contents) = 0;
	 
	 /*!
	  * Takes the list of MediaFile objects, should return string of
	  * encoded playlist file
	  */
	 virtual QString encode(const QList<MediaFile*>& contents) = 0;
	 
	 /*!
	  * Returns list of file extensions that current format supports
	  */
	 virtual QStringList getExtensions()const = 0;
	 
	 /*!
	  * Verifies is the \b ext file extension supported by current playlist format.
	  */
	 virtual bool hasFormat(const QString& ext) = 0;
	 
	 /// Unique name of playlist format.
	 virtual QString name()const = 0;
};

Q_DECLARE_INTERFACE(PlaylistFormat,"PlaylistFormatInterface/1.0");

/*!
 * Class for PLS playlist format parsing
 */
class PLSPlaylistFormat : public PlaylistFormat
{
public:
	PLSPlaylistFormat();
	virtual QStringList getExtensions()const;
	virtual bool hasFormat(const QString&);
	virtual QStringList decode(const QString& contents);
	virtual QString encode(const QList<MediaFile*>& contents);
	virtual QString name()const;
protected:
	QStringList m_supported_formats;
	 
};



/*!
 * Class for M3U playlist format parsing
 */
class M3UPlaylistFormat : public PlaylistFormat
{
	public:
		M3UPlaylistFormat();
		virtual QStringList getExtensions()const;
		virtual bool hasFormat(const QString&);
		virtual QStringList decode(const QString& contents);
		virtual QString encode(const QList<MediaFile*>& contents);
		virtual QString name()const;
protected:
		QStringList m_supported_formats;
};


/*!
 * Class for XSPF playlist format parsing
 */
class XSPFPlaylistFormat : public PlaylistFormat
{
	public:
		XSPFPlaylistFormat();
		virtual QStringList getExtensions()const;
		virtual bool hasFormat(const QString&);
		virtual QStringList decode(const QString& contents);
		virtual QString encode(const QList<MediaFile*>& contents);
		virtual QString name()const;
	protected:
		QStringList m_supported_formats;
};

#endif
