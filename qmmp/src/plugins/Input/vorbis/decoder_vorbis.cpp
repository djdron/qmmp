// Copyright (c) 2000-2001 Brad Hughes <bhughes@trolltech.com>
//
// Use, modification and distribution is allowed without limitation,
// warranty, or liability of any kind.
//


#include <qmmp/buffer.h>
#include <qmmp/output.h>
#include <qmmp/recycler.h>
#include <qmmp/fileinfo.h>

#include <QObject>
#include <QIODevice>

#include "decoder_vorbis.h"
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
    //dogg->input()->close();
    return 0;
}


static long oggtell(void *src)
{
    DecoderVorbis *dogg = (DecoderVorbis *) src;
    long t = dogg->input()->pos();
    return t;
}


// Decoder class

DecoderVorbis::DecoderVorbis(QIODevice *i)
        : Decoder(i)
{
    inited = FALSE;
    m_totalTime = 0;
    m_section = 0;
    m_last_section = -1;
    m_bitrate = 0;
}


DecoderVorbis::~DecoderVorbis()
{
    deinit();
}

bool DecoderVorbis::initialize()
{
    qDebug("DecoderVorbis: initialize");
    inited = FALSE;
    m_totalTime = 0;
    if (!input())
    {
        qDebug("DecoderVorbis: cannot initialize.  No input");
        return FALSE;
    }

    if (!input()->isOpen())
    {
        if (!input()->open(QIODevice::ReadOnly))
        {
            qWarning("%s",qPrintable("DecoderVorbis: failed to open input. " +
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

    quint32 freq = 0;
    m_bitrate = ov_bitrate(&oggfile, -1) / 1000;
    int chan = 0;

    if((m_totalTime = ov_time_total(&oggfile, -1) * 1000) < 0)
        m_totalTime = 0;

    vorbis_info *ogginfo = ov_info(&oggfile, -1);
    if (ogginfo)
    {
        freq = ogginfo->rate;
        chan = ogginfo->channels;
    }

    configure(freq, chan, 16);

    inited = TRUE;
    return TRUE;
}


qint64 DecoderVorbis::totalTime()
{
    if (!inited)
        return 0;
    return m_totalTime;
}

int DecoderVorbis::bitrate()
{
    return m_bitrate;
}


void DecoderVorbis::deinit()
{
    if (inited)
        ov_clear(&oggfile);
    len = 0;
}

void DecoderVorbis::updateTags()
{
    int i;
    vorbis_comment *comments;

    QMap <Qmmp::MetaData, QString> metaData;
    comments = ov_comment (&oggfile, -1);
    for (i = 0; i < comments->comments; i++)
    {
        if (!strncasecmp(comments->user_comments[i], "title=",
                         strlen ("title=")))
            metaData.insert(Qmmp::TITLE, QString::fromUtf8(comments->user_comments[i]
                            + strlen ("title=")));
        else if (!strncasecmp(comments->user_comments[i],
                              "artist=", strlen ("artist=")))
            metaData.insert(Qmmp::ARTIST,
                            QString::fromUtf8(comments->user_comments[i]
                                              + strlen ("artist=")));
        else if (!strncasecmp(comments->user_comments[i],
                              "album=", strlen ("album=")))
            metaData.insert(Qmmp::ALBUM,
                            QString::fromUtf8(comments->user_comments[i]
                                              + strlen ("album=")));
        else if (!strncasecmp(comments->user_comments[i],
                              "comment=", strlen ("comment=")))
            metaData.insert(Qmmp::COMMENT,
                            QString::fromUtf8(comments->user_comments[i]
                                              + strlen ("comment=")));
        else if (!strncasecmp(comments->user_comments[i],
                              "genre=", strlen ("genre=")))
            metaData.insert(Qmmp::GENRE, QString::fromUtf8 (comments->user_comments[i]
                            + strlen ("genre=")));
        else if (!strncasecmp(comments->user_comments[i],
                              "tracknumber=",
                              strlen ("tracknumber=")))
            metaData.insert(Qmmp::TRACK, QString::number(atoi(comments->user_comments[i]
                            + strlen ("tracknumber="))));
        else if (!strncasecmp(comments->user_comments[i],
                              "track=", strlen ("track=")))
            metaData.insert(Qmmp::TRACK, QString::number(atoi(comments->user_comments[i]
                            + strlen ("track="))));
        else if (!strncasecmp(comments->user_comments[i],
                              "date=", strlen ("date=")))
            metaData.insert(Qmmp::YEAR, QString::number(atoi(comments->user_comments[i]
                            + strlen ("date="))));
        else if (!strncasecmp(comments->user_comments[i],
                              "composer=", strlen ("composer=")))
            metaData.insert(Qmmp::COMPOSER, QString::fromUtf8 (comments->user_comments[i]
                            + strlen ("composer=")));
        else if (!strncasecmp(comments->user_comments[i],
                              "discnumber=", strlen ("discnumber=")))
            metaData.insert(Qmmp::DISCNUMBER, QString::number(atoi(comments->user_comments[i]
                            + strlen ("discnumber="))));

    }
    StateHandler::instance()->dispatch(metaData);
}

void DecoderVorbis::seek(qint64 time)
{
    ov_time_seek(&oggfile, (double) time/1000);
}

qint64 DecoderVorbis::read(char *data, qint64 maxSize)
{
    len = -1;
    while (len < 0)
        len = ov_read(&oggfile, data, maxSize, 0, 2, 1, &m_section);

    if (m_section != m_last_section)
        updateTags();
    m_last_section = m_section;
    if(len > 0)
        m_bitrate = ov_bitrate_instant(&oggfile) / 1000;
    return len;
}
