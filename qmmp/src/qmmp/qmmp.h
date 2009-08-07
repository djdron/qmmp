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
#ifndef QMMP_H
#define QMMP_H

#include <QUrl>

/*! @brief The Qmmp class stores global settings and enums.
 * @author Ilya Kotov <forkotov02@hotmail.ru>
 */
class Qmmp
{
public:
    /*!
     * Playback state enum
     */
    enum State
    {
        Playing = 0, /*!< The player is playing source */
        Paused,      /*!< The player has currently paused its playback */
        Stopped,     /*!< The player is ready to play source */
        Buffering,   /*!< The Player is waiting for data to be able to start playing.   */
        NormalError, /*!< Input source is invalid or unsupported. Player should skip this file */
        FatalError   /*!< This means unrecorvable error die audio output problems. Player should abort playback. */
    };
    /*!
     * Metadata keys
     */
    enum MetaData
    {
        TITLE = 0, /*!< Title */
        ARTIST,    /*!< Artist  */
        ALBUM,     /*!< Album */
        COMMENT,   /*!< Comment */
        GENRE,     /*!< Genre */
        COMPOSER,  /*!< Composer */
        YEAR,      /*!< Year */
        TRACK,     /*!< Track number */
        DISCNUMBER,/*!< Disc number */
        URL        /*!< Stream url or local file path */
    };
    /*!
     * Returns configuration file path.
     */
    static const QString configFile();
    /*!
     * Overrides default configuration file path.
     */
    static void setConfigFile(const QString &path);
    /*!
     * Returns %Qmmp library version.
     */
    static const QString strVersion();
    /*!
     * Returns \b true if global proxy is enabled, otherwise returns \b false
     */
    static bool useProxy();
    /*!
     * Returns \b true if global proxy authentication is enabled, otherwise returns \b false
     */
    static bool useProxyAuth();
    /*!
     * Returns global proxy url.
     */
    static const QUrl proxy();
    /*!
     * Enables or disables global proxy.
     * @param yes Proxy enable state (\b true - enabled, \b false - disabled)
     */
    static void setProxyEnabled(bool yes);
    /*!
     * Enables or disables global proxy authentication.
     * @param yes Proxy authentication enable state (\b true - enabled, \b false - disabled)
     */
    static void setProxyAuthEnabled(bool yes);
    /*!
     * Sets global proxy url to \b proxy
     */
    static void setProxy (const QUrl &proxy);
    /*!
     * Returns the location of the installed Qmmp plugins.
     */
    static const QString pluginsPath();
    /*!
     * Returns system language
     */
    static QString systemLanguageID();
    /*!
     * Returns global buffer size in bytes
     */
    static unsigned int globalBufferSize();


private:
    static QString m_configFile;

};

#endif
