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

#include <QApplication>
extern "C"{
#include <wildmidi_lib.h>
}
#include "wildmidihelper.h"

WildMidiHelper *WildMidiHelper::m_instance = 0;

WildMidiHelper::WildMidiHelper(QObject *parent) :
    QObject(parent)
{
    m_inited = false;
}

WildMidiHelper::~WildMidiHelper()
{
    if(m_inited)
        WildMidi_Shutdown();
    m_instance = 0;
}

bool WildMidiHelper::initialize()
{
    if(m_inited)
        return true;
    if (WildMidi_Init ("/etc/timidity/timidity.cfg", 48000, 0) < 0)
    {
        qWarning("WildMidiHelper: unable to initialize WildMidi library");
        return false;
    }
    m_inited = true;
    return true;
}


WildMidiHelper *WildMidiHelper::instance()
{
    if(!m_instance)
        m_instance = new WildMidiHelper(qApp);
    return m_instance;
}
