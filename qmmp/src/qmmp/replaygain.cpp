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
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#include <math.h>
#include "buffer.h"
#include "replaygain_p.h"

ReplayGain::ReplayGain()
{
    m_scale = 1.0;
    m_mode = QmmpSettings::REPLAYGAIN_DISABLED;
    m_format = Qmmp::PCM_UNKNOWM;
    m_preamp = 0.0;
    m_default_gain = 0.0;
    m_prebuf = 0;
    m_prevent_clipping = false;
}

ReplayGain::~ReplayGain()
{
    if(m_prebuf)
        delete [] m_prebuf;
}

void ReplayGain::configure(const AudioParameters &p)
{
    m_format = p.format();
    if(m_prebuf)
        delete [] m_prebuf;
    m_prebuf = new float[QMMP_BLOCK_FRAMES * p.channels() * 4];
}

void ReplayGain::setReplayGainInfo(const QMap<Qmmp::ReplayGainKey, double> &info)
{
    m_info = info;
    updateScale();
    if(m_mode != QmmpSettings::REPLAYGAIN_DISABLED)
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

qint64 ReplayGain::read(Decoder *decoder, char *data, qint64 size)
{
    if(m_mode == QmmpSettings::REPLAYGAIN_DISABLED || m_scale == 1.0)
        return decoder->read(data, size);

    qint64 samples = 0;

    switch (m_format)
    {
    case Qmmp::PCM_S8:
        samples = decoder->read(m_prebuf, size);
        break;
    case Qmmp::PCM_S16LE:
        samples = decoder->read(m_prebuf, size >> 1);
        break;
    case Qmmp::PCM_S24LE:
        samples = decoder->read(m_prebuf, size >> 2);
        break;
    case Qmmp::PCM_S32LE:
        samples = decoder->read(m_prebuf, size >> 2);
        break;
    default:
        break;
    }

    for(qint64 i = 0; i < samples; ++i)
    {
        m_prebuf[i] *= m_scale;
        m_prebuf[i] = qBound((float)-1.0, m_prebuf[i], (float)1.0);

        switch (m_format)
        {
        case Qmmp::PCM_S8:
            ((char*)data)[i] = m_prebuf[i] * 127.0;
            break;
        case Qmmp::PCM_S16LE:
            ((short*)data)[i] = m_prebuf[i] * 32767.0;
            break;
        case Qmmp::PCM_S24LE:
            ((qint32*)data)[i] = m_prebuf[i] * 32767.0;
            break;
        case Qmmp::PCM_S32LE:
            ((qint32*)data)[i] = m_prebuf[i] * 32767.0;
            break;
        default:
            break;
        }
    }
}

void ReplayGain::updateSettings(QmmpSettings::ReplayGainMode mode, double preamp,
                                double default_gain, bool clip)
{
    m_mode = mode;
    m_preamp = preamp;
    m_default_gain = default_gain;
    m_prevent_clipping = clip;
    setReplayGainInfo(m_info);
}

void ReplayGain::updateScale()
{
    double peak = 0.0;
    m_scale = 1.0;
    switch(m_mode)
    {
    case QmmpSettings::REPLAYGAIN_TRACK:
        m_scale = pow(10.0, m_info[Qmmp::REPLAYGAIN_TRACK_GAIN]/20);
        peak = m_info[Qmmp::REPLAYGAIN_TRACK_PEAK];
        break;
    case QmmpSettings::REPLAYGAIN_ALBUM:
        m_scale = pow(10.0, m_info[Qmmp::REPLAYGAIN_ALBUM_GAIN]/20);
        peak = m_info[Qmmp::REPLAYGAIN_ALBUM_PEAK];
        break;
    case QmmpSettings::REPLAYGAIN_DISABLED:
        m_scale = 1.0;
        return;
    }
    if(m_scale == 1.0)
        m_scale = pow(10.0, m_default_gain/20);
    m_scale *= pow(10.0, m_preamp/20);
    if(peak > 0.0 && m_prevent_clipping)
        m_scale = m_scale*peak > 1.0 ? 1.0 / peak : m_scale;
    m_scale = qMin(m_scale, 5.6234); // +15 dB
    m_scale = qMax(m_scale, 0.1778);  // -15 dB*/
}
