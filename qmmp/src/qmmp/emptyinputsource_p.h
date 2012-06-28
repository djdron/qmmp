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

#ifndef EMPTYINPUTSOURCE_P_H
#define EMPTYINPUTSOURCE_P_H

#include "inputsource.h"

/*! @internal
    @author Ilya Kotov <forkotov02@hotmail.ru>
*/
class EmptyInputSource : public InputSource
{
Q_OBJECT
public:
    EmptyInputSource(const QString &url, QObject *parent = 0);

    QIODevice *ioDevice();
    bool initialize();
    bool isReady();

private:
    bool m_ok;
};

#endif // EMPTYINPUTSOURCE_P_H