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
#ifndef GENERAL_H
#define GENERAL_H

#include <QObject>
#include <QStringList>
#include <QMap>

#include "songinfo.h"
#include "generalfactory.h"

class Control;

/**
    @author Ilya Kotov <forkotov02@hotmail.ru>
*/
class General : public QObject
{
    Q_OBJECT
public:
    General(QObject *parent = 0);

    ~General();

    enum State
    {
        Playing = 0,
        Paused,
        Stopped
    };

    virtual void setState(const uint &state);
    virtual void setSongInfo(const SongInfo &song);
    virtual void setVolume(int left, int right);

    //static methods
    static QList<GeneralFactory*> *generalFactories();
    static QStringList generalFiles();
    static void setEnabled(GeneralFactory* factory, bool enable = TRUE);
    static bool isEnabled(GeneralFactory* factory);

private:
    QMap <uint, QString> m_strValues;
    QMap <uint, uint> m_numValues;

};

#endif
