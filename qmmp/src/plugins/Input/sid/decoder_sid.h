/***************************************************************************
 *   Copyright (C) 2013 by Ilya Kotov                                      *
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

#ifndef DECODER_SID_H
#define DECODER_SID_H

#include <qmmp/decoder.h>
#include <sidplayfp/sidplayfp.h>
#include <sidplayfp/SidTune.h>
#include <sidplayfp/sidbuilder.h>
#include <sidplayfp/SidConfig.h>
#include <sidplayfp/builders/residfp.h>
#include <sidplayfp/SidInfo.h>

class GmeHelper;

/**
   @author Ilya Kotov <forkotov02@hotmail.ru>
*/
class DecoderSID : public Decoder
{
public:
    DecoderSID(QIODevice *input);
    virtual ~DecoderSID();

    // Standard Decoder API
    bool initialize();
    qint64 totalTime();
    int bitrate();
    qint64 read(char *data, qint64 size);
    void seek(qint64);

private:
    qint64 m_totalTime;
    QString m_path;
    sidplayfp *m_player;
};

#endif // DECODER_SID_H
