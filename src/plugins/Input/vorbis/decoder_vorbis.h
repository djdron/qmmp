// Copyright (c) 2000-2001 Brad Hughes <bhughes@trolltech.com>
//
// Use, modification and distribution is allowed without limitation,
// warranty, or liability of any kind.
//

#ifndef __decoder_vorbis_h
#define __decoder_vorbis_h

#include "decoder.h"

#include <vorbis/vorbisfile.h>


class DecoderVorbis : public Decoder
{
public:
    DecoderVorbis(QObject *, DecoderFactory *, QIODevice *, Output *);
    virtual ~DecoderVorbis();

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

    void updateTags();

    bool inited, user_stop;
    int stat;

    // output buffer
    char *output_buf;
    ulong output_bytes, output_at;

    // OggVorbis Decoder
    OggVorbis_File oggfile;

    unsigned int bks;
    bool done, finish;
    long len, freq, bitrate;
    int chan;
    unsigned long output_size;
    double totalTime, seekTime;
};


#endif // __decoder_vorbis_h
