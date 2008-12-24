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

/* Based on Modplug XMMS Plugin
 * Authors: Kenton Varda <temporal@gauge3d.org>
 */

#include <QObject>
#include <QIODevice>
#include <QFile>
#include <QDir>
#include <QSettings>
#include <math.h>

#include <libmodplug/stdafx.h>
#include <libmodplug/it_defs.h>
#include <libmodplug/sndfile.h>

#include <qmmp/constants.h>
#include <qmmp/buffer.h>
#include <qmmp/output.h>
#include <qmmp/recycler.h>

#include <stdint.h>

#include "archivereader.h"
#include "decoder_modplug.h"

// Decoder class

DecoderModPlug* DecoderModPlug::m_instance = 0;

DecoderModPlug::DecoderModPlug(QObject *parent, DecoderFactory *d, Output *o, const QString &path)
        : Decoder(parent, d, o)
{
    m_path = path;
    m_inited = FALSE;
    m_user_stop = FALSE;
    m_output_buf = 0;
    m_output_bytes = 0;
    m_output_at = 0;
    m_bks = 0;
    m_done = FALSE;
    m_finish = FALSE;
    m_freq = 0;
    m_bitrate = 0;
    m_seekTime = -1.0;
    m_totalTime = 0.0;
    m_chan = 0;
    m_output_size = 0;
    //m_modFile = 0;
    m_soundFile = 0;
    m_sampleSize = 0;
    m_instance = this;
}

DecoderModPlug::~DecoderModPlug()
{
    deinit();
    if (m_output_buf)
        delete [] m_output_buf;
    m_output_buf = 0;
    m_instance = 0;
}

void DecoderModPlug::stop()
{
    m_user_stop = TRUE;
}

void DecoderModPlug::flush(bool final)
{
    ulong min = final ? 0 : m_bks;

    while ((! m_done && ! m_finish) && m_output_bytes > min)
    {
        output()->recycler()->mutex()->lock ();

        while ((! m_done && ! m_finish) && output()->recycler()->full())
        {
            mutex()->unlock();

            output()->recycler()->cond()->wait(output()->recycler()->mutex());

            mutex()->lock ();
            m_done = m_user_stop;
        }

        if (m_user_stop || m_finish)
        {
            m_inited = FALSE;
            m_done = TRUE;
        }
        else
        {
            m_output_bytes -= produceSound(m_output_buf, m_output_bytes, m_bitrate, m_chan);
            m_output_size += m_bks;
            m_output_at = m_output_bytes;
        }

        if (output()->recycler()->full())
        {
            output()->recycler()->cond()->wakeOne();
        }

        output()->recycler()->mutex()->unlock();
    }
}

bool DecoderModPlug::initialize()
{
    m_bks = Buffer::size();
    m_inited = m_user_stop = m_done = m_finish = FALSE;
    m_freq = m_bitrate = 0;
    m_chan = 0;
    m_output_size = 0;
    m_seekTime = -1.0;
    m_totalTime = 0.0;

    if (! m_output_buf)
        m_output_buf = new char[globalBufferSize];
    m_output_at = 0;
    m_output_bytes = 0;

    ArchiveReader reader(this);
    if (reader.isSupported(m_path))
        m_input_buf = reader.unpack(m_path);
    else
    {
        QFile file(m_path);
        if (!file.open(QIODevice::ReadOnly))
        {
            qWarning("DecoderModPlug: error: %s", qPrintable(file.errorString ()));
            return FALSE;
        }
        m_input_buf = file.readAll();
        file.close();
    }
    if (m_input_buf.isEmpty())
    {
        qWarning("DecoderModPlug: error reading moplug file");
        return FALSE;
    }
    m_soundFile = new CSoundFile();
    readSettings();
    m_sampleSize = m_bps / 8 * m_chan;
    m_soundFile->Create((uchar*) m_input_buf.data(), m_input_buf.size());
    m_bitrate = m_soundFile->GetNumChannels();
    /*if(!m_modFile)
    {
        qWarning("DecoderModPlug: error reading moplug file");
        return FALSE;
    }*/

    m_totalTime = (qint64) m_soundFile->GetSongTime();
    configure(m_freq, m_chan, m_bps);
    m_inited = TRUE;
    return TRUE;
}

qint64 DecoderModPlug::lengthInSeconds()
{
    if (! m_inited)
        return 0;

    return m_totalTime;
}


void DecoderModPlug::seek(qint64 pos)
{
    m_seekTime = pos;
}

void DecoderModPlug::deinit()
{
    m_inited = m_user_stop = m_done = m_finish = FALSE;
    m_freq = m_bitrate = 0;
    m_chan = 0;
    m_output_size = 0;
    if (m_soundFile)
    {
        m_soundFile->Destroy();
        delete m_soundFile;
        m_soundFile = 0;
    }
    m_input_buf.clear();
}

