/***************************************************************************
 *   Copyright (C) 2009-2010 by Ilya Kotov                                 *
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

#ifndef REPLAYGAIN_H
#define REPLAYGAIN_H

#include <QtGlobal>
#include <QMap>
#include "qmmpsettings.h"
#include "qmmp.h"

/*! @internal
 * @author Ilya Kotov <forkotov02@hotmail.ru>
 */
class ReplayGain
{
public:
    ReplayGain();

    void setSampleSize(int size);
    void updateSettings(QmmpSettings::ReplayGainMode mode, double preamp,
                        double default_gain, bool clip);
    void setReplayGainInfo(const QMap<Qmmp::ReplayGainKey, double> &info);
    void applyReplayGain(char *data, qint64 size);

private:
    void updateScale();
    int m_sampleSize;
    QMap<Qmmp::ReplayGainKey, double> m_info;
    double m_scale;
    QmmpSettings::ReplayGainMode m_mode;
    double m_preamp;
    double m_default_gain;
    bool m_prevent_clipping;
};

#endif // REPLAYGAIN_H
