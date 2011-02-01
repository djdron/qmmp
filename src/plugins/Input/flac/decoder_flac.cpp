/***************************************************************************
 *   Copyright (C) 2006-2010 by Ilya Kotov                                 *
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


/* The code is based on MOC by Damian Pietras <daper@daper.net>
   and libxmms-flac written by Josh Coalson. */


#include <taglib/tag.h>
#include <taglib/fileref.h>
#include <taglib/flacfile.h>
#include <taglib/xiphcomment.h>
#include <taglib/tmap.h>
#include <taglib/id3v2header.h>
#include <qmmp/buffer.h>
#include <qmmp/output.h>
#include <qmmp/recycler.h>
#include <qmmp/statehandler.h>
#include <QObject>
#include <QFile>
#include <QIODevice>
#include <FLAC/all.h>
#include "replaygainreader.h"
#include "cueparser.h"
#include "decoder_flac.h"

static size_t pack_pcm_signed (FLAC__byte *data,
                               const FLAC__int32 * const input[],
                               unsigned wide_samples,
                               unsigned channels, unsigned bps)
{
    FLAC__byte * const start = data;
    FLAC__int32 sample;
    const FLAC__int32 *input_;
    unsigned samples, channel;
    unsigned bytes_per_sample;
    unsigned incr;

    if(bps == 24) // we encode to 32-bit words
        bps = 32;
    bytes_per_sample = bps / 8;
    incr = bytes_per_sample * channels;
    for (channel = 0; channel < channels; channel++)
    {
        samples = wide_samples;
        data = start + bytes_per_sample * channel;
        input_ = input[channel];

        while (samples--)
        {
            sample = *input_++;

            switch (bps)
            {
            case 8:
                data[0] = sample;
                break;
            case 16:
                data[1] = (FLAC__byte)(sample >> 8) & 0xff;
                data[0] = (FLAC__byte)sample & 0xff;
                break;
            case 32:
                data[3] = (FLAC__byte)(sample >> 16) & 0xff;
                data[2] = (FLAC__byte)(sample >> 8) & 0xff;
                data[1] = (FLAC__byte)sample & 0xff;
                data[0] = 0;
            }
            data += incr;
        }
    }
    return wide_samples * channels * bytes_per_sample;
}

static int flac_decode (void *void_data, char *buf, int buf_len)
{
    DecoderFLAC *dflac = (DecoderFLAC *) void_data;
    unsigned to_copy;
    int bytes_per_sample;

    bytes_per_sample = dflac->data()->bits_per_sample / 8;

    if (!dflac->data()->sample_buffer_fill)
    {

        if (FLAC__stream_decoder_get_state(dflac->data()->decoder)
                == FLAC__STREAM_DECODER_END_OF_STREAM)
        {
            return 0;
        }

        if (!FLAC__stream_decoder_process_single(
                    dflac->data()->decoder))
        {
            return 0;
        }
    }

    to_copy = qMin((unsigned)buf_len, dflac->data()->sample_buffer_fill);
    memcpy (buf, dflac->data()->sample_buffer, to_copy);
    memmove (dflac->data()->sample_buffer,
             dflac->data()->sample_buffer + to_copy,
             dflac->data()->sample_buffer_fill - to_copy);
    dflac->data()->sample_buffer_fill -= to_copy;
    return to_copy;
}


static FLAC__StreamDecoderReadStatus flac_callback_read (const FLAC__StreamDecoder*,
        FLAC__byte buffer[],
        size_t *bytes,
        void *client_data)
{
    DecoderFLAC *dflac = (DecoderFLAC *) client_data;
    qint64 res = dflac->data()->input->read((char *)buffer, *bytes);
    dflac->data()->last_bytes += res;

    if (res > 0)
    {
        *bytes = res;
        return FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;
    }
    if (res == 0)
    {
        *bytes = res;
        return FLAC__STREAM_DECODER_READ_STATUS_END_OF_STREAM;
    }

    return FLAC__STREAM_DECODER_READ_STATUS_ABORT;

}

