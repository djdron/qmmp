/***************************************************************************
 *   Copyright (C) 2008-2010 by Ilya Kotov                                 *
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
#include <QMenu>
#include <QWidget>
#include <QAction>

#include "general.h"
#include "generalfactory.h"
#include "commandlinemanager.h"

#include "generalhandler.h"

GeneralHandler *GeneralHandler::m_instance = 0;

GeneralHandler::GeneralHandler(QObject *parent)
        : QObject(parent)
{
    m_instance = this;
    m_toolsMenu = 0;
    m_playlistMenu = 0;
    GeneralFactory* factory;
    foreach(factory, *General::factories())
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
    }
    dialog->deleteLater();
}

bool GeneralHandler::visibilityControl()
{
    GeneralFactory* factory;
    foreach(factory, *General::factories())
    {
        if (General::isEnabled(factory) && factory->properties().visibilityControl)
            return true;
    }
    return FALSE;
}

void GeneralHandler::executeCommand(const QString &opt_str, const QStringList &args)
{
    if (CommandLineManager::hasOption(opt_str))
        m_commandLineManager->executeCommand(opt_str, args);
}

void GeneralHandler::addAction(QAction *action, MenuType type)
{
    connect(action, SIGNAL(destroyed (QObject *)), SLOT(removeAction(QObject*)));
    switch ((int) type)
    {
    case TOOLS_MENU:
        if (!m_toolsActions.contains(action))
            m_toolsActions.append(action);
        if (m_toolsMenu && !m_toolsMenu->actions ().contains(action))
            m_toolsMenu->addAction(action);
        break;
    case PLAYLIST_MENU:
        if (!m_playlistActions.contains(action))
            m_playlistActions.append(action);
        if (m_playlistMenu && !m_playlistMenu->actions ().contains(action))
            m_playlistMenu->addAction(action);
    }
}

void GeneralHandler::removeAction(QAction *action)
{
    m_toolsActions.removeAll(action);
    if (m_toolsMenu)
        m_toolsMenu->removeAction(action);
    m_playlistActions.removeAll(action);
    if (m_playlistMenu)
        m_playlistMenu->removeAction(action);
}

QList<QAction *> GeneralHandler::actions(MenuType type)
{
    if (type == TOOLS_MENU)
        return m_toolsActions;
    else
        return m_playlistActions;
}

QMenu *GeneralHandler::createMenu(MenuType type, const QString &title, QWidget *parent)
{
    switch ((int) type)
    {
    case TOOLS_MENU:
        if (!m_toolsMenu)
        {
            m_toolsMenu = new QMenu(title, parent);
            m_toolsMenu->addActions(m_toolsActions);
        }
        else
            m_toolsMenu->setTitle(title);
        return m_toolsMenu;
    case PLAYLIST_MENU:
        if (!m_playlistMenu)
        {
            m_playlistMenu = new QMenu(title, parent);
            m_playlistMenu->addActions(m_playlistActions);
        }
        else
            m_playlistMenu->setTitle(title);
        return m_playlistMenu;
    }
    return 0;
}

GeneralHandler* GeneralHandler::instance()
{
    return m_instance;
}

void GeneralHandler::removeAction(QObject *action)
{
    removeAction((QAction *) action);
}
