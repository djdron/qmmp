/***************************************************************************
 *   Copyright (C) 2007 by Ilya Kotov                                      *
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
#include <QDir>

#include <decoderfactory.h>
#include <outputfactory.h>
#include <visualfactory.h>
#include <soundcore.h>

#include "pluginitem.h"

/*Input*/
InputPluginItem::InputPluginItem(QObject *parent, DecoderFactory *fact,
                                                     const QString &filePath)
        : QObject(parent)
{
    m_fileName = filePath.section('/',-1);
    m_factory = fact;
}

InputPluginItem::~InputPluginItem()
{}

bool InputPluginItem::isSelected()
{
    QSettings settings (QDir::homePath() +"/.qmmp/qmmprc", QSettings::IniFormat);
    QStringList blacklist = settings.value("Decoder/disabled_plugins").toStringList();
    return !blacklist.contains(m_fileName);
}

DecoderFactory* InputPluginItem::factory()
{
    return m_factory;
}

void InputPluginItem::setSelected(bool select)
{
    QSettings settings (QDir::homePath() +"/.qmmp/qmmprc", QSettings::IniFormat);
    QStringList blacklist = settings.value("Decoder/disabled_plugins").toStringList();
    if (select)
        blacklist.removeAll (m_fileName);
    else
        blacklist.append (m_fileName);
    settings.setValue("Decoder/disabled_plugins", blacklist);
}

/*Output*/
OutputPluginItem::OutputPluginItem(QObject *parent, OutputFactory *fact,
                                   const QString &filePath): QObject(parent)
{
    m_fileName = filePath.section('/',-1);
    m_factory = fact;
}


OutputPluginItem::~OutputPluginItem()
{}

void OutputPluginItem::select()
{
    QSettings settings (QDir::homePath() +"/.qmmp/qmmprc", QSettings::IniFormat);
    settings.setValue("Output/plugin_file", m_fileName);
}

bool OutputPluginItem::isSelected()
{
    QSettings settings (QDir::homePath() +"/.qmmp/qmmprc", QSettings::IniFormat);
    return m_fileName == settings.value("Output/plugin_file","libalsa.so").toString();
}

OutputFactory *OutputPluginItem::factory()
{
   return m_factory;
}

/*Visual*/
VisualPluginItem::VisualPluginItem(QObject *parent, VisualFactory *fact,
                                   const QString &filePath): QObject(parent)
{
    m_fileName = filePath.section('/',-1);
    m_factory = fact;
}


VisualPluginItem::~VisualPluginItem()
{}

void VisualPluginItem::select(bool on)
{
    if(on)
        SoundCore::instance()->addVisual(m_factory, 0);
    else
        SoundCore::instance()->removeVisual(m_factory);
}

bool VisualPluginItem::isSelected()
{
    return Visual::isEnabled(m_factory);
}

VisualFactory *VisualPluginItem::factory()
{
   return m_factory;
}
