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

#include <QSettings>
#include <QApplication>
#include "metadatamanager.h"
#include "qmmp.h"
#include "qmmpsettings.h"

QmmpSettings *QmmpSettings::m_instance = 0;

QmmpSettings::QmmpSettings(QObject *parent) : QObject(parent)
{
    m_instance = this;
    QSettings settings (Qmmp::configFile(), QSettings::IniFormat);
    //replaygain settings
    settings.beginGroup("ReplayGain");
    m_rg_mode = (ReplayGainMode) settings.value("mode", REPLAYGAIN_DISABLED).toInt();
    m_rg_preamp = settings.value("preamp", 0.0).toDouble();
    m_rg_defaut_gain = settings.value("default_gain", 0.0).toDouble();
    m_rg_prevent_clipping = settings.value("prevent_clipping", false).toBool();
    settings.endGroup();
    //audio settings
    m_aud_software_volume = settings.value("Output/software_volume", false).toBool();
    m_aud_16bit = settings.value("Output/use_16bit", false).toBool();
    //cover settings
    settings.beginGroup("Cover");
    m_cover_inc = settings.value("include", (QStringList() << "*.jpg" << "*.png")).toStringList();
    m_cover_exclude = settings.value("exclude", (QStringList() << "*back*")).toStringList();
    m_cover_depth = settings.value("depth", 0).toInt();
    m_cover_use_files = settings.value("use_files", true).toBool();
    settings.endGroup();
    //network settings
    m_proxy_enabled = settings.value("Proxy/use_proxy", false).toBool();
    m_proxy_auth = settings.value("Proxy/authentication", false).toBool();
    m_proxy_url = settings.value("Proxy/url").toUrl();
    //equalizer settings
    for (int i = 0; i < 10; ++i)
        m_eq_settings.setGain(i, settings.value("Equalizer/band_"+ QString("%1").arg(i), 0).toDouble());
    m_eq_settings.setPreamp(settings.value("Equalizer/preamp", 0).toDouble());
    m_eq_settings.setEnabled(settings.value("Equalizer/enabled", true).toBool());
}

QmmpSettings::~QmmpSettings()
{
    sync();
    m_instance = 0;
}

QmmpSettings::ReplayGainMode QmmpSettings::replayGainMode() const
{
    return m_rg_mode;
}

double QmmpSettings::replayGainPreamp() const
{
    return m_rg_preamp;
}

double QmmpSettings::replayGainDefaultGain() const
{
    return m_rg_defaut_gain;
}

bool QmmpSettings::replayGainPreventClipping() const
{
    return m_rg_prevent_clipping;
}

void QmmpSettings::setReplayGainSettings(ReplayGainMode mode, double preamp, double def_gain, bool clip)
{
    m_rg_mode = mode;
    m_rg_preamp = preamp;
    m_rg_defaut_gain = def_gain;
    m_rg_prevent_clipping = clip;
    emit replayGainSettingsChanged();
}

bool QmmpSettings::useSoftVolume() const
{
    return m_aud_software_volume;
}

bool QmmpSettings::use16BitOutput() const
{
    return m_aud_16bit;
}

void QmmpSettings::setAudioSettings(bool soft_volume, bool use_16bit)
{
    m_aud_software_volume = soft_volume;
    m_aud_16bit = use_16bit;
    emit audioSettingsChanged();
}

QStringList QmmpSettings::coverNameFilters(bool include) const
{
    return include ? m_cover_inc : m_cover_exclude;
}

int QmmpSettings::coverSearchDepth() const
{
    return m_cover_depth;
}

bool QmmpSettings::useCoverFiles() const
{
    return m_cover_use_files;
}

void QmmpSettings::setCoverSettings(QStringList inc, QStringList exc, int depth, bool use_files)
{
    m_cover_inc = inc;
    m_cover_exclude = exc;
    m_cover_depth = depth;
    m_cover_use_files = use_files;
    MetaDataManager::instance()->clearCoverChache();
    emit coverSettingsChanged();
}

bool QmmpSettings::isProxyEnabled() const
{
    return m_proxy_enabled;
}

bool QmmpSettings::useProxyAuth() const
{
    return m_proxy_auth;
}

QUrl QmmpSettings::proxy() const
{
    return  m_proxy_url;
}

void QmmpSettings::setNetworkSettings(bool use_proxy, bool auth, const QUrl &proxy)
{
    m_proxy_enabled = use_proxy;
    m_proxy_auth = auth;
    m_proxy_url = proxy;
    emit networkSettingsChanged();
}

EqSettings QmmpSettings::eqSettings() const
{
    return m_eq_settings;
}

void QmmpSettings::setEqSettings(const EqSettings &settings)
{
    m_eq_settings = settings;
    emit eqSettingsChanged();
}

void QmmpSettings::sync()
{
    QSettings settings (Qmmp::configFile(), QSettings::IniFormat);
    //replaygain settings
    settings.beginGroup("ReplayGain");
    settings.setValue("mode", m_rg_mode);
    settings.setValue("preamp", m_rg_preamp);
    settings.setValue("default_gain", m_rg_defaut_gain);
    settings.setValue("prevent_clipping", m_rg_prevent_clipping);
    settings.endGroup();
    //audio settings
    settings.setValue("Output/software_volume", m_aud_software_volume);
    settings.setValue("Output/use_16bit", m_aud_16bit);
    //cover settings
    settings.beginGroup("Cover");
    settings.setValue("include", m_cover_inc);
    settings.setValue("exclude", m_cover_exclude);
    settings.setValue("depth", m_cover_depth);
    settings.setValue("use_files", m_cover_use_files);
    settings.endGroup();
    //network settings
    settings.setValue("Proxy/use_proxy", m_proxy_enabled);
    settings.setValue("Proxy/authentication", m_proxy_auth);
    settings.setValue("Proxy/url", m_proxy_url);
    //equalizer settings
    for (int i = 0; i < 10; ++i)
        settings.setValue("Equalizer/band_"+ QString("%1").arg(i), m_eq_settings.gain(i));
    settings.setValue("Equalizer/preamp", m_eq_settings.preamp());
    settings.setValue("Equalizer/enabled", m_eq_settings.isEnabled());
}

QmmpSettings* QmmpSettings::instance()
{
    if(!m_instance)
        return new QmmpSettings(qApp);
    return m_instance;
}
