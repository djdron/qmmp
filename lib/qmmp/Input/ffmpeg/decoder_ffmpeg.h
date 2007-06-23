/***************************************************************************
 *   Copyright (C) 2006 by Ilya Kotov                                      *
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

#ifndef __decoder_ffmeg_h
#define __decoder_ffmeg_h

extern "C"{
#include <ffmpeg/avformat.h>
#include <ffmpeg/avcodec.h>
}
#include "decoder.h"

class DecoderFFmpeg : public Decoder
{
public:
    DecoderFFmpeg(QObject *, DecoderFactory *, QIODevice *, Output *);
    virtual ~DecoderFFmpeg();

    // Standard Decoder API
    bool initialize();
    double lengthInSeconds();
    void seek(double);
    void stop();

    // Equalizer
    bool isEQSupported() const { return FALSE; }
    void setEQEnabled(bool) { ; }
    void setEQGain(int) { ; }
    void setEQBands(int[10]) { ; }


private:
    // thread run function
    void run();
    // helper functions
    void flush(bool = FALSE);
    void deinit();
    void ffmpeg_out(int size);

    bool inited, user_stop;
    int stat;

    // output buffer
    char *output_buf;
    ulong output_bytes, output_at;

    AVFormatContext *ic;
    AVCodecContext *c;
    int wma_st_buff, wma_idx, wma_idx2;
    uint8_t *wma_outbuf;

    unsigned int bks;
    bool done, finish;
    long len, freq, bitrate;
    int chan;
    unsigned long output_size;
    double totalTime, seekTime;
};


#endif // __decoder_mpc_h
