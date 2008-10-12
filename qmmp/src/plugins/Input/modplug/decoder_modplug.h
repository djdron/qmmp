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

#ifndef DECODER_MODPLUG_H
#define DECODER_MODPLUG_H

#include <qmmp/decoder.h>

class CSoundFile;

class DecoderModPlug : public Decoder
{
public:
    DecoderModPlug(QObject *, DecoderFactory *, Output *, const QString &path);
    virtual ~DecoderModPlug();

    // Standard Decoder API
    bool initialize();
    qint64 lengthInSeconds();
    void seek(qint64);
    void stop();

    void readSettings();
    static DecoderModPlug* instance();

private:
    // thread run function
    void run();

    // helper functions
    void flush(bool = FALSE);
    void deinit();

    //ModPlug_Settings m_modSettings;
    //CSoundFile *m_modFile;
    CSoundFile *m_soundFile;

    bool m_inited, m_user_stop;
    int m_bps; //bits per sample


    //input buffer
    QByteArray m_input_buf;

    // output buffer
    char *m_output_buf;
    ulong m_output_bytes, m_output_at;

    unsigned int m_bks; //block size
    bool m_done, m_finish;
    long m_freq, m_bitrate;
    int m_chan, m_sampleSize;
    unsigned long m_output_size;
    qint64 m_totalTime, m_seekTime;
    double m_preampFactor;
    bool m_usePreamp;
    QString m_path;
    static DecoderModPlug* m_instance;
};


#endif // DECODER_MODPLUG_H