static FLAC__StreamDecoderWriteStatus flac_callback_write (const FLAC__StreamDecoder *,
        const FLAC__Frame *frame,
        const FLAC__int32* const buffer[],
        void *client_data)
{
    DecoderFLAC *dflac = (DecoderFLAC *) client_data;
    const unsigned wide_samples = frame->header.blocksize;

    if (dflac->data()->abort)
        return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;

    dflac->data()->bitrate = dflac->data()->last_bytes * 8.0 * frame->header.sample_rate /
                             frame->header.blocksize / 1000.0;
    dflac->data()->last_bytes = 0;

    dflac->data()->sample_buffer_fill = pack_pcm_signed (
                                            dflac->data()->sample_buffer,
                                            buffer, wide_samples,
                                            dflac->data()->channels,
                                            dflac->data()->bits_per_sample);

    return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}

static FLAC__StreamDecoderTellStatus flac_callback_tell (const FLAC__StreamDecoder *,
        FLAC__uint64 *offset,
        void *client_data)
{
    DecoderFLAC *dflac = (DecoderFLAC *) client_data;
    *offset = dflac->data()->input->pos ();
    return FLAC__STREAM_DECODER_TELL_STATUS_OK;
}

static FLAC__StreamDecoderSeekStatus flac_callback_seek (const FLAC__StreamDecoder *,
        FLAC__uint64 offset,
        void *client_data)
{
    DecoderFLAC *dflac = (DecoderFLAC *) client_data;

    return dflac->data()->input->seek(offset)
           ? FLAC__STREAM_DECODER_SEEK_STATUS_OK
           : FLAC__STREAM_DECODER_SEEK_STATUS_ERROR;
}

static FLAC__StreamDecoderLengthStatus flac_callback_length (const FLAC__StreamDecoder *,
        FLAC__uint64 *stream_length,
        void *client_data)
{
    DecoderFLAC *dflac = (DecoderFLAC *) client_data;
    *stream_length = dflac->data()->input->size();
    return FLAC__STREAM_DECODER_LENGTH_STATUS_OK;
}

static void flac_callback_metadata (const FLAC__StreamDecoder *,
                                    const FLAC__StreamMetadata *metadata,
                                    void *client_data)
{
    DecoderFLAC *dflac = (DecoderFLAC *) client_data;

    if (metadata->type == FLAC__METADATA_TYPE_STREAMINFO)
    {
        qDebug ("DecoderFLAC: getting metadata info");

        dflac->data()->total_samples =
            (unsigned)(metadata->data.stream_info.total_samples
                       & 0xffffffff);
        dflac->data()->bits_per_sample =
            metadata->data.stream_info.bits_per_sample;
        dflac->data()->channels = metadata->data.stream_info.channels;
        dflac->data()->sample_rate = metadata->data.stream_info.sample_rate;
        dflac->data()->length = dflac->data()->total_samples * 1000 / dflac->data()->sample_rate;
    }
}

static FLAC__bool flac_callback_eof (const FLAC__StreamDecoder *, void *)
{
    return false;
}

static void flac_callback_error (const FLAC__StreamDecoder *,
                                 FLAC__StreamDecoderErrorStatus status,
                                 void *)
{
    Q_UNUSED(status);
}

// Decoder class

DecoderFLAC::DecoderFLAC(const QString &path, QIODevice *i)
        : Decoder(i)
{
    m_data = 0;
    m_path = path;
    m_data = new flac_data;
    m_data->decoder = NULL;
    data()->input = i;
    m_parser = 0;
    length_in_bytes = 0;
    m_totalBytes = 0;
    m_sz = 0;
    m_buf = 0;
    m_offset = 0;
}


DecoderFLAC::~DecoderFLAC()
{
    deinit();
    if (data())
    {
        if (data()->decoder)
            FLAC__stream_decoder_delete (data()->decoder);
        delete data();
        m_data = 0;
    }
    if(m_buf)
        delete[] m_buf;
    m_buf = 0;
}

