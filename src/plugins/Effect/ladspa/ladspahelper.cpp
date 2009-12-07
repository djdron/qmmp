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

#include <QApplication>
#include "ladspahost.h"
#include "ladspahelper.h"

LADSPAHelper::LADSPAHelper() : Effect()
{
    if(!LADSPAHost::instance())
        new LADSPAHost(qApp);
}

LADSPAHelper::~LADSPAHelper()
{}

ulong LADSPAHelper::process(char *in_data, const ulong size, char **out_data)
{
    LADSPAHost::instance()->applyEffect((qint16 *) in_data, size);
    memcpy(*out_data, in_data, size);
    return size;
}

void LADSPAHelper::configure(quint32 freq, int chan, int res)
{
    LADSPAHost::instance()->configure(freq,chan,res);
    Effect::configure(freq, chan, res);
}
