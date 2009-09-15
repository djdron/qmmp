// Copyright (c) 2000-2001 Brad Hughes <bhughes@trolltech.com>
//
// Use, modification and distribution is allowed without limitation,
// warranty, or liability of any kind.
//

#ifndef   DECODER_MAD_H
#define   DECODER_MAD_H

class QIODevice;
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
    DecoderMAD(QIODevice *i);
    virtual ~DecoderMAD();

    // standard decoder API
    bool initialize();
    qint64 totalTime();
    int bitrate();
    qint64 read(char *data, qint64 size);
    void seek(qint64);

private:
    // helper functions
    qint64 madOutput(char *data, qint64 size);    
    bool fillBuffer();
    void deinit();
    bool findHeader();
    bool findXingHeader(struct mad_bitptr, unsigned int);
    uint findID3v2(uchar *data, ulong size);
    bool m_inited, m_eof;
    qint64 m_totalTime;
    int m_channels, m_skip_frames;
    uint m_bitrate;
    long m_freq, m_len;
    qint64 m_output_bytes, m_output_at;

    // file input buffer
    char *m_input_buf;
    qint64 m_input_bytes;

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
