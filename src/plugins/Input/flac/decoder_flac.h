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

#ifndef DECODER_FLAC_H
#define DECODER_FLAC_H

#include <qmmp/decoder.h>

#include <FLAC/all.h>

#define MAX_SUPPORTED_CHANNELS      2

#define SAMPLES_PER_WRITE     512
#define SAMPLE_BUFFER_SIZE ((FLAC__MAX_BLOCK_SIZE + SAMPLES_PER_WRITE) * MAX_SUPPORTED_CHANNELS * (32/8))

struct flac_data
{
    //FLAC__SeekableStreamDecoder *decoder;
    FLAC__StreamDecoder *decoder;
    struct io_stream *stream;
    int bitrate;
    int abort; /* abort playing (due to an error) */

    unsigned length;
    unsigned total_samples;

    FLAC__byte sample_buffer[SAMPLE_BUFFER_SIZE];
    unsigned sample_buffer_fill;

    /* sound parameters */
    unsigned bits_per_sample;
    unsigned sample_rate;
    unsigned channels;

    FLAC__uint64 last_decode_position;

    int ok; /* was this stream successfully opened? */
    //struct decoder_error error;
};

class DecoderFLAC : public Decoder
{
public:
    DecoderFLAC(QObject *, DecoderFactory *, QIODevice *, Output *);
    virtual ~DecoderFLAC();

    // Standard Decoder API
    bool initialize();
    double lengthInSeconds();
    void seek(double);
    void stop();

    // Equalizer
    bool isEQSupported() const
    {
        return FALSE;
    }
    void setEQEnabled(bool)
    {
        ;
    }
    void setEQGain(int)
    {
        ;
    }
    void setEQBands(int[10])
    {
        ;
    }

    struct flac_data *data()
    {
        return m_data;
    }


private:
    // thread run function
    void run();
    struct flac_data *m_data;
    // helper functions
    void flush(bool = FALSE);
    void deinit();

    bool inited, user_stop;
    int stat;

    // output buffer
    char *output_buf;
    ulong output_bytes, output_at;

    // FLAC Decoder
    //FLAC__SeekableStreamDecoder *m_flacDecoder;
    FLAC__StreamDecoder *m_flacDecoder;

    unsigned int bks;
    bool done, finish;
    long len, freq, bitrate;
    int chan;
    unsigned long output_size;
    double totalTime, seekTime;
};


#endif // __decoder_flac_h
