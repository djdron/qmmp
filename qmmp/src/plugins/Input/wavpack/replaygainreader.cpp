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

#include <QtGlobal>
#include "replaygainreader.h"

ReplayGainReader::ReplayGainReader(const QString &path)
{
    if(path.contains("://"))
    {
        QString p = QUrl(path).path();
        p.replace(QString(QUrl::toPercentEncoding("#")), "#");
        p.replace(QString(QUrl::toPercentEncoding("?")), "?");
        p.replace(QString(QUrl::toPercentEncoding("%")), "%");
        m_path = p;
    }
    else
        m_path = path;

    char err[80];
    m_ctx = WavpackOpenFileInput (m_path.toLocal8Bit(), err,
                                  OPEN_WVC | OPEN_EDIT_TAGS, 0);
    if (!m_ctx)
    {
        qWarning("WavPackMetaDataModel: error: %s", err);
        return;
    }
    readAPE();
}

ReplayGainReader::~ReplayGainReader()
{
     if(m_ctx)
        WavpackCloseFile (m_ctx);
}

QMap <Qmmp::ReplayGainKey, double> ReplayGainReader::replayGainInfo() const
{
    return m_values;
}

void ReplayGainReader::readAPE()
{
    char value[200];
    WavpackGetTagItem (m_ctx, "REPLAYGAIN_TRACK_GAIN", value, sizeof(value));
    setValue(Qmmp::REPLAYGAIN_TRACK_GAIN, value);
    WavpackGetTagItem (m_ctx, "REPLAYGAIN_TRACK_PEAK", value, sizeof(value));
    setValue(Qmmp::REPLAYGAIN_TRACK_PEAK, value);
    WavpackGetTagItem (m_ctx, "REPLAYGAIN_ALBUM_GAIN", value, sizeof(value));
    setValue(Qmmp::REPLAYGAIN_ALBUM_GAIN, value);
    WavpackGetTagItem (m_ctx, "REPLAYGAIN_ALBUM_PEAK", value, sizeof(value));
    setValue(Qmmp::REPLAYGAIN_ALBUM_PEAK, value);
}

void ReplayGainReader::setValue(Qmmp::ReplayGainKey key, QString value)
{
    value.remove(" dB");
    value = value.trimmed();
    if(value.isEmpty())
        return;
    bool ok;
    double v = value.toDouble(&ok);
    if(ok)
        m_values[key] = v;
}
