/***************************************************************************
 *   Copyright (C) 2011-2013 by Ilya Kotov                                 *
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

#ifndef DECODER_MPG123_H
#define DECODER_MPG123_H

class QIODevice;

#include <mpg123.h>
#include <qmmp/decoder.h>
#include "decodermpg123factory.h"

class DecoderMPG123 : public Decoder
{
public:
    DecoderMPG123(const QString &url, QIODevice *i);
    virtual ~DecoderMPG123();

    // standard decoder API
    bool initialize();
    qint64 totalTime();
    int bitrate();
    qint64 read(unsigned char *data, qint64 size);
    qint64 read(float *data, qint64 samples);
    void seek(qint64);

private:
    void cleanup(mpg123_handle *handle);
    void updateMPG123Format(int encoding);
    void setMPG123Format(int encoding);
    mpg123_handle *m_handle;
    mpg123_frameinfo m_frame_info;
    QString m_url;
    qint64 m_totalTime;
    long m_rate;
    int m_mpg123_encoding;
};


#endif // DECODER_MPG123_H
