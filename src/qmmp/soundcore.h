/***************************************************************************
 *   Copyright (C) 2006-2010 by Ilya Kotov                                 *
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
#ifndef SOUNDCORE_H
#define SOUNDCORE_H

#include <QObject>
#include <QString>
#include "decoder.h"
#include "output.h"
#include "visual.h"
#include "qmmp.h"
#include "qmmpsettings.h"
#include "eqsettings.h"

class QIODevice;
class VolumeControl;
class AbstractEngine;
class InputSource;

/*! \brief The SoundCore class provides a simple interface for audio playback.
 * @author Ilya Kotov <forkotov02@hotmail.ru>
 */
class SoundCore : public QObject
{
    Q_OBJECT
public:
    /*!
     * Object constructor.
     * @param parent Parent object.
     */
    SoundCore(QObject *parent = 0);
    /*!
     * Destructor.
     */
    ~SoundCore();
    /*!
     * Returns length in milliseconds
     */
    qint64 totalTime() const;

    EqSettings eqSettings() const;
    void setEqSettings(const EqSettings &settings);

    /*!
     * Returns left volume level.
     */
    int leftVolume();
    /*!
     * Returns left volume level.
     */
    int rightVolume();
    /*!
     * Returns the current time (in milliseconds).
     */
    qint64 elapsed();
    /*!
     * Returns current bitrate (in kbps)
     */
    int bitrate();
    /*!
     * Returns current sample rate (in Hz).
     */
    quint32 frequency();
    /*!
     * Returns sample size (in bits).
     */
    int precision();
    /*!
     * Returns channels number.
     */
    int channels();
    /*!
     * Returns the current state.
     */
    Qmmp::State state() const;
    /*!
     * Returns all meta data in map.
     */
    QMap <Qmmp::MetaData, QString> metaData();
    /*!
     * Returns the metdata string associated with the given \b key.
     */
    QString metaData(Qmmp::MetaData key);
    /*!
     * Returns a pointer to the SoundCore instance.
     */
    static SoundCore* instance();

public slots:
    /*!
     * Sets volume.
     * @param left - volume of the left channel \b [0..100].
     * @param right - volume of the right channel \b [0..100].
     */
    void setVolume(int left, int right);
    /*!
     * This function plays file or stream with the given path \p source.
     * Returns \b true if playback has been started successful or source is not a local file,
     * otherwise returns \b false. Useful for invalid files skipping.
     */
    bool play(const QString &source, bool queue = false, qint64 offset = -1);
    /*!
     *  Stops playback
     */
    void stop();
    /*!
     *  Pauses/resumes playback
     */
    void pause();
    /*!
     *  This function sets the current play position to \p pos in milliseconds.
     */
    void seek(qint64 pos);
    /*!
     *  This function returns file path or stream url.
     */
    const QString url();

signals:
    /*!
     * This signal is emitted when the stream reader fills it's buffer.
     * The argument \b progress indicates the current percentage of buffering completed.
     */
    void bufferingProgress(int progress);
    /*!
     * Tracks elapesed time.
     * @param time New track position in milliseconds.
     */
    void elapsedChanged(qint64 time);
    /*!
     * Emitted when bitrate has changed.
     * @param bitrate New bitrate (in kbps)
     */
    void bitrateChanged(int bitrate);
    /*!
     * Emitted when samplerate has changed.
     * @param frequency New sample rate (in Hz)
     */
    void frequencyChanged(quint32 frequency);
    /*!
     * Emitted when sample size has changed.
     * @param precision New sample size (in bits)
     */
    void precisionChanged(int precision);
    /*!
     * Emitted when channels number has changed.
     * @param channels New channels number.
     */
    void channelsChanged(int channels);
    /*!
     * Emitted when new metadata is available.
     */
    void metaDataChanged ();
    /*!
     * This signal is emitted when the state of the SoundCore has changed.
     */
    void stateChanged (Qmmp::State newState);
    /*!
     * Emitted when playback has finished.
     */
    void finished();
    /*!
     * Emitted when volume has changed.
     * @param left Left channel volume level. It should be \b [0..100]
     * @param right Right channel volume level. It should be \b [0..100]
     */
    void volumeChanged(int left, int right);
    /*!
     * Emitted when equalizer settings have changed.
     */
    void eqSettingsChanged();
    /*!
     * Emitted before playback ends. Use this signal to append new url to the queue.
     */
    void nextTrackRequest();

private slots:
    bool enqueue(InputSource *);
    void startPendingEngine();
    void updateVolume();

private:
    Decoder* m_decoder;
    QString m_url;
    uint m_error;
    bool m_paused;
    bool m_update;
    bool m_block;
    Visual *m_vis;
    QList <Visual*> m_visuals;
    QWidget *m_parentWidget;
    static SoundCore* m_instance;
    StateHandler *m_handler;
    VolumeControl *m_volumeControl;
    AbstractEngine *m_engine;
    AbstractEngine *m_pendingEngine;
    QList<InputSource *> m_pendingSources;
};

#endif