bool DecoderFLAC::initialize()
{
    if (!data()->input)
    {
        QString p = m_path;
        if (m_path.startsWith("flac://")) //embeded cue track
        {
            p = QUrl(m_path).path();
            if (!p.endsWith(".flac"))
            {
                qWarning("DecoderFLAC: invalid url.");
                return false;
            }
            p.replace(QString(QUrl::toPercentEncoding("#")), "#");
            p.replace(QString(QUrl::toPercentEncoding("?")), "?");
            p.replace(QString(QUrl::toPercentEncoding("%")), "%");
            p.replace(QString(QUrl::toPercentEncoding(":")), ":");
            TagLib::FLAC::File fileRef(p.toLocal8Bit ());
            //looking for cuesheet comment
            TagLib::Ogg::XiphComment *xiph_comment = fileRef.xiphComment();
            QList <FileInfo*> list;
            if (xiph_comment && xiph_comment->fieldListMap().contains("CUESHEET"))
            {
                qDebug("DecoderFLAC: using cuesheet xiph comment.");
                m_parser = new CUEParser(xiph_comment->fieldListMap()["CUESHEET"].toString()
                                            .toCString(true), p);
                m_track = m_path.section("#", -1).toInt();
                if(m_track > m_parser->count())
                {
                    qWarning("DecoderFLAC: invalid cuesheet xiph comment");
                    return false;
                }
                data()->input = new QFile(p);
                data()->input->open(QIODevice::ReadOnly);
                QMap<Qmmp::MetaData, QString> metaData = m_parser->info(m_track)->metaData();
                StateHandler::instance()->dispatch(metaData); //send metadata
                ReplayGainReader rg(p);
                setReplayGainInfo(rg.replayGainInfo());
            }
            else
            {
                qWarning("DecoderFLAC: unable to find cuesheet comment.");
                return false;
            }
        }
        else
        {
            qWarning("DecoderFLAC: cannot initialize.  No input.");
            return false;
        }
    }

    if (!data()->input->isOpen())
    {
        qWarning("DecoderFLAC: unable to open input file");
        return false;
    }

    m_data->bitrate = -1;
    m_data->abort = 0;
    m_data->sample_buffer_fill = 0;
    m_data->last_bytes = 0;
    if (!m_data->decoder)
    {
        qDebug("DecoderFLAC: creating FLAC__StreamDecoder");
        m_data->decoder = FLAC__stream_decoder_new ();
    }
    char buf[500];
    //skip id3v2
    data()->input->peek(buf, sizeof(buf));
    ulong id3v2_size = findID3v2(buf, sizeof(buf));
    if(id3v2_size)
    {
        qDebug("DecoderFLAC: skipping id3v2 tag (%lu bytes)", id3v2_size);
        data()->input->seek(id3v2_size);
    }
    data()->input->peek(buf,sizeof(buf));
    data()->input->seek(0);
    qDebug("DecoderFLAC: setting callbacks");
    if(!memcmp(buf, "OggS", 4))
    {
        if(!FLAC_API_SUPPORTS_OGG_FLAC)
        {
            qWarning("DecoderFLAC: unsupported format");
            return false;
        }
        if (FLAC__stream_decoder_init_ogg_stream(
                m_data->decoder,
                flac_callback_read,
                flac_callback_seek,
                flac_callback_tell,
                flac_callback_length,
                flac_callback_eof,
                flac_callback_write,
                flac_callback_metadata,
                flac_callback_error,
                this) != FLAC__STREAM_DECODER_INIT_STATUS_OK)
        {
            data()->ok = 0;
            return false;
        }
    }
    else if (!memcmp(buf, "fLaC", 4))
    {
        if (FLAC__stream_decoder_init_stream(
                m_data->decoder,
                flac_callback_read,
                flac_callback_seek,
                flac_callback_tell,
                flac_callback_length,
                flac_callback_eof,
                flac_callback_write,
                flac_callback_metadata,
                flac_callback_error,
                this) != FLAC__STREAM_DECODER_INIT_STATUS_OK)
        {
            data()->ok = 0;
            return false;
        }
    }
    else
    {
        qWarning("DecoderFLAC: unsupported format");
        return false;
    }

    if (!FLAC__stream_decoder_process_until_end_of_metadata(
                data()->decoder))
    {
        data()->ok = 0;
        return false;
    }
    switch(data()->bits_per_sample)
    {
    case 8:
        configure(data()->sample_rate, data()->channels, Qmmp::PCM_S8);
        break;
    case 16:
        configure(data()->sample_rate, data()->channels, Qmmp::PCM_S16LE);
        break;
    case 24:
    case 32:
        configure(data()->sample_rate, data()->channels, Qmmp::PCM_S32LE);
        break;
    default:
        return false;
    }
    if(!m_path.contains("://"))
    {
        ReplayGainReader rg(m_path);
        setReplayGainInfo(rg.replayGainInfo());
    }

    if(m_parser)
    {
        m_length = m_parser->length(m_track);
        m_offset = m_parser->offset(m_track);
        length_in_bytes = audioParameters().sampleRate() *
                          audioParameters().channels() *
                          audioParameters().sampleSize() * m_length/1000;
        seek(0);
    }
    m_totalBytes = 0;
    m_sz = audioParameters().sampleSize() * audioParameters().channels();

    qDebug("DecoderFLAC: initialize succes");
    return true;
}

