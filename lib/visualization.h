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
#ifndef VISUALIZATION_H
#define VISUALIZATION_H

/**
   @author Ilya Kotov <forkotov02@hotmail.ru>
*/

#include <QMutex>

class Buffer;
class Decoder;
class Output;

class Visualization
{
public:
    Visualization() {};

    virtual ~Visualization() {};

    virtual void add(Buffer *, unsigned long, int, int) = 0;
    virtual void prepare() = 0;

    Decoder *decoder() const
    {
        return m_decoder;
    }
    void setDecoder(Decoder *decoder)
    {
        m_decoder = decoder;
    }

    Output *output() const
    {
        return m_output;
    }
    void setOutput(Output *output)
    {
        m_output = output;
    }

    QMutex *mutex()
    {
        return &m_mutex;
    }


private:
    Decoder *m_decoder;
    Output *m_output;
    QMutex m_mutex;

};

#endif
