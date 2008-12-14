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

#include <QDialog>
#include "general.h"
#include "generalfactory.h"
#include "commandlinemanager.h"

#include "generalhandler.h"

GeneralHandler *GeneralHandler::m_instance = 0;

GeneralHandler::GeneralHandler(QObject *parent)
        : QObject(parent)
{
    m_instance = this;
    /*m_left = 0;
    m_right = 0;
    m_time = 0;
    m_state = General::Stopped;*/
    GeneralFactory* factory;
    //m_control = new Control(this);
    /*connect(m_control, SIGNAL(commandCalled(uint)), SLOT(processCommand(uint)));
    connect(m_control, SIGNAL(seekCalled(int)), SIGNAL(seekCalled(int)));
    connect(m_control, SIGNAL(volumeChanged(int, int)), SIGNAL(volumeChanged(int, int)));*/
    foreach(factory, *General::generalFactories())
    {
        if (General::isEnabled(factory))
        {
            General *general = factory->create(parent);
            connect (general, SIGNAL(toggleVisibilityCalled()), SIGNAL(toggleVisibilityCalled()));
            connect (general, SIGNAL(exitCalled()), SIGNAL(exitCalled()));
            m_generals.insert(factory, general);
        }
    }
    m_commandLineManager = new CommandLineManager(this);
    m_generals.insert(0, m_commandLineManager);
}

GeneralHandler::~GeneralHandler()
{}

void GeneralHandler::setEnabled(GeneralFactory* factory, bool enable)
{
    if (enable == m_generals.keys().contains(factory))
        return;
    if (enable)
    {
        General *general = factory->create(parent());
        connect (general, SIGNAL(toggleVisibilityCalled()), SIGNAL(toggleVisibilityCalled()));
        connect (general, SIGNAL(exitCalled()), SIGNAL(exitCalled()));
        m_generals.insert(factory, general);
        //general->setVolume(m_left, m_right);
        /*if (m_state != General::Stopped)
        {
            general->setState(m_state);
            general->setSongInfo(m_songInfo);
            general->setTime(m_time);
        }*/
    }
    else
    {
        delete m_generals.value(factory);
        m_generals.remove(factory);
    }
    General::setEnabled(factory, enable);
}

void GeneralHandler::showSettings(GeneralFactory* factory, QWidget* parentWidget)
{
    QDialog *dialog = factory->createConfigDialog(parentWidget);
    if (!dialog)
        return;

    if (dialog->exec() == QDialog::Accepted && m_generals.keys().contains(factory))
    {
        delete m_generals.value(factory);
        General *general = factory->create(parent());
        connect (general, SIGNAL(toggleVisibilityCalled()), SIGNAL(toggleVisibilityCalled()));
        connect (general, SIGNAL(exitCalled()), SIGNAL(exitCalled()));
        m_generals[factory] = general;
        /*general->setVolume(m_left, m_right);
        if (m_state != General::Stopped)
        {
            general->setState(m_state);
            general->setSongInfo(m_songInfo);
        }*/
    }
    dialog->deleteLater();
}

bool GeneralHandler::visibilityControl()
{
    GeneralFactory* factory;
    foreach(factory, *General::generalFactories())
    {
        if (General::isEnabled(factory) && factory->properties().visibilityControl)
            return TRUE;
    }
    return FALSE;
}

void GeneralHandler::executeCommand(const QString &opt_str)
{
    if(CommandLineManager::hasOption(opt_str))
        m_commandLineManager->executeCommand(opt_str);
}

GeneralHandler* GeneralHandler::instance()
{
    return m_instance;
}

