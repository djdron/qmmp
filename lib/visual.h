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
#ifndef VISUAL_H
#define VISUAL_H

/**
   @author Ilya Kotov <forkotov02@hotmail.ru>
*/

#include <QMutex>

class Buffer;
class Decoder;
class Output;

class Visual
{
public:
    Visual();

    virtual ~Visual();

    virtual void add(Buffer *, unsigned long, int, int) = 0;
    virtual void clear() = 0;

    Decoder *decoder() const;
    void setDecoder(Decoder *decoder);
    Output *output() const;
    void setOutput(Output *output);
    QMutex *mutex();

private:
    Decoder *m_decoder;
    Output *m_output;
    QMutex m_mutex;

};

#endif
