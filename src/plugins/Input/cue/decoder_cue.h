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

#ifndef DECODER_CUE_H
#define DECODER_CUE_H

#include <qmmp/decoder.h>
#include <qmmp/statehandler.h>

class Output;
class QIDevice;
//class CUEStateHandler;

class DecoderCUE : public Decoder
{
    Q_OBJECT
public:
    DecoderCUE(QObject *, DecoderFactory *, const QString &url);
    virtual ~DecoderCUE();

    // Standard Decoder API
    bool initialize();
    qint64 lengthInSeconds();
    void seek(qint64);
    void stop();
    void pause();

    // Equalizer
    void setEQ(int bands[10], int preamp);
    void setEQEnabled(bool on);

private:
    // thread run function
    void run();
    QString path;
    Decoder *m_decoder;
    Output *m_output2;
    QIODevice *m_input2;
    qint64 m_length;
    qint64 m_offset;
    int m_preamp2;
    int m_bands2[10];
    bool m_useEQ2;
};

class CUEStateHandler : public StateHandler
{
    Q_OBJECT
public:
    CUEStateHandler(QObject *parent, qint64 offset, qint64 length);
    virtual ~CUEStateHandler();

    void dispatch(qint64 elapsed,
                  qint64 totalTime,
                  int bitrate,
                  int frequency,
                  int precision,
                  int channels);

    void dispatch(const QMap<Qmmp::MetaData, QString> &metaData);

    void dispatch(const Qmmp::State &state);

signals:
    void finished();

private:
    qint64 m_length2;
    qint64 m_offset;
};

#endif // DECODER_CUE_H
