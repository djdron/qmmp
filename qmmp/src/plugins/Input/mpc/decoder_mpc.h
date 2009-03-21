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

#ifndef DECODER_MPC_H
#define DECODER_MPC_H

#ifdef MPC_OLD_API
#include <mpcdec/mpcdec.h>
#else
#include <mpc/mpcdec.h>
#endif

#include <qmmp/decoder.h>

struct mpc_data
{
#ifdef MPC_OLD_API
    mpc_decoder decoder;
#else
    mpc_demux *demuxer;
#endif
    mpc_reader reader;
    mpc_streaminfo info;
};

class DecoderMPC : public Decoder
{
public:
    DecoderMPC(QObject *, DecoderFactory *, QIODevice *, Output *);
    virtual ~DecoderMPC();

    // Standard Decoder API
    bool initialize();
    qint64 totalTime();
    void seek(qint64);
    void stop();

    struct mpc_data *data()
    {
        return m_data;
    }


private:
    // thread run function
    void run();
    struct mpc_data *m_data;
    // helper functions
    void flush(bool = FALSE);
    void deinit();

    bool inited, user_stop;

    // output buffer
    char *output_buf;
    ulong output_bytes, output_at;

    unsigned int bks;
    bool done, m_finish;
    long len, freq, bitrate;
    int chan;
    qint64 output_size;
    qint64 m_totalTime, seekTime;
};


#endif // __decoder_mpc_h
