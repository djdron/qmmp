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

#include <math.h>
#include "replaygain.h"

ReplayGain::ReplayGain()
{
    m_sampleSize = 2;
    m_scale = 1.0;
}

void ReplayGain::setSampleSize(int size)
{
    m_sampleSize = size;
    updateScale();
}

void ReplayGain::setReplayGainInfo(const QMap<Qmmp::ReplayGainKey, double> &info)
{
    m_info = info;
    updateScale();
    if(m_settings.value(AudioSettings::REPLAYGAIN_MODE).toInt() != AudioSettings::REPLAYGAIN_DISABLED)
    {
        qDebug("ReplayGain: track: gain=%f dB, peak=%f; album: gain=%f dB, peak=%f",
               m_info[Qmmp::REPLAYGAIN_TRACK_GAIN],
               m_info[Qmmp::REPLAYGAIN_TRACK_PEAK],
               m_info[Qmmp::REPLAYGAIN_ALBUM_GAIN],
               m_info[Qmmp::REPLAYGAIN_ALBUM_PEAK]);
        qDebug("ReplayGain: scale=%f", m_scale);
    }
    else
        qDebug("ReplayGain: disabled");
}

void ReplayGain::setAudioSettings(const AudioSettings &settings)
{
    m_settings = settings;
    setReplayGainInfo(m_info);
}

void ReplayGain::applyReplayGain(char *data, qint64 size)
{
    if( m_scale == 1.0 ||
        m_settings.value(AudioSettings::REPLAYGAIN_MODE).toInt() == AudioSettings::REPLAYGAIN_DISABLED)
        return;
    size = size/m_sampleSize;
    if(m_sampleSize == 2)
    {
        for (qint64 i = 0; i < size; i++)
            ((short*)data)[i]*= m_scale;

    }
    else if(m_sampleSize == 1)
    {
        for (qint64 i = 0; i < size; i++)
            ((char*)data)[i]*= m_scale;
    }
    else if(m_sampleSize == 4)
    {
        for (qint64 i = 0; i < size; i++)
           ((qint32*)data)[i]*= m_scale;
    }
}

void ReplayGain::updateScale()
{
    double peak = 0.0;
    m_scale = 1.0;
    switch(m_settings.value(AudioSettings::REPLAYGAIN_MODE).toInt())
    {
    case AudioSettings::REPLAYGAIN_TRACK:
        m_scale = pow(10.0, m_info[Qmmp::REPLAYGAIN_TRACK_GAIN]/20);
        peak = m_info[Qmmp::REPLAYGAIN_TRACK_PEAK];
        break;
    case AudioSettings::REPLAYGAIN_ALBUM:
        m_scale = pow(10.0, m_info[Qmmp::REPLAYGAIN_ALBUM_GAIN]/20);
        peak = m_info[Qmmp::REPLAYGAIN_ALBUM_PEAK];
        break;
    case AudioSettings::REPLAYGAIN_DISABLED:
        m_scale = 1.0;
        return;
    }
    if(m_scale == 1.0)
        m_scale = pow(10.0, m_settings.value(AudioSettings::REPLAYGAIN_DEFAULT_GAIN).toDouble()/20);
    m_scale *= pow(10.0, m_settings.value(AudioSettings::REPLAYGAIN_PREAMP).toDouble()/20);
    if(peak > 0.0 && m_settings.value(AudioSettings::REPLAYGAIN_PREVENT_CLIPPING).toBool())
        m_scale = m_scale*peak > 1.0 ? 1.0 / peak : m_scale;
    m_scale = qMin(m_scale, 5.6234); // +15 dB
    m_scale = qMax(m_scale, 0.1778);  // -15 dB
}
