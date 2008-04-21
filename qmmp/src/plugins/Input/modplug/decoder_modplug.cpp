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

DecoderModPlug::DecoderModPlug(QObject *parent, DecoderFactory *d, QIODevice *i, Output *o)
        : Decoder(parent, d, i, o)
{
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
    m_bks = blockSize();
    m_inited = m_user_stop = m_done = m_finish = FALSE;
    m_freq = m_bitrate = 0;
    m_chan = 0;
    m_output_size = 0;
    m_seekTime = -1.0;
    m_totalTime = 0.0;


    if (! input())
    {
        error("DecoderModPlug: cannot initialize.  No input.");

        return FALSE;
    }

    if (! m_output_buf)
        m_output_buf = new char[globalBufferSize];
    m_output_at = 0;
    m_output_bytes = 0;

    QString filename = qobject_cast<QFile*>(input())->fileName ();
    ArchiveReader reader(this);
    if(reader.isSupported(filename))
    {
        input()->close();
        m_input_buf = reader.unpack(filename);
    }
    else
        m_input_buf = input()->readAll();

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

    m_totalTime = (int) m_soundFile->GetSongTime();
    configure(m_freq, m_chan, m_bps, m_bitrate);
    m_inited = TRUE;
    return TRUE;
}

double DecoderModPlug::lengthInSeconds()
{
    if (! m_inited)
        return 0;

    return m_totalTime;
}


void DecoderModPlug::seek(double pos)
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
    if (! m_inited)
    {
        mutex()->unlock();

        return;
    }
    mutex()->unlock();
    dispatch(DecoderState::Decoding);

    while (! m_done && ! m_finish)
    {
        mutex()->lock ();

        //seeking

        if (m_seekTime >= 0.0)
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
        len = m_soundFile->Read (m_output_buf + m_output_at, len) * m_sampleSize;

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
            // error in read
            error("DecoderModPlug: Error while decoding stream, File appears to be "
                  "corrupted");
            m_finish = TRUE;
        }
        mutex()->unlock();
    }

    mutex()->lock ();

    if (m_finish)
        dispatch(DecoderState::Finished);
    else if (m_user_stop)
        dispatch(DecoderState::Stopped);

    mutex()->unlock();

    deinit();
}

void DecoderModPlug::readSettings()
{
    if (!m_soundFile)
        return;
    QSettings settings(QDir::homePath()+"/.qmmp/qmmprc", QSettings::IniFormat);
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
    ui.amigaCheckBox->setChecked(settings.value("GrabAmigaMOD", TRUE).toBool());*/
    //resampling frequency
    //preamp
    //ui.preampGroupBox->setChecked(settings.value("PreAmp", FALSE).toBool());
    //ui.preampSlider->setValue(int(settings.value("PreAmpLevel", 0.0f).toDouble()*10));
    settings.endGroup();
}

DecoderModPlug* DecoderModPlug::instance()
{
    return m_instance;
}
