/***************************************************************************
 *   Copyright (C) 2006-2009 by Ilya Kotov                                 *
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

#ifndef DECODER_AAC_H
#define DECODER_AAC_H


#include <neaacdec.h>

#include <qmmp/decoder.h>


struct aac_data
{
    NeAACDecHandle handle;
};

class DecoderAAC : public Decoder
{
public:
    DecoderAAC(QObject *, DecoderFactory *, QIODevice *, Output *);
    virtual ~DecoderAAC();


    struct aac_data *data()
    {
        return m_data;
    }
      // Standard Decoder API
    bool initialize();
    qint64 totalTime();
    int bitrate();

private:
    qint64 readAudio(char *audio, qint64 maxSize);
    void seekAudio(qint64 time);
    struct aac_data *m_data;

    char *m_input_buf;
    int m_bitrate;
    ulong  m_input_at, m_output_at;
    double m_frameSize;
    qint64 m_totalTime;
};


#endif // __decoder_aac_h
