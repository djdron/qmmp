/***************************************************************************
 *   Copyright (C) 2006-2208 by Ilya Kotov                                 *
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

/**
   @author Ilya Kotov <forkotov02@hotmail.ru>
*/

class QIODevice;

class SoundCore : public QObject
{
    Q_OBJECT
public:

    /*! This enum describes the errors that may be returned by the error() function.
     *  Available values is:
     *  \b SoundCore:NoError - no error occurred,
     *  \b SoundCore:DecoderError - an error occurred when creating decoder,
     *  \b SoundCore:OutputError - an error occurred when creating output.
     */
    enum ErrorType { NoError = 0, DecoderError, OutputError };

    SoundCore(QObject *parent = 0);

    ~SoundCore();

    // properties

    /*!
    *  Returns the playback error status.
    *  For example, if play() returns false, this function can be called to find out
    *  the reason why the operation failed.
    */
    uint error();

    /*!
    * Returns length in seconds
    */
    int length();

    /*!
    * Returns \b TRUE if \b play() called successful, otherwise \b FALSE.
    */
    bool isReady();

    /*!
    * Returns \b TRUE if \b play() called successful, otherwise \b FALSE.
    * Also this function returns \b FALSE if \b stop() called before
    */
    bool isInitialized();

    /*!
    * Returns \b TRUE if plugins in pause mode, otherwise \b FALSE.
    */
    bool isPaused();

    //equalizer

    /*!
    * Sets equalizer settings. Each item of \b bands[] and \b reamp should be
    *  \b -100..100
    */
    void setEQ(int bands[10], const int &preamp);

    /*!
    * Enables equalizer if on is \b TRUE or disables it if on is \b FALSE
    */
    void setEQEnabled(bool on);

    //volume

    void volume(int*, int*);

    //config

    /*!
    * updates configuration
    */
    void updateConfig();

    //visualization
    /*!
    * adds visualization \b visual
    */
    void addVisualization(Visual *visual);

    /*!
    * shows enabled visualization with parent widget \b parent
    */
    void showVisualization(QWidget *parent);

    /*!
    * adds visualization by factory \b factory
    */
    void addVisual(VisualFactory *factory, QWidget *parent);

    /*!
    * removes visualization by factory \b factory
    */
    void removeVisual(VisualFactory *factory);

    /*!
    * removes visualization \b visual
    */
    void removeVisual(Visual *visual);

    static SoundCore* instance();

public slots:

    /*!
    * Sets volume. \b L - volume of left channel. \b R - volume of right channel.
    * \b L and \b R should be 0..100
    */
    void setVolume(int L, int R);

    //control

    /*!
    *  This function plays file with given path, returns \b TRUE if all is OK, otherwise \b FALSE
    */
    bool play(const QString &source);


    /*!
    *  Stops playing
    */
    void stop();

    /*!
    *  Pauses/resumes playing
    */
    void pause();

    /*!
    *This function sets the current play position to \b pos
    */
    void seek(int pos);


signals:

    /*!
    *  This signal is emited when the state of the decoder changes.
    *  The argument \b state is the new state of the decoder
    */
    void decoderStateChanged(const DecoderState& state);

    /*!
    *  This signal is emited when the state of the output changes.
    *  The argument \b state is the new state of the output
    */
    void outputStateChanged(const OutputState& state);

    /*!
    *  This signal is emited when the title of the stream changes.
    *  The argument \b title is the new title of the stream.
    *  Signal emits with the shoutcast server stream only.
    *  For another servers use decoderStateChanged()
    */
    void titleChanged(const QString& title);

    /*!
     * This signal is emited when the stream reader fills it's buffer.
     * The argument \b progress indicates the current percentage of buffering completed
     */

    void bufferingProgress(int progress);

private slots:
    bool decode();

private:
    Decoder* m_decoder;
    Output* m_output;
    QIODevice* m_input;
    uint m_error;
    bool m_paused;
    bool m_useEQ;
    bool m_update;
    bool m_block;
    int m_preamp;
    int m_bands[10];
    Visual *m_vis;
    QList <Visual*> m_visuals;
    QString m_source;
    QWidget *m_parentWidget;
    static SoundCore* m_instance;
};

#endif
