/***************************************************************************
 *   Copyright (C) 2007-2009 by Ilya Kotov                                 *
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

#ifndef DECODER_AUDIOFILE_H
#define DECODER_AUDIOFILE_H

extern "C"{
#include <sndfile.h>
}
#include <qmmp/decoder.h>


class DecoderSndFile : public Decoder
{
public:
    DecoderSndFile(QObject *, DecoderFactory *, Output *, const QString &);
    virtual ~DecoderSndFile();

    // Standard Decoder API
    bool initialize();
    qint64 totalTime();
    int bitrate();

private:
    qint64 readAudio(char *audio, qint64 maxSize);
    void seekAudio(qint64 time);

    // helper functions
    void deinit();

    SNDFILE *m_sndfile;
    int m_bitrate;
    quint32 m_freq;
    qint64 m_totalTime;
    QString m_path;
};


#endif // DECODER_SNDFILE_H
