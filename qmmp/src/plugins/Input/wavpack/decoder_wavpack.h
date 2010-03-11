/***************************************************************************
 *   Copyright (C) 2008-2009 by Ilya Kotov                                 *
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

#ifndef DECODER_WAVPACK_H
#define DECODER_WAVPACK_H

extern "C"{
#include <wavpack/wavpack.h>
}
#include <qmmp/decoder.h>

class CUEParser;

class DecoderWavPack : public Decoder
{
public:
    DecoderWavPack(const QString &);
    virtual ~DecoderWavPack();

    // Standard Decoder API
    bool initialize();
    qint64 totalTime();
    int bitrate();
    qint64 read(char *data, qint64 maxSize);
    void seek(qint64 time);
    const QString nextURL();
    void next();

private:
    // helper functions
    void deinit();
    qint64 wavpack_decode(char *data, qint64 size);
    WavpackContext *m_context;    
    int32_t *m_output_buf; // output buffer
    int m_chan;
    quint32 m_freq;
    qint64 m_totalTime;
    qint64 length_in_bytes;
    qint64 m_totalBytes;
    qint64 m_offset;
    qint64 m_length;
    QString m_path;
    CUEParser *m_parser;
    int m_track;
    int m_bps;
    char *m_buf; //buffer for remainig data
    qint64 m_buf_size;
    qint64 m_sz; //sample size

};

#endif // DECODER_WAVPACK_H
