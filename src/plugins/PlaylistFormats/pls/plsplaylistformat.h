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
#ifndef PLSPLAYLISTFORMAT_H
#define PLSPLAYLISTFORMAT_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <qmmpui/playlistformat.h>
#include <qmmpui/songinfo.h>

/*!
 * Class for PLS playlist format parsing
 */
class PLSPlaylistFormat : public QObject, public PlaylistFormat
{
    Q_OBJECT
    Q_INTERFACES(PlaylistFormat)
public:
    PLSPlaylistFormat();
    virtual QStringList getExtensions()const;
    virtual bool hasFormat(const QString&);
    virtual QStringList decode(const QString& contents);
    virtual QString encode(const QList<SongInfo*>& contents);
    virtual QString name()const;
protected:
    QStringList m_supported_formats;
};

#endif
