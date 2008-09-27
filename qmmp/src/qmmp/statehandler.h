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
#ifndef STATEHANDLER_H
#define STATEHANDLER_H

#include <QObject>
#include <QMap>
#include <QMutex>

#include "qmmp.h"

/**
    @author Ilya Kotov <forkotov02@hotmail.ru>
*/
class StateHandler : public QObject
{
    Q_OBJECT
public:
    StateHandler(QObject *parent = 0);

    ~StateHandler();

    void dispatch(qint64 elapsed,
                  qint64 totalTime,
                  int bitrate,
                  int frequency,
                  int precision,
                  int channels);

    void dispatch(const QMap<Qmmp::MetaData, QString> &metaData);

    void dispatch(const Qmmp::State &state);

    qint64 elapsed();
    int bitrate();
    int frequency();
    int precision();
    int channels();
    Qmmp::State state();
    QMap <Qmmp::MetaData, QString> metaData();
    QString metaData(Qmmp::MetaData key);

    /*!
     * Returns a pointer to the StateHandler instance.
     */
    static StateHandler* instance();

signals:
    void elapsedChanged(qint64 time);
    void bitrateChanged(int bitrate);
    void frequencyChanged(int frequency);
    void precisionChanged(int precision);
    void channelsChanged(int channels);
    void metaDataChanged ();
    void stateChanged (Qmmp::State newState);
    void finished();

private:
    qint64 m_elapsed;
    int m_bitrate, m_frequency, m_precision, m_channels;
    static StateHandler* m_instance;
    QMap <Qmmp::MetaData, QString> m_metaData;
    Qmmp::State m_state;
    QMutex m_mutex;
    bool m_sendMeta;

};

#endif
