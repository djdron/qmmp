// Copyright (c) 2000-2001 Brad Hughes <bhughes@trolltech.com>
//
// Use, modification and distribution is allowed without limitation,
// warranty, or liability of any kind.
//

#ifndef   DECODER_MAD_H
#define   DECODER_MAD_H

class DecoderMAD;

#include <qmmp/decoder.h>
#include "decodermadfactory.h"

extern "C"
{
#include <mad.h>
}


class DecoderMAD : public Decoder
{
public:
    DecoderMAD(QObject *parent = 0, DecoderFactory *d = 0,
               QIODevice *i = 0, Output *o = 0);
    virtual ~DecoderMAD();

    // standard decoder API
    bool initialize();
    qint64 lengthInSeconds();
    void seek(qint64);
    void stop();

private:
    // thread run function
    void run();

    enum mad_flow madOutput();
    enum mad_flow madError(struct mad_stream *, struct mad_frame *);

    // helper functions
    void flush(bool = FALSE);
    void deinit();
    bool findHeader();
    bool findXingHeader(struct mad_bitptr, unsigned int);
    uint findID3v2(uchar *data, uint size);
    bool inited, user_stop, done, m_finish, derror, eof, useeq;
    qint64 totalTime, seekTime;
    int channels;
    unsigned long bitrate;
    long freq, len;
    unsigned int bks;
    mad_fixed_t eqbands[32];

    // file input buffer
    char *input_buf;
    unsigned long input_bytes;

    // output buffer
    char *output_buf;
    unsigned long output_bytes, output_at, output_size;

    // MAD decoder
    struct
    {
        int flags;
        unsigned long frames;
        unsigned long bytes;
        unsigned char toc[100];
        long scale;
    } xing;

    enum
    {
        XING_FRAMES = 0x0001,
        XING_BYTES  = 0x0002,
        XING_TOC    = 0x0004,
        XING_SCALE  = 0x0008
    };

    struct mad_stream stream;
    struct mad_frame frame;
    struct mad_synth synth;
};


#endif // DECODER_MAD_H
