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
#include <QStringList>
#include "enginefactory.h"
#include "qmmpsettings.h"

class QIODevice;
class InputSource;


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
    virtual bool enqueue(InputSource *source) = 0;
    /*!
     * Returns the total time in milliseconds.
     * Subclass should reimplement this function.
     */
    virtual qint64 totalTime() = 0;
    /*!
     * Starts playback. Returns \b true if playback has been started successful,
     * otherwise returns \b false.
     */
    virtual bool play() = 0;
    /*!
     * Requests a seek to the time \b time indicated, specified in milliseconds.
     */
    virtual void seek(qint64 time) = 0;
    /*!
     * Requests playback to stop
     */
    virtual void stop() = 0;
    /*!
     *  Pauses/resumes playback
     */
    virtual void pause() = 0;
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
    virtual void setEQ(double bands[10], double preamp) = 0;
    /*!
     * Enables equalizer if \p on is \b true or disables it if \p on is \b false
     * Subclass with own equalizer should reimplement this function.
     */
    virtual void setEQEnabled(bool on) = 0;
    /*!
     * Returns a list of decoder factories.
     */
    static QList<EngineFactory*> *factories();
    /*!
     * Returns EngineFactory pointer which supports file \b path or 0 if file \b path is unsupported
     */
    static EngineFactory *findByPath(const QString &path);
    /*!
     * Sets whether the engine is enabled.
     * @param factory Engine plugin factory.
     * @param enable Plugin enable state (\b true - enable, \b false - disable)
     */
    static void setEnabled(EngineFactory* factory, bool enable = true);
    /*!
     * Returns \b true if engine is enabled, otherwise returns \b false
     * @param factory Engine plugin factory.
     */
    static bool isEnabled(EngineFactory* factory);
    /*!
     * Returns a list of engine plugin files.
     */
    static QStringList files();

signals:
    /*!
     * Emitted when the decoder has finished playback.
     */
    void playbackFinished();

private:
    QMutex m_mutex;
    QWaitCondition m_waitCondition;

    static void checkFactories();
    static QList<EngineFactory*> *m_factories;
    static QStringList m_files;
};


#endif // ABSTRACTENGINE_H
