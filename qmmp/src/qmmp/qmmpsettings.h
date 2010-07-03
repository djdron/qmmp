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
#include "eqsettings.h"

/*! @brief The QmmpSettings class provides access to global settings.
 * @author Ilya Kotov <forkotov02@hotmail.ru>
 */
class QmmpSettings : public QObject
{
Q_OBJECT
public:
    /*!
     * Constructor.
     * @param parent Parent object.
     * This functions is for internal usage only, use QmmpSettings::instance() instead.
     */
    QmmpSettings(QObject *parent = 0);
    /*!
     * Destructor.
     */
    ~QmmpSettings();
    /*!
     * This enum describes possible replaygain modes.
     */
    enum ReplayGainMode
    {
        REPLAYGAIN_TRACK = 0, /*!< Use track gain/peak */
        REPLAYGAIN_ALBUM,     /*!< Use album gain/peak */
        REPLAYGAIN_DISABLED   /*!< Disable ReplayGain */
    };
    /*!
     * Returns current ReplayGain mode.
     */
    QmmpSettings::ReplayGainMode replayGainMode() const;
    /*!
     * Returns preamp in dB.
     */
    double replayGainPreamp() const;
    /*!
     * Returns default gain in dB.
     */
    double replayGainDefaultGain() const;
    /*!
     * Returns \b true if clipping prevention is enabled; otherwise returns \b false.
     */
    bool replayGainPreventClipping() const;
    /*!
     * Sets ReplayGains settings.
     * @param mode ReplayGain mode.
     * @param preamp Preamp in dB.
     * @param default_gain Default gain in dB.
     * @param clip Clipping prevention state.
     */
    void setReplayGainSettings(ReplayGainMode mode, double preamp, double default_gain, bool clip);
    /*!
     * Returns \b true if software volume is enabled; otherwise returns \b false.
     */
    bool useSoftVolume() const;
    /*!
     * Returns \b true if 16-bit converter is enabled; otherwise returns \b false.
     */
    bool use16BitOutput() const;
    /*!
     * Sets audio settings.
     * @param soft_volume State of software volume.
     * @param use_16bit State of the 16-bit audio converter.
     */
    void setAudioSettings(bool soft_volume, bool use_16bit);
    /*!
     * If \b include is \b true, this function returns include cover file name filters,
     * otherwise returns exclude filters.
     */
    QStringList coverNameFilters(bool include = true) const;
    /*!
     * Returns a depth of recursive cover file search.
     */
    int coverSearchDepth() const;
    /*!
     * Returns \b true if cover file search is enabled; otherwise returns \b false.
     */
    bool useCoverFiles() const;
    /*!
     * Sets cover search options.
     * @param inc Include cover name filters
     * @param exc Exclude cover name filters
     * @param depth Depth of recursive cover file search.
     * Recursive cover file search can be disabled by setting \b depth to \b 0.
     * @param use_files Use or not use files with covers.
     * This parameter doesn't take effect in embedded covers.
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
    /*!
     * Sets network settings.
     * @param use_proxy Enables or disables global proxy.
     * @param auth Enables or disables proxy authentication.
     * @param proxy Proxy url.
     */
    void setNetworkSettings(bool use_proxy, bool auth, const QUrl &proxy);


    EqSettings eqSettings() const;
    void setEqSettings(const EqSettings &settings);
    /*!
     * Returns a pointer to the QmmpSettings instance.
     */
    static QmmpSettings* instance();

signals:
    /*!
     * Emitted when ReplayGain settings are changed.
     */
    void replayGainSettingsChanged();
    /*!
     * Emitted when audio settings are changed.
     */
    void audioSettingsChanged();
    /*!
     * Emitted when cover settings are changed.
     */
    void coverSettingsChanged();
    /*!
     * Emitted when network settings are changed.
     */
    void networkSettingsChanged();
    /*!
     * Emitted when equalizer settings are changed.
     */
    void eqSettingsChanged();

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
    //equalizer settings
    EqSettings m_eq_settings;

    static QmmpSettings* m_instance;

};

#endif // QMMPSETTINGS_H
