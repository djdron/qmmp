/***************************************************************************
 *   Copyright (C) 2009-2010 by Ilya Kotov                                 *
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

#ifndef QMMPAUDIOENGINE_P_H
#define QMMPAUDIOENGINE_P_H

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
class ReplayGain;
class QmmpSettings;

/*! @internal
 * @author Ilya Kotov <forkotov02@hotmail.ru>
 */
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
    void addEffect(EffectFactory *factory);
    void removeEffect(EffectFactory *factory);

    static QmmpAudioEngine *instance();

private slots:
    void finish();
    void updateReplayGainSettings();

private:
    void run();
    void reset();
    void flush(bool = false);
    void addOffset();
    qint64 produceSound(char *data, qint64 size, quint32 brate);
    void sendMetaData();
    Output *createOutput();
    void prepareEffects(Decoder *d);

    DecoderFactory *m_factory;
    QList <Effect*> m_effects;
    QList <Effect*> m_blockedEffects;
    Output *m_output;

    bool m_done, m_finish, m_user_stop;
    uint m_bks;
    qint64 m_totalTime, m_seekTime;
    qint64 m_output_at, m_output_size;
    int m_bitrate, m_chan, m_bps;
    unsigned char *m_output_buf;
    Decoder *m_decoder;
    QQueue <Decoder*> m_decoders;
    QHash <Decoder*, InputSource*> m_inputs;
    AudioParameters m_ap;
    bool m_next;
    static QmmpAudioEngine *m_instance;
    ReplayGain *m_replayGain;
    QmmpSettings *m_settings;
};

#endif // QMMPAUDIOENGINE_P_H
