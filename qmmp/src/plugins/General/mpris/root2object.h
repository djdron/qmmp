/***************************************************************************
 *   Copyright (C) 2010 by Ilya Kotov                                      *
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
#ifndef ROOT2OBJECT_H
#define ROOT2OBJECT_H

#include <QObject>
#include <QStringList>

/**
    @author Ilya Kotov <forkotov02@hotmail.ru>
*/
class Root2Object : public QObject
{
Q_OBJECT
Q_CLASSINFO("D-Bus Interface", "org.mpris.MediaPlayer2")
Q_PROPERTY(bool CanQuit READ canQuit)
Q_PROPERTY(bool CanRaise READ canRaise)
Q_PROPERTY(QString DesktopEntry READ desktopEntry)
Q_PROPERTY(bool HasTrackList READ hasTrackList)
Q_PROPERTY(QString Identity READ identity)
Q_PROPERTY(QStringList SupportedMimeTypes READ supportedMimeTypes)
Q_PROPERTY(QStringList SupportedUriSchemes READ supportedUriSchemes)

public:
    Root2Object(QObject *parent = 0);
    virtual ~Root2Object();

    bool canQuit() const;
    bool canRaise() const;
    QString desktopEntry() const;
    bool hasTrackList() const;
    QString identity() const;
    QStringList supportedMimeTypes() const;
    QStringList supportedUriSchemes() const;

public slots:
     void Quit();
     void Raise();
};

#endif
