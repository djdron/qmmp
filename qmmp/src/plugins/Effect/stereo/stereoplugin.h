/***************************************************************************
 *   Copyright (C) 2011 by Ilya Kotov                                      *
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
#ifndef STEREOPLUGIN_H
#define STEREOPLUGIN_H

#include <QMutex>
#include <qmmp/effect.h>

/**
    @author Ilya Kotov <forkotov02@hotmail.ru>
*/
class StereoPlugin : public Effect
{
public:
    StereoPlugin();

    virtual ~StereoPlugin();

    void applyEffect(Buffer *b);
    void configure(quint32 freq, int chan, Qmmp::AudioFormat format);
    void setIntensity(double level);
    static StereoPlugin* instance();

private:
    int m_chan;
    QMutex m_mutex;
    double m_avg, m_ldiff, m_rdiff, m_tmp, m_mul;
    static StereoPlugin *m_instance;
};

#endif
