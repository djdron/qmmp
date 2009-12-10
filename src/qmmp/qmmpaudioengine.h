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

#ifndef QMMPAUDIOENGINE_H
#define QMMPAUDIOENGINE_H

#include <QQueue>
#include <QHash>
#include "abstractengine.h"
#include "audioparameters.h"

class QIODevice;
class Output;
class Effect;
class DecoderFactory;
class StateHandler;
class Decoder;
class InputSource;
class EffectFactory;


class QmmpAudioEngine : public AbstractEngine
{
Q_OBJECT
public:
    QmmpAudioEngine(QObject *parent);
    ~QmmpAudioEngine();

    bool play();
    bool enqueue(InputSource *source);
    qint64 totalTime();
    void seek(qint64 time);
    void stop();
    void pause();
    void setEQ(double bands[10], double preamp);
    void setEQEnabled(bool on);
    void addEffect(EffectFactory *factory);
    void removeEffect(EffectFactory *factory);
    static QmmpAudioEngine *instance();

private slots:
    void finish();

private:
    void run();
    void reset();
    void flush(bool = FALSE);
    qint64 produceSound(char *data, qint64 size, quint32 brate, int chan);
    void sendMetaData();
    Output *createOutput(Decoder *d);

    DecoderFactory *m_factory;
    QList <Effect*> m_effects;
    Output *m_output;

    uint _blksize;
    bool m_eqInited;
    bool m_useEQ;
    bool m_done, m_finish, m_user_stop;
    ulong m_bks;
    qint64 m_totalTime, m_seekTime;
    qint64 m_output_at;
    int m_bitrate, m_chan, m_bps;
    unsigned char *m_output_buf;
    Decoder *m_decoder;
    QQueue <Decoder*> m_decoders;
    QHash <Decoder*, InputSource*> m_inputs;
    AudioParameters m_ap;
    bool m_next;
    static QmmpAudioEngine *m_instance;

};

#endif // QMMPAUDIOENGINE_H
