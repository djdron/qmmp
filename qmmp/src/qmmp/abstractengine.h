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

#ifndef ABSTRACTENGINE_H
#define ABSTRACTENGINE_H


#include <QMutex>
#include <QWaitCondition>
#include <QThread>

class QIODevice;

/*!
 * @author Ilya Kotov <forkotov02@hotmail.ru>
 */

class AbstractEngine : public QThread
{
 Q_OBJECT
public:
    AbstractEngine(QObject *parent = 0);

    /*!
     * Prepares decoder for usage.
     * Subclass should reimplement this function.
     */
    virtual bool initialize(const QString &source, QIODevice *input = 0) = 0;
    /*!
     * Returns the total time in milliseconds.
     * Subclass should reimplement this function.
     */
    virtual qint64 totalTime() = 0;
    /*!
     * Requests a seek to the time \b time indicated, specified in milliseconds.
     */
    virtual void seek(qint64 time) = 0;
    /*!
     * Requests playback to stop
     */
    virtual void stop() = 0;
    /*!
     * Returns current bitrate (in kbps).
     * Subclass should reimplement this function.
     */
    //virtual int bitrate();
    /*!
     * Requests playback to pause. If it was paused already, playback should resume.
     * Subclass with own output should reimplement this function.
     */
    //virtual void pause();
    /*!
     * Returns decoder input or 0 if input is not specified.
     */
    //QIODevice *input();
    /*!
     * Returns decoder output or 0 if output is not specified.
     */
    //Output *output();
    /*!
     * Returns mutex pointer.
     */
    QMutex *mutex();
    /*!
     * Returns wait condition pointer.
     */
    QWaitCondition *cond();
    /*!
     * Sets equalizer settings. Each item of \p bands[] and \p reamp should be \b -20.0..20.0
     * Subclass with own equalizer should reimplement this function.
     */
    //virtual void setEQ(double bands[10], double preamp);
    /*!
     * Enables equalizer if \p on is \b true or disables it if \p on is \b false
     * Subclass with own equalizer should reimplement this function.
     */
    //virtual void setEQEnabled(bool on);
    /*!
     * Returns \b true if \b file is supported by input plugins, otherwise returns \b false
     */
    //bool supports(const QString &file);

signals:
    /*!
     * Emitted when the decoder has finished playback.
     */
    void playbackFinished();

protected:
    /*!
     * The starting point for the decoding thread.
     */
    virtual void run() = 0;

protected slots:
    /*!
     * Subclass should call this slot when decoding is finished.
     */
    //void finish();

private:
    QMutex m_mutex;
    QWaitCondition m_waitCondition;
};


#endif // ABSTRACTENGINE_H
