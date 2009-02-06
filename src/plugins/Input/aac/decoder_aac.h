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

    // Standard Decoder API
    bool initialize();
    qint64 lengthInSeconds();
    void seek(qint64);
    void stop();

    struct aac_data *data()
    {
        return m_data;
    }


private:
    // thread run function
    void run();
    struct aac_data *m_data;
    // helper functions
    void flush(bool = FALSE);
    void deinit();
    qint64 aac_decode(void *out);

    bool m_inited, m_user_stop;

    // output buffer
    char *m_output_buf, *m_input_buf;
    void *m_prebuf2;
    unsigned long m_output_bytes;

#ifdef FAAD_MODIFIED
    uint32_t m_freq;
    uint8_t m_chan;
#else
    unsigned long m_freq;
    unsigned char m_chan;
#endif

    unsigned int m_bks;
    bool m_done, m_finish;
    unsigned long m_len, m_bitrate, m_input_at, m_output_at;
    unsigned long m_output_size;
    double m_frameSize; //frame size in bytes
    qint64 m_totalTime, m_seekTime;
};


#endif // __decoder_aac_h
