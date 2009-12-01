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
#ifndef MEDIAPLAYER_H
#define MEDIAPLAYER_H

#include <QObject>
#include <qmmp/soundcore.h>
#include "playlistmanager.h"


/*! @brief The MediaPlayer class provides a simple way to use SoundCore and PlayListModel together.
 * @author Ilya Kotov <forkotov02@hotmail.ru>
 */
class MediaPlayer : public QObject
{
    Q_OBJECT
public:
    /*!
     * Object constructor,
     * @param parent Parent object
     */
    MediaPlayer(QObject *parent = 0);
    /*!
     * Destructor
     */
    ~MediaPlayer();
    /*!
     * Returns a pointer to the object's instance.
     */
    static MediaPlayer* instance();
    /*!
     * Prepares object for usage.
     * @param core Pointer to the SoundCore object.
     * @param model Playlist model
     */
    void initialize(SoundCore *core, PlayListManager *pl_mamager);
    /*!
     * Returns playlist manager pointer
     */
    PlayListManager *playListManager();
    /*!
     * Returns \b true if "Repeate Track" option is enabled, otherwise returns \b false
     */
    bool isRepeatable() const;

public slots:
    /*!
     * Starts playback.
     */
    void play();
    /*!
     * Stops playback.
     */
    void stop();
    /*!
     * Sets next playlist item for playing.
     */
    void next();
    /*!
     * Sets previous playlist item for playing.
     */
    void previous();
    /*!
     * Toggles the current track repeat.
     * @param enable Repeate state of the current track (\b true - to repeat, \b false - to stop repeating)
     */
    void setRepeatable(bool enable);

signals:
    /*!
     * Tracks current track repeat state;
     * @param enabled New repeate state of the current track (\b true - enabled, \b false - disabled)
     */
    void repeatableChanged(bool enabled);

private slots:
    void playNext();
    void updateNextUrl();

private:
    PlayListManager *m_pl_manager;
    SoundCore *m_core;
    static MediaPlayer* m_instance;
    bool m_repeat;
    int m_skips;
    QString m_nextUrl;
};

#endif
