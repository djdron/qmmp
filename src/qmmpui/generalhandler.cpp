/***************************************************************************
 *   Copyright (C) 2008 by Ilya Kotov                                      *
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

#include "general.h"
#include "generalfactory.h"

#include "generalhandler.h"

GeneralHandler *GeneralHandler::m_instance = 0;

GeneralHandler::GeneralHandler(QObject *parent)
        : QObject(parent)
{
    m_instance = this;
    m_state = General::Stopped;
    GeneralFactory* factory;
    foreach(factory, *General::generalFactories())
    {
        if (General::isEnabled(factory))
        {
            General *general = factory->create(parent);
            connect(general, SIGNAL(commandCalled(uint)), SLOT(processCommand(uint)));
            m_generals.append(general);
        }
    }
}

GeneralHandler::~GeneralHandler()
{}

void GeneralHandler::setState(uint state)
{
    if (state == m_state)
        return;
    m_state = state;
    General *general;
    if (state == General::Stopped)
        m_songInfo.clear();

    foreach(general, m_generals)
    {
        general->setState(state);
    }
}

void GeneralHandler::setSongInfo(const SongInfo &info)
{
    if (m_state == General::Stopped)
        return;
    if (m_songInfo != info)
    {
        m_songInfo = info;
        General *general;
        foreach(general, m_generals)
        {
            general->setSongInfo(m_songInfo);
        }
    }
}

void GeneralHandler::updateConfig()
{
    while (!m_generals.isEmpty())
        delete m_generals.takeFirst();

    GeneralFactory* factory;
    foreach(factory, *General::generalFactories())
    {
        if (General::isEnabled(factory))
        {
            General *general = factory->create(parent());
            connect(general, SIGNAL(commandCalled(uint)), SLOT(processCommand(uint)));
            m_generals.append(general);
            general->setState(m_state);
            if (m_state != General::Stopped)
                general->setSongInfo(m_songInfo);
        }
    }
}

GeneralHandler* GeneralHandler::instance()
{
    return m_instance;
}

void GeneralHandler::processCommand(uint command)
{
    switch ((uint) command)
    {
    case General::Play:
    {
        emit playCalled();
        break;
    }
    case General::Stop:
    {
        emit stopCalled();
        break;
    }
    case General::Pause:
    {
        emit pauseCalled();
        break;
    }
    case General::Previous:
    {
        emit previousCalled();
        break;
    }
    case General::Next:
    {
        emit nextCalled();
        break;
    }
    case General::Exit:
    {
        emit exitCalled();
        break;
    }
    case General::ToggleVisibility:
    {
        emit toggleVisibilityCalled();
        break;
    }
    }
}
