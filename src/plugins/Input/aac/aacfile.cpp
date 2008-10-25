/***************************************************************************
 *   Copyright (C) 2008 by Ilya Kotov                                      *
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

#include <QIODevice>

#include <neaacdec.h>

#include "aacfile.h"

#define MAX_CHANNELS 6

static int adts_sample_rates[] = {96000,88200,64000,48000,44100,32000,24000,22050,16000,12000,11025,8000,7350,0,0,0};

AACFile::AACFile(QIODevice *i)
{
    m_length = 0;
    m_bitrate = 0;
    m_input = i;
    parseADTS();
}

AACFile::~AACFile()
{
}

qint64 AACFile::length()
{
    return m_length;
}

quint32 AACFile::bitrate()
{
    return m_bitrate;
}

void AACFile::parseADTS()
{
    uchar *buf = new uchar[FAAD_MIN_STREAMSIZE*MAX_CHANNELS];
    qint64 buf_at = 0;
    int frames, frame_length;
    int t_framelength = 0;
    int samplerate = 0;
    float frames_per_sec, bytes_per_frame;
    qint64 pos = m_input->pos();

    m_input->seek(0);

    buf_at = m_input->read((char *)buf, FAAD_MIN_STREAMSIZE*MAX_CHANNELS);

    for (int i = 0; i < buf_at - 1; i++)
    {
        if (buf[i] == 0xff && (buf[i+1]&0xf6) == 0xf0)
        {
            memmove (buf, buf + i, buf_at - i);
            buf_at -= i;
            break;
        }
    }

    /* Read all frames to ensure correct time and bitrate */
    for (frames = 0; /* */; frames++)
    {
        //qDebug("frame header = %d", buf[0]);
        buf_at += m_input->read((char *)buf + buf_at, FAAD_MIN_STREAMSIZE*MAX_CHANNELS - buf_at);

        if (buf_at > 7)
        {
            /* check syncword */
            if (!((buf[0] == 0xFF)&&((buf[1] & 0xF6) == 0xF0)))
                break;

            if (frames == 0)
                samplerate = adts_sample_rates[(buf[2]&0x3c)>>2];

            frame_length = ((((unsigned int)buf[3] & 0x3)) << 11)
                           | (((unsigned int)buf[4]) << 3) | (buf[5] >> 5);

            t_framelength += frame_length;

            if (frame_length > buf_at)
                break;

            buf_at -= frame_length;
            memmove(buf, buf + frame_length, buf_at);
        }
        else
        {
            break;
        }
    }
    m_input->seek(pos);
    frames_per_sec = (float)samplerate/1024.0f;
    if (frames != 0)
        bytes_per_frame = (float)t_framelength/(float)(frames*1000);
    else
        bytes_per_frame = 0;
    m_bitrate = (quint32)(8. * bytes_per_frame * frames_per_sec + 0.5);

    if (frames_per_sec != 0)
        m_length = frames/frames_per_sec;
    else
        m_length = 1;
}
