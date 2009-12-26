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
#include <taglib/mpegfile.h>
#include <taglib/id3v2tag.h>
#include <taglib/tag.h>
#include <taglib/id3v1tag.h>
#include <taglib/id3v2header.h>
#include "replaygainreader.h"

ReplayGainReader::ReplayGainReader(const QString &path)
{
    TagLib::MPEG::File fileRef(path.toLocal8Bit ().constData());
    if(fileRef.APETag())
        readAPE(fileRef.APETag());
}

QMap <Qmmp::ReplayGainKey, double> ReplayGainReader::replayGainInfo() const
{
    return m_values;
}

void ReplayGainReader::readAPE(TagLib::APE::Tag *tag)
{
    TagLib::APE::ItemListMap items = tag->itemListMap();
    if (items.contains("REPLAYGAIN_TRACK_GAIN")) 
        setValue(Qmmp::REPLAYGAIN_TRACK_GAIN,TStringToQString(items["REPLAYGAIN_TRACK_GAIN"].values()[0]));
    if (items.contains("REPLAYGAIN_TRACK_PEAK"))
        setValue(Qmmp::REPLAYGAIN_TRACK_PEAK,TStringToQString(items["REPLAYGAIN_TRACK_PEAK"].values()[0]));
    if (items.contains("REPLAYGAIN_ALBUM_GAIN"))
        setValue(Qmmp::REPLAYGAIN_ALBUM_GAIN,TStringToQString(items["REPLAYGAIN_ALBUM_GAIN"].values()[0]));
    if (items.contains("REPLAYGAIN_ALBUM_PEAK"))
        setValue(Qmmp::REPLAYGAIN_ALBUM_PEAK,TStringToQString(items["REPLAYGAIN_ALBUM_PEAK"].values()[0]));
}

void ReplayGainReader::setValue(Qmmp::ReplayGainKey key, QString value)
{
    value.remove(" dB");
    if(value.isEmpty())
        return;
    bool ok;
    double v = value.toDouble(&ok);
    if(ok)
        m_values[key] = v;
}
