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

#include <QApplication>
#include <QSettings>
extern "C"{
#include <wildmidi_lib.h>
}
#include <qmmp/qmmp.h>
#include "wildmidihelper.h"

WildMidiHelper *WildMidiHelper::m_instance = 0;

WildMidiHelper::WildMidiHelper(QObject *parent) :
    QObject(parent)
{
    m_inited = false;
    m_sample_rate = 0;
}

WildMidiHelper::~WildMidiHelper()
{
    if(m_inited)
        WildMidi_Shutdown();
    m_instance = 0;
}

bool WildMidiHelper::initialize()
{
    m_mutex.lock();
    if(m_inited)
    {
        m_mutex.unlock();
        return true;
    }

    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("Midi");
    unsigned short int mixer_options = 0;
    QString conf_path = settings.value("conf_path", "/etc/timidity/timidity.cfg").toString();
    unsigned short int sample_rate = settings.value("sample_rate", 44100).toInt();
    if(settings.value("enhanced_resampling", false).toBool())
        mixer_options |= WM_MO_ENHANCED_RESAMPLING;
    if(settings.value("reverberation", false).toBool())
        mixer_options |= WM_MO_REVERB;
    settings.endGroup();

    m_sample_rate = sample_rate;
    if (WildMidi_Init (qPrintable(conf_path), sample_rate, mixer_options) < 0)
    {
        qWarning("WildMidiHelper: unable to initialize WildMidi library");
        m_mutex.unlock();
        return false;
    }
    m_inited = true;
    m_mutex.unlock();
    return true;
}

void WildMidiHelper::readSettings()
{
    m_mutex.lock();
    if(!m_ptrs.isEmpty())
    {
        m_mutex.unlock();
        return;
    }
    if(m_inited)
        WildMidi_Shutdown();
    m_inited = false;
    m_mutex.unlock();
    initialize();
}

void WildMidiHelper::addPtr(void *t)
{
    m_mutex.lock();
    m_ptrs.append(t);
    m_mutex.unlock();
}

void WildMidiHelper::removePtr(void *t)
{
    m_mutex.lock();
    m_ptrs.removeAll(t);
    m_mutex.unlock();
}

quint32 WildMidiHelper::sampleRate()
{
    return m_sample_rate;
}

WildMidiHelper *WildMidiHelper::instance()
{
    if(!m_instance)
        m_instance = new WildMidiHelper(qApp);
    return m_instance;
}
