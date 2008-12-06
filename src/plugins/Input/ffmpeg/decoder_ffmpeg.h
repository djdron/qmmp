/***************************************************************************
 *   Copyright (C) 2006-2008 by Ilya Kotov                                 *
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

#if defined HAVE_FFMPEG_AVFORMAT_H
#include <ffmpeg/avformat.h>
#elif defined HAVE_LIBAVFORMAT_AVFORMAT_H
#include <libavformat/avformat.h>
#else
#include <avformat.h>
#endif


#if defined HAVE_FFMPEG_AVCODEC_H
#include <ffmpeg/avcodec.h>
#elif defined HAVE_LIBAVCODEC_AVCODEC_H
#include <libavcodec/avcodec.h>
#else
#include <avcodec.h>
#endif
}
#include <qmmp/decoder.h>

class DecoderFFmpeg : public Decoder
{
public:
    DecoderFFmpeg(QObject *, DecoderFactory *, Output *, const QString &);
    virtual ~DecoderFFmpeg();

    // Standard Decoder API
    bool initialize();
    qint64 lengthInSeconds();
    void seek(qint64);
    void stop();

private:
    // thread run function
    void run();
    // helper functions
    void flush(bool = FALSE);
    void deinit();
    void ffmpeg_out(int size);
    bool inited, user_stop;

    // output buffer
    char *output_buf;
    qint64 output_bytes, output_at;

    AVFormatContext *ic;
    AVCodecContext *c;
    uint wma_st_buff, wma_idx, wma_idx2;
    uint8_t *wma_outbuf;

    unsigned int bks;
    bool done, m_finish;
    long freq, bitrate;
    int chan;
    qint64 output_size;
    qint64 totalTime, seekTime;
    QString m_path;
};


#endif // __decoder_ffmpeg_h
