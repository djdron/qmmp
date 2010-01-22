/***************************************************************************
 *   Copyright (C) 2010 by Ilya Kotov                                      *
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

#ifndef QMMPSETTINGS_H
#define QMMPSETTINGS_H

#include <QObject>
#include <QUrl>
#include <QStringList>

/*!
 * @author Ilya Kotov <forkotov02@hotmail.ru>
 */
class QmmpSettings : public QObject
{
Q_OBJECT
public:
    QmmpSettings(QObject *parent = 0);
    ~QmmpSettings();

    enum ReplayGainMode
    {
        REPLAYGAIN_TRACK = 0,
        REPLAYGAIN_ALBUM,
        REPLAYGAIN_DISABLED
    };

    QmmpSettings::ReplayGainMode replayGainMode() const;
    double replayGainPreamp() const;
    double replayGainDefaultGain() const;
    bool replayGainPreventClipping() const;
    void setReplayGainSettings(ReplayGainMode mode, double preamp, double default_gain, bool clip);

    bool useSoftVolume() const;
    bool use16BitOutput() const;
    void setAudioSettings(bool soft_volume, bool use_16bit);
    /*!
     * If \b include is \b true, this function returns include cover file name filters,
     * otherwise returns exclude filters.
     */
    QStringList coverNameFilters(bool include = TRUE) const;
    /*!
     * Returns a depth of recursive cover file search.
     */
    int coverSearchDepth() const;
    bool useCoverFiles() const;
    /*!
     * Sets cover search options.
     * @param inc Include cover name filters
     * @param exc Exclude cover name filters
     * @param depth Depth of recursive cover file search.
     * Recursive cover file search can be disabled by setting \b depth to \b 0.
     */
    void setCoverSettings(QStringList inc, QStringList exc, int depth, bool use_files);
    /*!
     * Returns \b true if global proxy is enabled, otherwise returns \b false
     */
    bool isProxyEnabled() const;
    /*!
     * Returns \b true if global proxy authentication is enabled, otherwise returns \b false
     */
    bool useProxyAuth() const;
    /*!
     * Returns global proxy url.
     */
    QUrl proxy() const;
    void setNetworkSettings(bool use_proxy, bool auth, const QUrl &proxy);


    static QmmpSettings* instance();

signals:
    void replayGainSettingsChanged();
    void audioSettingsChanged();
    void coverSettingsChanged();
    void networkSettingsChanged();

private slots:
    void sync();

private:
    //replaygain settings
    QmmpSettings::ReplayGainMode m_rg_mode;
    double m_rg_preamp;
    double m_rg_defaut_gain;
    bool m_rg_prevent_clipping;
    //audio settings
    bool m_aud_software_volume;
    bool m_aud_16bit;
    //cover settings
    QStringList m_cover_inc;
    QStringList m_cover_exclude;
    int m_cover_depth;
    bool m_cover_use_files;
    //network settings
    bool m_proxy_enabled;
    bool m_proxy_auth;
    QUrl m_proxy_url;

    static QmmpSettings* m_instance;

};

#endif // QMMPSETTINGS_H
