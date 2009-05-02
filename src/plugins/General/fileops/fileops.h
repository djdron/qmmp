/***************************************************************************
 *   Copyright (C) 2009 by Ilya Kotov                                      *
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
#ifndef FILEOPS_H
#define FILEOPS_H

#include <qmmpui/general.h>
#include <qmmp/qmmp.h>

class QAction;
class SoundCore;

/**
	@author Ilya Kotov <forkotov02@hotmail.ru>
*/

class FileOps : public General
{
Q_OBJECT
public:
    FileOps(QObject *parent = 0);

    ~FileOps();

    enum ActionType
    {
        COPY = 0,
        RENAME,
        MOVE,
        REMOVE
    };


private:
    QAction *m_copyAction;
    QAction *m_moveAction;
    QAction *m_removeAction;
    QString m_copyDestination;
};

#endif
