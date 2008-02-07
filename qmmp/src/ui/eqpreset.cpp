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
#include "eqpreset.h"

EQPreset::EQPreset()
 : QListWidgetItem()
{
    m_preamp = 0;
    for(int i = 0; i < 10; ++i)
    m_bands[i] = 0;
}


EQPreset::~EQPreset()
{}

void EQPreset::setGain(int n, int value)
{
    if(n > 9 || n < 0)
        return;
    m_bands[n] = value;
}

void EQPreset::setPreamp(int preamp)
{
    m_preamp = preamp;
}

int EQPreset::gain(int n)
{
    if(n > 9 || n < 0)
        return 0;
    return m_bands[n];
}

int EQPreset::preamp()
{
    return m_preamp;
}
