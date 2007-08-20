// Copyright (c) 2000-2001 Brad Hughes <bhughes@trolltech.com>
//
// Use, modification and distribution is allowed without limitation,
// warranty, or liability of any kind.
//

#include "decoder_vorbis.h"
#include "constants.h"
#include "buffer.h"
#include "output.h"
#include "recycler.h"
#include "filetag.h"

#include <QObject>
#include <QIODevice>


// ic functions for OggVorbis

static size_t oggread (void *buf, size_t size, size_t nmemb, void *src)
{
    if (! src) return 0;

    DecoderVorbis *dogg = (DecoderVorbis *) src;
    int len = dogg->input()->read((char *) buf, (size * nmemb));
    return len / size;
}


static int oggseek(void *src, int64_t offset, int whence)
{
    DecoderVorbis *dogg = (DecoderVorbis *) src;

    if ( dogg->input()->isSequential ())
        return -1;

    long start = 0;
    switch (whence)
    {
    case SEEK_END:
        start = dogg->input()->size();
        break;

    case SEEK_CUR:
        start = dogg->input()->pos();
        break;

    case SEEK_SET:
    default:
        start = 0;
    }

    if (dogg->input()->seek(start + offset))
        return 0;
    return -1;
}


static int oggclose(void *src)
{
    DecoderVorbis *dogg = (DecoderVorbis *) src;
    dogg->input()->close();
    return 0;
}


static long oggtell(void *src)
{
    DecoderVorbis *dogg = (DecoderVorbis *) src;
    long t = dogg->input()->pos();
    return t;
}


// Decoder class

DecoderVorbis::DecoderVorbis(QObject *parent, DecoderFactory *d, QIODevice *i, Output *o)
        : Decoder(parent, d, i, o)
{
    inited = FALSE;
    user_stop = FALSE;
    stat = 0;
    output_buf = 0;
    output_bytes = 0;
    output_at = 0;
    bks = 0;
    done = FALSE;
    finish = FALSE;
    len = 0;
    freq = 0;
    bitrate = 0;
    seekTime = -1.0;
    totalTime = 0.0;
    chan = 0;
    output_size = 0;
}


DecoderVorbis::~DecoderVorbis()
{
    deinit();

    if (output_buf)
        delete [] output_buf;
    output_buf = 0;
}


void DecoderVorbis::stop()
{
    user_stop = TRUE;
}


void DecoderVorbis::flush(bool final)
{
    ulong min = final ? 0 : bks;

    while ((! done && ! finish) && output_bytes > min)
    {
        output()->recycler()->mutex()->lock ();

        while ((! done && ! finish) && output()->recycler()->full())
        {
            mutex()->unlock();

            output()->recycler()->cond()->wait(output()->recycler()->mutex());

            mutex()->lock ();
            done = user_stop;
        }

        if (user_stop || finish)
        {
            inited = FALSE;
            done = TRUE;
        }
        else
        {
            /*ulong sz = output_bytes < bks ? output_bytes : bks;
            Buffer *b = output()->recycler()->get();

            memcpy(b->data, output_buf, sz);
            if (sz != bks) memset(b->data + sz, 0, bks - sz);

            b->nbytes = bks;
            b->rate = bitrate;
            output_size += b->nbytes;
            output()->recycler()->add();

            output_bytes -= sz;
            memmove(output_buf, output_buf + sz, output_bytes);*/
            output_bytes -= produceSound(output_buf, output_bytes, bitrate, chan);
            output_size += bks;
            output_at = output_bytes;
        }

        if (output()->recycler()->full())
        {
            output()->recycler()->cond()->wakeOne();
        }

        output()->recycler()->mutex()->unlock();
    }
}


bool DecoderVorbis::initialize()
{
    qDebug("DecoderVorbis: initialize");
    bks = blockSize();

    inited = user_stop = done = finish = FALSE;
    len = freq = bitrate = 0;
    stat = chan = 0;
    output_size = 0;
    seekTime = -1.0;
    totalTime = 0.0;
    if (! input())
    {
        qDebug("DecoderVorbis: cannot initialize.  No input");

        return FALSE;
    }

    if (! output_buf)
        output_buf = new char[globalBufferSize];
    output_at = 0;
    output_bytes = 0;

    if (! input()->isOpen())
    {
        if (! input()->open(QIODevice::ReadOnly))
        {
            qWarning(qPrintable("DecoderVorbis: failed to open input. " +
                                input()->errorString () + "."));
            return FALSE;
        }
    }

    ov_callbacks oggcb =
        {
            oggread,
            oggseek,
            oggclose,
            oggtell
        };
    if (ov_open_callbacks(this, &oggfile, NULL, 0, oggcb) < 0)
    {
        qWarning("DecoderVorbis: cannot open stream");

        return FALSE;
    }

    freq = 0;
    bitrate = ov_bitrate(&oggfile, -1) / 1000;
    chan = 0;

    totalTime = long(ov_time_total(&oggfile, 0));
    totalTime = totalTime < 0 ? 0 : totalTime;

    vorbis_info *ogginfo = ov_info(&oggfile, -1);
    if (ogginfo)
    {
        freq = ogginfo->rate;
        chan = ogginfo->channels;
    }

    if (output())
        output()->configure(freq, chan, 16, bitrate);

    inited = TRUE;
    return TRUE;
}


