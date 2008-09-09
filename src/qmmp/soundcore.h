/***************************************************************************
 *   Copyright (C) 2006-2008 by Ilya Kotov                                 *
 *   forkotov02\hotmail.ru                                                 *
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

/**
   \author Ilya Kotov <forkotov02\hotmail.ru>
*/

class QIODevice;

class SoundCore : public QObject
{
    Q_OBJECT
public:
    SoundCore(QObject *parent = 0);

    ~SoundCore();

    /*!
     * Returns the current state.
     *
     * \return the state of the object.
     */

    Qmmp::State state() const;

    /*!
     * Returns length in seconds
     */
    qint64 length() const;

    /*!
     * Sets equalizer settings. Each item of \p bands[] and \p reamp should be
     *  \b -100..100
     */
    void setEQ(int bands[10], const int &preamp);

    /*!
     * Enables equalizer if \p on is \b TRUE or disables it if \p on is \b FALSE
     */
    void setEQEnabled(bool on);

    /*!
     * adds visualization \p visual
     */
    //void addVisualization(Visual *visual);

    /*!
     * shows enabled visualization with the parent widget \p parent
     */
    //void showVisualization(QWidget *parent);

    /*!
     * adds visualization by factory \p factory
     */
    //void addVisual(VisualFactory *factory, QWidget *parent);

    /*!
     * removes visualization by factory \p factory
     */
    //void removeVisual(VisualFactory *factory);

    /*!
     * removes visualization \p visual
     */
    //void removeVisual(Visual *visual);

    /*!
     * reads current volume.
     * \p left contains volume of the left channel.
     * \p right contains volume of the right channel.
     */
    void volume(int *left, int *right);

    /*!
    * updates configuration
    */
    void updateConfig();


    qint64 elapsed();
    int bitrate();
    int frequency();
    int precision();
    int channels();


    /*!
     * Returns a pointer to the SoundCore instance.
     */
    static SoundCore* instance();

public slots:

    /*!
     * Sets volume.
     * \p left - volume of the left channel.
     * \p right - volume of the right channel.
     * \b left and \b right should be \b 0..100.
     */
    void setVolume(int left, int right);

    /*!
     *  This function plays file with the given path \p source.
     *
     *  \return \b TRUE if playback started successful or source is not a local file.
     *  \return \b FALSE otherwise.
     */
    bool play(const QString &source);

    /*!
     *  Stops playback
     */
    void stop();

    /*!
     *  Pauses/resumes playback
     */
    void pause();

    /*!
    *This function sets the current play position to \p pos.
    */
    void seek(qint64 pos);

signals:

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
    void elapsedChanged(qint64 time);
    void bitrateChanged(int bitrate);
    void frequencyChanged(int frequency);
    void precisionChanged(int precision);
    void channelsChanged(int channels);
    void metaDataChanged ();
    void stateChanged (Qmmp::State newState);
    void finished();

private slots:
    //bool decode();
    void setState(Qmmp::State);

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
    Qmmp::State m_state;
};

#endif