void DecoderModPlug::run()
{
    mutex()->lock ();

    ulong len = 0;
    if (!m_inited)
    {
        mutex()->unlock();
        return;
    }
    mutex()->unlock();

    char *prebuf = new char[m_bks];

    while (!m_done && !m_finish)
    {
        mutex()->lock ();

        //seeking

        if (m_seekTime >= 0)
        {
            quint32  lMax;
            quint32  lMaxtime;
            double lPostime;

            if (m_seekTime > (lMaxtime = m_soundFile->GetSongTime()))
                m_seekTime = lMaxtime;
            lMax = m_soundFile->GetMaxPosition();
            lPostime = float(lMax) / lMaxtime;
            m_soundFile->SetCurrentPos(int(m_seekTime * lPostime));
            m_seekTime = -1.0;
        }

        // decode
        len = m_bks > (globalBufferSize - m_output_at) ? globalBufferSize - m_output_at : m_bks;
        len = m_soundFile->Read (prebuf, len) * m_sampleSize;

        //preamp
        if (m_usePreamp)
        {
            {
                //apply preamp
                if (m_bps == 16)
                {
                    uint n = len >> 1;
                    for (uint i = 0; i < n; i++)
                    {
                        short old = ((short*)prebuf)[i];
                        ((short*)prebuf)[i] *= m_preampFactor;
                        // detect overflow and clip!
                        if ((old & 0x8000) !=
                                (((short*)prebuf)[i] & 0x8000))
                            ((short*)prebuf)[i] = old | 0x7FFF;

                    }
                }
                else
                {
                    for (uint i = 0; i < len; i++)
                    {
                        uchar old = ((uchar*)prebuf)[i];
                        ((uchar*)prebuf)[i] *= m_preampFactor;
                        // detect overflow and clip!
                        if ((old & 0x80) !=
                                (((uchar*)prebuf)[i] & 0x80))
                            ((uchar*)prebuf)[i] = old | 0x7F;
                    }
                }
            }
        }

        memmove(m_output_buf + m_output_at, prebuf, len);

        if (len > 0)
        {
            m_output_at += len;
            m_output_bytes += len;

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
                while (! output()->recycler()->empty() && ! m_user_stop)
                {
                    output()->recycler()->cond()->wakeOne();
                    mutex()->unlock();
                    output()->recycler()->cond()->wait(output()->recycler()->mutex());
                    mutex()->lock ();
                }
                output()->recycler()->mutex()->unlock();
            }

            m_done = TRUE;
            if (! m_user_stop)
            {
                m_finish = TRUE;
            }
        }
        else
        {
            // error while read
            qWarning("DecoderModPlug: Error while decoding stream, File appears to be corrupted");
            m_finish = TRUE;
        }
        mutex()->unlock();
    }

    mutex()->lock ();

    if (m_finish)
        finish();

    mutex()->unlock();
    delete prebuf;
    deinit();
}

void DecoderModPlug::readSettings()
{
    if (!m_soundFile)
        return;
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("ModPlug");
    CSoundFile::SetWaveConfig
    (
        m_freq = settings.value("Frequency", 44100).toInt(),
        m_bps = settings.value("Bits", 16).toInt(),
        m_chan = settings.value("Channels", 2).toInt()
    );

    CSoundFile::SetWaveConfigEx
    (
        settings.value("Surround", TRUE).toBool(),
        TRUE,
        settings.value("Reverb", TRUE).toBool(),
        TRUE,
        settings.value("Megabass", FALSE).toBool(),
        settings.value("NoiseReduction", FALSE).toBool(),
        FALSE
    );
    if (settings.value("Reverb", TRUE).toBool())
    {
        CSoundFile::SetReverbParameters
        (
            settings.value("ReverbDepth", 30).toInt(),
            settings.value("ReverbDelay", 100).toInt()
        );
    }
    if (settings.value("Megabass", FALSE).toBool())
    {
        CSoundFile::SetXBassParameters
        (
            settings.value("BassAmount", 40).toInt(),
            settings.value("BassRange", 30).toInt()
        );
    }
    if (settings.value("Surround", TRUE).toBool())
    {
        CSoundFile::SetSurroundParameters
        (
            settings.value("SurroundDepth", 20).toInt(),
            settings.value("SurroundDelay", 20).toInt()
        );
    }
    CSoundFile::SetResamplingMode(settings.value("ResamplineMode", SRCMODE_POLYPHASE).toInt());
    m_soundFile->SetRepeatCount(settings.value("LoopCount", 0).toInt());


    //general
    /*
     settings.value("GrabAmigaMOD", TRUE).toBool());*/
    //preamp
    m_usePreamp = settings.value("PreAmp", FALSE).toBool();
    m_preampFactor = exp(settings.value("PreAmpLevel", 0.0f).toDouble());
    settings.endGroup();
}

DecoderModPlug* DecoderModPlug::instance()
{
    return m_instance;
}
