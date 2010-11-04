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
#include <qmmp/qmmp.h>
#include "actionmanager.h"
#include "viewmenu.h"

ViewMenu::ViewMenu(MainWindow *parent) : QMenu(parent)
{
    setTitle(tr("View"));
    m_alwaysOnTopAction = ActionManager::instance()->action(ActionManager::WM_ALLWAYS_ON_TOP);
    m_showOnAllDesktopsAction = ActionManager::instance()->action(ActionManager::WM_STICKY);
    m_doubleSizeAction = ActionManager::instance()->action(ActionManager::WM_DOUBLE_SIZE);
    addAction(m_alwaysOnTopAction);
    addAction(m_showOnAllDesktopsAction);
    addAction(m_doubleSizeAction);
    QSettings settings (Qmmp::configFile(), QSettings::IniFormat);
    m_doubleSizeAction->setChecked(settings.value ("General/double_size", false).toBool());
    m_alwaysOnTopAction->setChecked(settings.value ("General/always_on_top",false).toBool());
    m_showOnAllDesktopsAction->setChecked(settings.value ("General/show_on_all_desktops", false).toBool());
    connect(this, SIGNAL(triggered(QAction*)), SLOT(updateSettings()));
}

void ViewMenu::updateSettings()
{
    QSettings settings (Qmmp::configFile(), QSettings::IniFormat);
    settings.setValue ("General/double_size",  m_doubleSizeAction->isChecked());
    settings.setValue ("General/always_on_top",  m_alwaysOnTopAction->isChecked());
    settings.setValue ("General/show_on_all_desktops", m_showOnAllDesktopsAction->isChecked());
    qobject_cast <MainWindow *>(parent())->updateSettings();
}
