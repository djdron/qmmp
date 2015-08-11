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
    connect(m_ui->actionsListWidget->model(), SIGNAL(rowsAboutToBeRemoved(const QModelIndex &,int,int)),
            SLOT(onRowsAboutToBeRemoved(const QModelIndex &, int, int)));
    connect(m_ui->activeActionsListWidget->model(), SIGNAL(rowsAboutToBeRemoved(const QModelIndex &,int,int)),
            SLOT(onRowsAboutToBeRemoved(const QModelIndex &, int, int)));
    populateActionList();
}

ToolBarEditor::~ToolBarEditor()
{
    delete m_ui;
}

void ToolBarEditor::accept()
{
    QStringList names;
    for(int row = 0; row < m_ui->activeActionsListWidget->count(); ++row)
        names.append(m_ui->activeActionsListWidget->item(row)->data(Qt::UserRole).toString());

    QSettings settings (Qmmp::configFile(), QSettings::IniFormat);
    settings.setValue("Simple/toolbar_actions", names);
    QDialog::accept();
}

void ToolBarEditor::populateActionList(bool reset)
{
    QStringList names = ActionManager::instance()->toolBarActionNames();
    if(!reset)
    {
        QSettings settings (Qmmp::configFile(), QSettings::IniFormat);
        names = settings.value("Simple/toolbar_actions", names).toStringList();
    }

    for(int id = ActionManager::PLAY; id <= ActionManager::QUIT; ++id)
    {
        QAction *action = ACTION(id);
        if(action->icon().isNull())
            continue;
        QListWidgetItem *item = new QListWidgetItem();
        item->setIcon(action->icon());
        item->setText(action->text().replace("&", ""));
        item->setData(Qt::UserRole, action->objectName());
        if(!names.contains(action->objectName()))
            m_ui->actionsListWidget->addItem(item);
    }

    {
        QListWidgetItem *item = new QListWidgetItem();
        item->setText("-- " + tr("Separator") + " --");
        item->setData(Qt::UserRole, "separator");
        m_ui->actionsListWidget->addItem(item);
    }

    foreach (QString name, names)
    {
        QAction *action = ActionManager::instance()->findChild<QAction *>(name);
        if(action)
        {
            QListWidgetItem *item = new QListWidgetItem();
            item->setIcon(action->icon());
            item->setText(action->text().replace("&", ""));
            item->setData(Qt::UserRole, action->objectName());
            m_ui->activeActionsListWidget->addItem(item);
        }
        else if(name == "separator")
        {
            QListWidgetItem *item = new QListWidgetItem();
            item->setText("-- " + tr("Separator") + " --");
            item->setData(Qt::UserRole, "separator");
            m_ui->activeActionsListWidget->addItem(item);
        }
    }
}

void ToolBarEditor::on_addToolButton_clicked()
{
    int row = m_ui->actionsListWidget->currentRow();
    if(row > -1)
    {
        QListWidgetItem *item = m_ui->actionsListWidget->takeItem(row);
        m_ui->activeActionsListWidget->addItem(item);
    }
}

void ToolBarEditor::on_removeToolButton_clicked()
{
    int row = m_ui->activeActionsListWidget->currentRow();
    if(row > -1)
    {
        QListWidgetItem *item = m_ui->activeActionsListWidget->takeItem(row);
        m_ui->actionsListWidget->addItem(item);
    }
}

void ToolBarEditor::on_upToolButton_clicked()
{
    int row = m_ui->activeActionsListWidget->currentRow();
    if(row > 0)
    {
        QListWidgetItem *item = m_ui->activeActionsListWidget->takeItem(row);
        m_ui->activeActionsListWidget->insertItem(row - 1, item);
        m_ui->activeActionsListWidget->setCurrentItem(item);
    }
}

void ToolBarEditor::on_downToolButton_clicked()
{
    int row = m_ui->activeActionsListWidget->currentRow();
    if(row > -1 && row < m_ui->activeActionsListWidget->count())
    {
        QListWidgetItem *item = m_ui->activeActionsListWidget->takeItem(row);
        m_ui->activeActionsListWidget->insertItem(row + 1, item);
        m_ui->activeActionsListWidget->setCurrentItem(item);
    }
}

void ToolBarEditor::on_resetPushButton_clicked()
{
    m_ui->actionsListWidget->clear();
    m_ui->activeActionsListWidget->clear();
    populateActionList(true);
}

void ToolBarEditor::onRowsAboutToBeRemoved(const QModelIndex &, int start, int)
{
    if(sender() == m_ui->actionsListWidget->model())
    {
        //recreate separator
        QListWidgetItem *item = m_ui->actionsListWidget->item(start);
        if(item && item->data(Qt::UserRole).toString() == "separator")
            m_ui->actionsListWidget->addItem(item->clone());
    }
    else if(sender() == m_ui->activeActionsListWidget->model())
    {
        //remove separator
        QListWidgetItem *item = m_ui->activeActionsListWidget->item(start);
        if(item && item->data(Qt::UserRole).toString() == "separator")
        {
            for(int i = 0; i < m_ui->actionsListWidget->count(); ++i)
            {
                if(m_ui->actionsListWidget->item(i)->data(Qt::UserRole).toString() == "separator")
                {
                    m_ui->actionsListWidget->model()->blockSignals(true);
                    delete m_ui->actionsListWidget->takeItem(i);
                    m_ui->actionsListWidget->model()->blockSignals(false);
                    break;
                }
            }
        }
    }
}
