/***************************************************************************
 *   Copyright (C) 2009 by Ilya Kotov                                      *
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

#ifndef DECODER_CDAUIDO_H
#define DECODER_CDAUIDO_H

#include <sys/types.h>
#include <cdio/cdio.h>

#include <qmmp/decoder.h>

class CDATrack
{
public:
    CDATrack()
    {
        first_sector = 0;
        last_sector = 0;
    }

    FileInfo info;
    lsn_t first_sector;
    lsn_t last_sector;
};

class DecoderCDAudio : public Decoder
{
public:
    DecoderCDAudio(QObject *, DecoderFactory *, const QString &url, Output *);
    virtual ~DecoderCDAudio();

    static QList <CDATrack> generateTrackList(const QString &device = QString());
    static qint64 calculateTrackLength(lsn_t startlsn, lsn_t endlsn);

    // Standard Decoder API
    bool initialize();
    qint64 totalTime();
    void seek(qint64);
    void stop();

private:
    // thread run function
    void run();
    // helper functions
    void flush(bool = FALSE);
    void deinit();
    // libcdio variables
    lsn_t m_first_sector;
    lsn_t m_last_sector;
    lsn_t m_current_sector;
    CdIo_t *m_cdio;
    // output buffer
    char *m_output_buf;
    ulong m_output_bytes, m_output_at;
    //other variables
    bool m_inited, m_user_stop, m_done, m_finish;
    QString m_url;
    unsigned int m_bks;
    quint64 m_bitrate;
    qint64 m_totalTime, m_seekTime;
};

#endif // DECODER_CDAUIDO_H
