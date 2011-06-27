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

#ifndef ABSTRACTENGINE_H
#define ABSTRACTENGINE_H

#include <QMutex>
//#include <QWaitCondition>
#include <QThread>
#include <QStringList>
#include "enginefactory.h"
#include "qmmpsettings.h"

class QIODevice;
class InputSource;


/*! @brief The AbstractEngine class provides the base interface class of audio audio engines.
 * @author Ilya Kotov <forkotov02@hotmail.ru>
 */
class AbstractEngine : public QThread
{
 Q_OBJECT
public:
    /*!
     * Object contsructor.
     * @param parent Parent object.
     */
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
    //QWaitCondition *cond();
    /*!
     * Creates Engine object.
     * @param s InputSource object.
     * @param parent Parent object.
     * Returns \b 0 if the given source is not supported.
     */
    static AbstractEngine *create(InputSource *s, QObject *parent = 0);
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
    /*!
     * Returns a list of supported protocols (including meta-protocols).
     * This fuction ignores disabled engines.
     */
    static QStringList protocols();

signals:
    /*!
     * Emitted when the decoder has finished playback.
     */
    //void playbackFinished();

private:
    QMutex m_mutex;
    //QWaitCondition m_waitCondition;

    static void checkFactories();
    static QList<EngineFactory*> *m_factories;
    static QList<EngineFactory*> *m_disabledFactories;
    static QStringList m_files;
};


#endif // ABSTRACTENGINE_H
