/***************************************************************************
 *   Copyright (C) 2008-2009 by Ilya Kotov                                 *
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
#ifndef STATEHANDLER_H
#define STATEHANDLER_H

#include <QObject>
#include <QMap>
#include <QMutex>

#include "qmmp.h"

/*! @brief The StateHandler class allows to track information about playback progress.
 * @author Ilya Kotov <forkotov02@hotmail.ru>
 */
class StateHandler : public QObject
{
    Q_OBJECT
public:
    /*!
     * Object constructor.
     * @param parent Parent object.
     */
    StateHandler(QObject *parent = 0);
    /*!
     * Destructor.
     */
    ~StateHandler();
    /*!
     * Sends information about playback progress.
     * @param elapsed Current time (in milliseconds).
     * @param bitrate Current bitrate (in kbps).
     * @param frequency Current samplerate (in Hz).
     * @param precision Sample size (in bits).
     * @param channels Number of channels.
     */
    virtual void dispatch(qint64 elapsed,
                          int bitrate,
                          quint32 frequency,
                          int precision,
                          int channels);
    /*!
     * Sends metadata \b metaData
     */
    virtual void dispatch(const QMap<Qmmp::MetaData, QString> &metaData);
    /*!
     * Sends playback state.
     */
    virtual void dispatch(Qmmp::State state);
    /*!
     * Sends buffering progress.
     * @param \b percent Indicates the current percentage of buffering completed.
     */
    virtual void dispatchBuffer(int percent);
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
    int frequency();
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
     * Returns a pointer to the first created StateHandler instance.
     */
    static StateHandler* instance();

signals:
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
    void metaDataChanged();
    /*!
     * This signal is emitted when the playback state has changed.
     */
    void stateChanged(Qmmp::State newState);
    /*!
    * Emitted when playback has finished.
    */
    void finished();
    /*!
     *
     */
    void aboutToFinish();
     /*!
     * This signal is emitted when the stream reader fills it's buffer.
     * The argument \b progress indicates the current percentage of buffering completed.
     */
    void bufferingProgress(int progress);

private:
    qint64 m_elapsed;
    quint32 m_frequency;
    bool m_sendAboutToFinish;
    int m_bitrate, m_precision, m_channels;
    static StateHandler* m_instance;
    QMap <Qmmp::MetaData, QString> m_metaData;
    QMap <Qmmp::MetaData, QString> m_cachedMetaData;
    Qmmp::State m_state;
    QMutex m_mutex;
};

#endif
