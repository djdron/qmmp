/***************************************************************************
 *   Copyright (C) 2008-2009 by Ilya Kotov                                 *
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


#include <QObject>
#include <QIODevice>
#include <QFile>
#include <math.h>
#include <stdint.h>

#include <qmmp/buffer.h>
#include <qmmp/output.h>
#include <qmmp/recycler.h>
#include "QtDebug"

#include "decoder_wavpack.h"
#include "cueparser.h"

// Decoder class

DecoderWavPack::DecoderWavPack(QObject *parent, DecoderFactory *d, Output *o, const QString &path)
        : Decoder(parent, d, o)
{
    m_path = path;
    m_totalTime = 0.0;
    m_chan = 0;
    m_context = 0;
    m_freq = 0;
    m_cue_parser = 0;
    m_output_buf = 0;
}

DecoderWavPack::~DecoderWavPack()
{
    deinit();
    if (m_output_buf)
        delete [] m_output_buf;
    m_output_buf = 0;
}

bool DecoderWavPack::initialize()
{
    m_chan = 0;
    m_totalTime = 0;

    char err [80];
    if (m_path.startsWith("wvpack://")) //embeded cue track
    {
        QString p = QUrl(m_path).path();
        p.replace(QString(QUrl::toPercentEncoding("#")), "#");
        p.replace(QString(QUrl::toPercentEncoding("%")), "%");
        m_context = WavpackOpenFileInput (p.toLocal8Bit(), err, OPEN_WVC | OPEN_TAGS, 0);
        int cue_len = WavpackGetTagItem (m_context, "cuesheet", NULL, 0);
        char *value;
        if (cue_len)
        {
            value = (char*)malloc (cue_len * 2 + 1);
            WavpackGetTagItem (m_context, "cuesheet", value, cue_len + 1);
            m_cue_parser = new CUEParser(value, p);
            int track = m_path.section("#", -1).toInt();
            if(track > m_cue_parser->count())
            {
                qWarning("DecoderWavPack: invalid cuesheet comment");
                return FALSE;
            }
            m_path = p;
            //send metadata
            QMap<Qmmp::MetaData, QString> metaData = m_cue_parser->info(track)->metaData();
            StateHandler::instance()->dispatch(metaData);
            connect(stateHandler(),SIGNAL(aboutToFinish()),SLOT(processFinish()));
            //next url
            m_nextUrl.clear();
            if(track <= m_cue_parser->count() - 1)
                m_nextUrl = m_cue_parser->info(track + 1)->path();
            m_totalTime = m_cue_parser->length(track);
            setFragment(m_cue_parser->offset(track), m_cue_parser->length(track));
        }
    }
    else
        m_context = WavpackOpenFileInput (m_path.toLocal8Bit(), err, OPEN_WVC | OPEN_TAGS, 0);

    if (!m_context)
    {
        qWarning("DecoderWavPack: error: %s", err);
        return FALSE;
    }

    m_chan = WavpackGetNumChannels(m_context);
    m_freq = WavpackGetSampleRate (m_context);
    int bps = WavpackGetBitsPerSample (m_context);
    if (!m_output_buf)
        m_output_buf = new int32_t[Qmmp::globalBufferSize()/4];
    configure(m_freq, m_chan, bps);
    if(!m_cue_parser)
        m_totalTime = (qint64) WavpackGetNumSamples(m_context) * 1000 / m_freq;
    qDebug("DecoderWavPack: initialize succes");
    return TRUE;
}

int DecoderWavPack::bitrate()
{
    if(m_context)
        return int(WavpackGetInstantBitrate(m_context)/1000);
    return 0;
}

qint64 DecoderWavPack::totalTime()
{
    return m_totalTime;
}

void DecoderWavPack::deinit()
{
    m_chan = 0;
    m_freq = 0;
    if (m_context)
        WavpackCloseFile (m_context);
    m_context = 0;
    if(m_cue_parser)
        delete m_cue_parser;
    m_cue_parser = 0;
}

void DecoderWavPack::seekAudio(qint64 time)
{
     WavpackSeekSample (m_context, time * m_freq / 1000);
}

qint64 DecoderWavPack::readAudio(char *data, qint64 maxSize)
{
    ulong len = WavpackUnpackSamples (m_context, m_output_buf, maxSize / m_chan / 4);
    uint m = 0;
    //convert 32 to 16
    for (uint i = 0;  i < len * m_chan; ++i)
    {
        data[m++] = (m_output_buf[i] >> 0) & 0xff;
        data[m++] = (m_output_buf[i] >> 8) & 0xff;
    }
    return len * m_chan * 2;
}

void DecoderWavPack::processFinish()
{
    if(m_cue_parser && nextUrlRequest(m_nextUrl))
    {
        qDebug("DecoderFLAC: going to next track");
        int track = m_nextUrl.section("#", -1).toInt();
        QString p = QUrl(m_nextUrl).path();
        p.replace(QString(QUrl::toPercentEncoding("#")), "#");
        p.replace(QString(QUrl::toPercentEncoding("%")), "%");
        //update current fragment
        mutex()->lock();
        setFragment(m_cue_parser->offset(track), m_cue_parser->length(track));
        m_totalTime = m_cue_parser->length(track);
        output()->seek(0); //reset time counter
        mutex()->unlock();
         // find next track
        m_nextUrl.clear();
        if(track <= m_cue_parser->count() - 1)
            m_nextUrl = m_cue_parser->info(track + 1)->path();
        //change track
        emit playbackFinished();
        //send metadata
        QMap<Qmmp::MetaData, QString> metaData = m_cue_parser->info(track)->metaData();
        stateHandler()->dispatch(metaData);
    }
}
