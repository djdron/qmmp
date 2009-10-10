/***************************************************************************
 *   Copyright (C) 2007-2008 by Ilya Kotov                                 *
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

#include <qmmp/decoderfactory.h>
#include <qmmp/outputfactory.h>
#include <qmmp/visualfactory.h>
#include <qmmp/effectfactory.h>
#include <qmmp/effect.h>
#include <qmmp/soundcore.h>
#include <qmmp/enginefactory.h>
#include <qmmp/abstractengine.h>
#include <qmmpui/generalfactory.h>
#include <qmmpui/general.h>
#include <qmmpui/generalhandler.h>

#include "pluginitem.h"

/*Input*/
InputPluginItem::InputPluginItem(QObject *parent, DecoderFactory *fact)
        : QObject(parent)
{
    m_factory = fact;
}

InputPluginItem::~InputPluginItem()
{}

bool InputPluginItem::isSelected()
{
    return Decoder::isEnabled(m_factory);
}

DecoderFactory* InputPluginItem::factory()
{
    return m_factory;
}

void InputPluginItem::setSelected(bool select)
{
    Decoder::setEnabled(m_factory, select);
}

/*Engines*/
EnginePluginItem::EnginePluginItem(QObject *parent, EngineFactory *fact)
        : QObject(parent)
{
    m_factory = fact;
}

EnginePluginItem::~EnginePluginItem()
{}

bool EnginePluginItem::isSelected()
{
    return AbstractEngine::isEnabled(m_factory);
}

EngineFactory* EnginePluginItem::factory()
{
    return m_factory;
}

void EnginePluginItem::setSelected(bool select)
{
    AbstractEngine::setEnabled(m_factory, select);
}

/*Output*/
OutputPluginItem::OutputPluginItem(QObject *parent, OutputFactory *fact): QObject(parent)
{
    m_factory = fact;
}


OutputPluginItem::~OutputPluginItem()
{}

void OutputPluginItem::select()
{
    Output::setCurrentFactory(m_factory);
}

bool OutputPluginItem::isSelected()
{
    return Output::currentFactory() == m_factory;
}

OutputFactory *OutputPluginItem::factory()
{
    return m_factory;
}

/*Visual*/
VisualPluginItem::VisualPluginItem(QObject *parent, VisualFactory *fact): QObject(parent)
{
    m_factory = fact;
}


VisualPluginItem::~VisualPluginItem()
{}

void VisualPluginItem::select(bool on)
{
    Visual::setEnabled(m_factory, on);
}

bool VisualPluginItem::isSelected()
{
    return Visual::isEnabled(m_factory);
}

VisualFactory *VisualPluginItem::factory()
{
    return m_factory;
}

/*Effect*/
EffectPluginItem::EffectPluginItem(QObject *parent, EffectFactory *fact): QObject(parent)
{
    m_factory = fact;
}


EffectPluginItem::~EffectPluginItem()
{}

void EffectPluginItem::select(bool on)
{
    Effect::setEnabled(m_factory, on);
}

bool EffectPluginItem::isSelected()
{
    return Effect::isEnabled(m_factory);
}

EffectFactory *EffectPluginItem::factory()
{
    return m_factory;
}

/*General*/
GeneralPluginItem::GeneralPluginItem(QObject *parent, GeneralFactory *fact): QObject(parent)
{
    m_factory = fact;
}

GeneralPluginItem::~GeneralPluginItem()
{}

void GeneralPluginItem::select(bool on)
{
    GeneralHandler::instance()->setEnabled(m_factory, on);
}

bool GeneralPluginItem::isSelected()
{
    return General::isEnabled(m_factory);
}

GeneralFactory *GeneralPluginItem::factory()
{
    return m_factory;
}

