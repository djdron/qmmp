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
#ifndef BS2BPLUGIN_H
#define BS2BPLUGIN_H

#include <QMutex>
#include <bs2b/bs2b.h>
#include <qmmp/effect.h>

/**
    @author Ilya Kotov <forkotov02@hotmail.ru>
*/

class Bs2bPlugin : public Effect
{
    Q_OBJECT
public:
    Bs2bPlugin(QObject *parent = 0);

    virtual ~Bs2bPlugin();

    ulong process(char *in_data, const ulong size, char **out_data);
    void configure(quint32 freq, int chan, int res);
    void setCrossfeedLevel(uint32_t level);
    static Bs2bPlugin* instance();

private:
    t_bs2bdp m_bs2b_handler;
    QMutex m_mutex;
    static Bs2bPlugin *m_instance;
};

#endif
