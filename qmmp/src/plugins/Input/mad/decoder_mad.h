// Copyright (c) 2000-2001 Brad Hughes <bhughes@trolltech.com>
//
// Use, modification and distribution is allowed without limitation,
// warranty, or liability of any kind.
//

#ifndef   __decoder_mad_h
#define   __decoder_mad_h

class DecoderMAD;

#include "decoder.h"
#include "decodermadfactory.h"

extern "C" {
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
    double lengthInSeconds();
    void seek(double);
    void stop();

    // Equalizer
    //bool isEQSupported() const { return TRUE; }
    //void setEQEnabled(bool);
    //void setEQ(const EqPreset &);

    static const int maxDecodeRetries;
    static const int maxFrameSize;
    static const int maxFrameCheck;
    static const int initialFrameSize;


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
    void calcLength(struct mad_header *);

    bool inited, user_stop, done, finish, derror, eof, useeq;
    double totalTime, seekTime;
    int channels;
    long bitrate, freq, len;
    unsigned int bks;
    mad_fixed_t eqbands[32];

    // file input buffer
    char *input_buf;
    unsigned long input_bytes;

    // output buffer
    char *output_buf;
    unsigned long output_bytes, output_at, output_size;

    // MAD decoder
    struct {
	int flags;
	unsigned long frames;
	unsigned long bytes;
	unsigned char toc[100];
	long scale;
    } xing;

    enum {
	XING_FRAMES = 0x0001,
	XING_BYTES  = 0x0002,
	XING_TOC    = 0x0004,
	XING_SCALE  = 0x0008
    };

    struct mad_stream stream;
    struct mad_frame frame;
    struct mad_synth synth;
};


#endif // __decoder_mad_h
