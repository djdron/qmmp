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
#ifndef COMMANDLINEMANAGER_H
#define COMMANDLINEMANAGER_H

#include "general.h"
#include "commandlineoption.h"


/**
	@author Ilya Kotov <forkotov02@hotmail.ru>
*/
class CommandLineManager : public General
{
Q_OBJECT
public:
    CommandLineManager(QObject *parent = 0);

    ~CommandLineManager();

    //general

    //properties

    //command line
    void executeCommand(const QString& opt_str);

    //static methods
    static bool hasOption(const QString &opt_str);
    static void printUsage();

private:
    uint m_state;
    int m_left, m_right;
    int m_time;
};

#endif
