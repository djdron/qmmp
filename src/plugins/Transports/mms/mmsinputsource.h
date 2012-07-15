/***************************************************************************
 *   Copyright (C) 2009-2012 by Ilya Kotov                                 *
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

#ifndef HTTPINPUTSOURCE_H
#define HTTPINPUTSOURCE_H

#include <qmmp/inputsource.h>

class MMSStreamReader;

/**
    @author Ilya Kotov <forkotov02@hotmail.ru>
*/
class MMSInputSource : public InputSource
{
Q_OBJECT
public:
    MMSInputSource(const QString &url, QObject *parent = 0);

    QIODevice *ioDevice();
    bool initialize();
    bool isReady();
    QString contentType() const;

private:
    MMSStreamReader *m_reader;

};

#endif // MMSINPUTSOURCE_H