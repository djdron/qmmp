/***************************************************************************
 *   Copyright (C) 2013 by Ilya Kotov                                      *
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
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#include <QApplication>
#include <QSettings>
#include <qmmp/qmmp.h>
#include "toolbareditor.h"
#include "ui_toolbareditor.h"

ToolBarEditor::ToolBarEditor(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::ToolBarEditor)
{
    m_ui->setupUi(this);
    m_ui->upToolButton->setIcon(qApp->style()->standardIcon(QStyle::SP_ArrowUp));
    m_ui->downToolButton->setIcon(qApp->style()->standardIcon(QStyle::SP_ArrowDown));
    m_ui->addToolButton->setIcon(qApp->style()->standardIcon(QStyle::SP_ArrowRight));
    m_ui->removeToolButton->setIcon(qApp->style()->standardIcon(QStyle::SP_ArrowLeft));

    m_defaultIdList << ActionManager::PREVIOUS
                        << ActionManager::PLAY
                        << ActionManager::PAUSE
                        << ActionManager::STOP
                        << ActionManager::NEXT
                        << ActionManager::EJECT;

    populateActionList();
    populateActiveActionList();
}

ToolBarEditor::~ToolBarEditor()
{
    delete m_ui;
}

void ToolBarEditor::populateActionList()
{
    for(int id = ActionManager::PLAY; id <= ActionManager::QUIT; ++id)
    {
        QAction *action = ACTION(id);
        if(action->icon().isNull())
            continue;
        QListWidgetItem *item = new QListWidgetItem();
        item->setIcon(action->icon());
        item->setText(action->text().replace("&", ""));
        item->setData(Qt::UserRole, action->objectName());
        m_ui->actionsListWidget->addItem(item);
    }
}

void ToolBarEditor::populateActiveActionList()
{
    ActionManager *manager = ActionManager::instance();
    QStringList defaultActionNames;
    foreach (ActionManager::Type id, m_defaultIdList)
    {
        defaultActionNames << manager->action(id)->objectName();
    }

    QSettings settings (Qmmp::configFile(), QSettings::IniFormat);
    QStringList names = settings.value("Simple/toolbar_actions", defaultActionNames).toStringList();
    foreach (QString name, names)
    {
        QAction *action = manager->findChild<QAction *>(name);
        if(!action)
            continue;
        QListWidgetItem *item = new QListWidgetItem();
        item->setIcon(action->icon());
        item->setText(action->text().replace("&", ""));
        item->setData(Qt::UserRole, action->objectName());
        m_ui->activeActionsListWidget->addItem(item);
    }
}
