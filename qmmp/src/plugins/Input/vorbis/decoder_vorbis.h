// Copyright (c) 2000-2001 Brad Hughes <bhughes@trolltech.com>
//
// Use, modification and distribution is allowed without limitation,
// warranty, or liability of any kind.
//

#ifndef DECODER_VORBIS_H
#define DECODER_VORBIS_H

#include <qmmp/decoder.h>

#include <vorbis/vorbisfile.h>


class DecoderVorbis : public Decoder
{
public:
    DecoderVorbis(QObject *, DecoderFactory *, QIODevice *, Output *);
    virtual ~DecoderVorbis();

    // Standard Decoder API
    bool initialize();
    qint64 totalTime();
    int bitrate();

private:
    virtual qint64 readAudio(char *data, qint64 maxSize);
    void seekAudio(qint64 time);

    // helper functions
    void deinit();
    void updateTags();

    // OggVorbis Decoder
    OggVorbis_File oggfile;
    qint64 m_totalTime;
    long len;
    int m_section, m_last_section;
    int m_bitrate;
    bool inited;
};


#endif // __decoder_vorbis_h