qint64 DecoderFLAC::totalTime()
{
    if(m_parser)
        return m_length;
    return data()->length;
}

int DecoderFLAC::bitrate()
{
    return data()->bitrate;
}

void DecoderFLAC::seek(qint64 time)
{
    m_totalBytes = audioParameters().sampleRate() *
                   audioParameters().channels() *
                   audioParameters().sampleSize() * time/1000;
    if(m_parser)
        time += m_offset;
    FLAC__uint64 target_sample = FLAC__uint64(time * data()->total_samples /data()->length);
    FLAC__stream_decoder_seek_absolute(data()->decoder, target_sample);

}

qint64 DecoderFLAC::read(char *data, qint64 size)
{
    if(m_parser)
    {
        if(length_in_bytes - m_totalBytes < m_sz) //end of cue track
            return 0;

        qint64 len = 0;

        if(m_buf) //read remaining data first
        {
            len = qMin(m_buf_size, size);
            memmove(data, m_buf, len);
            if(size >= m_buf_size)
            {
                delete[] m_buf;
                m_buf = 0;
                m_buf_size = 0;
            }
            else
                memmove(m_buf, m_buf + len, size - len);
        }
        else
            len = flac_decode (this, data, size);

        if(len <= 0) //end of file
            return 0;

        if(len + m_totalBytes <= length_in_bytes)
        {
            m_totalBytes += len;
            return len;
        }

        qint64 len2 = qMax(qint64(0), length_in_bytes - m_totalBytes);
        len2 = (len2 / m_sz) * m_sz; //returned size must contain integer number of samples
        m_totalBytes += len2;
        //save data of the next track
        if(m_buf)
            delete[] m_buf;
        m_buf_size = len - len2;
        m_buf = new char[m_buf_size];
        memmove(m_buf, data + len2, m_buf_size);
        return len2;
    }
    return flac_decode (this, data, size);
}

void DecoderFLAC::deinit()
{
    if (data()->decoder)
        FLAC__stream_decoder_finish (data()->decoder);

    if (!input() && data()->input) //delete internal input only
    {
        data()->input->close();
        delete data()->input;
        data()->input = 0;
    };
    if(m_parser)
        delete m_parser;
    m_parser = 0;
}

const QString DecoderFLAC::nextURL()
{
    if(m_parser && m_track +1 <= m_parser->count())
        return m_parser->trackURL(m_track + 1);
    else
        return QString();
}

void DecoderFLAC::next()
{
    if(m_parser && m_track +1 <= m_parser->count())
    {
        m_track++;
        m_offset = m_parser->length(m_track);
        m_length = m_parser->length(m_track);
        length_in_bytes = audioParameters().sampleRate() *
                          audioParameters().channels() *
                          audioParameters().sampleSize() * m_length/1000;
        StateHandler::instance()->dispatch(m_parser->info(m_track)->metaData());
        m_totalBytes = 0;
    }
}

uint DecoderFLAC::findID3v2(char *data, ulong size) //retuns ID3v2 tag size
{
    if (size < 10)
        return 0;
    if (!memcmp(data, "ID3", 3))
    {
        TagLib::ByteVector byteVector(data, size);
        TagLib::ID3v2::Header header(byteVector);
        return header.completeTagSize();
    }
    return 0;
}