double DecoderVorbis::lengthInSeconds()
{
    if (! inited)
        return 0;

    return totalTime;
}


void DecoderVorbis::seek(double pos)
{
    seekTime = pos;
}


void DecoderVorbis::deinit()
{
    if (inited)
        ov_clear(&oggfile);
    inited = user_stop = done = finish = FALSE;
    len = freq = bitrate = 0;
    stat = chan = 0;
    output_size = 0;
}

void DecoderVorbis::updateTags()
{
    int i;
    vorbis_comment *comments;

    FileTag tag;
    //TODO add all tags
    comments = ov_comment (&oggfile, -1);
    for (i = 0; i < comments->comments; i++)
    {
        if (!strncasecmp(comments->user_comments[i], "title=",
                         strlen ("title=")))
            tag.setValue(FileTag::TITLE, QString::fromUtf8(comments->user_comments[i]
                         + strlen ("title=")));
        else if (!strncasecmp(comments->user_comments[i],
                              "artist=", strlen ("artist=")))
            tag.setValue(FileTag::ARTIST,
                         QString::fromUtf8(comments->user_comments[i]
                                           + strlen ("artist=")));
        else if (!strncasecmp(comments->user_comments[i],
                              "album=", strlen ("album=")))
            tag.setValue(FileTag::ALBUM,
                         QString::fromUtf8(comments->user_comments[i]
                                           + strlen ("album=")));
        else if (!strncasecmp(comments->user_comments[i],
                              "comment=", strlen ("comment=")))
            tag.setValue(FileTag::COMMENT,
                         QString::fromUtf8(comments->user_comments[i]
                                           + strlen ("comment=")));
        else if (!strncasecmp(comments->user_comments[i],
                              "genre=", strlen ("genre=")))
            tag.setValue(FileTag::GENRE, QString::fromUtf8 (comments->user_comments[i]
                         + strlen ("genre=")));
        else if (!strncasecmp(comments->user_comments[i],
                              "tracknumber=",
                              strlen ("tracknumber=")))
            tag.setValue(FileTag::TRACK, atoi (comments->user_comments[i]
                                               + strlen ("tracknumber=")));
        else if (!strncasecmp(comments->user_comments[i],
                              "track=", strlen ("track=")))
            tag.setValue(FileTag::TRACK, atoi (comments->user_comments[i]
                                               + strlen ("track=")));
        else if (!strncasecmp(comments->user_comments[i],
                              "date=", strlen ("date=")))
            tag.setValue(FileTag::YEAR, atoi (comments->user_comments[i]
                                               + strlen ("date=")));

    }
    tag.setValue(FileTag::LENGTH, uint(totalTime));
    dispatch(tag);
}

void DecoderVorbis::run()
{
    mutex()->lock ();

    if (! inited)
    {
        mutex()->unlock();

        return;
    }

    //stat = DecoderEvent::Decoding;
    stat = DecoderState::Decoding;
    mutex()->unlock();

    {
        //DecoderEvent e((DecoderEvent::Type) stat);
        //dispatch(e);
        //DecoderStatus st ((DecoderStatus::Type) stat);
        dispatch(DecoderState ((DecoderState::Type) stat));

        //emit statusChanged(stat);
    }

    int section = 0;
    int last_section = -1;

    while (! done && ! finish)
    {
        mutex()->lock ();
        // decode

        if (seekTime >= 0.0)
        {
            ov_time_seek(&oggfile, double(seekTime));
            seekTime = -1.0;

            output_size = long(ov_time_tell(&oggfile)) * long(freq * chan * 2);
        }
        len = -1;
        while (len < 0)
        {
            len = ov_read(&oggfile, (char *) (output_buf + output_at), bks, 0, 2, 1,
                          &section);
        }
        if (section != last_section)
            updateTags();
        last_section = section;

        if (len > 0)
        {
            bitrate = ov_bitrate_instant(&oggfile) / 1000;

            output_at += len;
            output_bytes += len;

            if (output())
                flush();
        }
        else if (len == 0)
        {
            flush(TRUE);

            if (output())
            {
                output()->recycler()->mutex()->lock ();
                // end of stream
                while (! output()->recycler()->empty() && ! user_stop)
                {
                    output()->recycler()->cond()->wakeOne();
                    mutex()->unlock();
                    output()->recycler()->cond()->wait(output()->recycler()->mutex());
                    mutex()->lock ();
                }
                output()->recycler()->mutex()->unlock();
            }

            done = TRUE;
            if (! user_stop)
            {
                finish = TRUE;
            }
        }
        else
        {
            // error in read
            error("DecoderVorbis: Error while decoding stream, File appears to be "
                  "corrupted");

            finish = TRUE;
        }

        mutex()->unlock();
    }

    mutex()->lock ();

    if (finish)
        stat = DecoderState::Finished;
    else if (user_stop)
        stat = DecoderState::Stopped;

    mutex()->unlock();

    {
        /*DecoderEvent e((DecoderEvent::Type) stat);
        dispatch(e);*/
        //DecoderStatus st ((DecoderStatus::Type) stat);
        //emit statusChanged(st);
        dispatch(DecoderState ((DecoderState::Type) stat));
    }

    deinit();
}
