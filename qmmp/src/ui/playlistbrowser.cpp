/***************************************************************************
 *   Copyright (C) 2009 by Ilya Kotov                                      *
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

#include <QAction>
#include <QApplication>
#include <QStyle>
#include <qmmpui/playlistmanager.h>
#include "playlistbrowser.h"

PlayListBrowser::PlayListBrowser(PlayListManager *manager, QWidget *parent) : QDialog(parent)
{
    setAttribute(Qt::WA_DeleteOnClose, true);
    setAttribute(Qt::WA_QuitOnClose, false);
    setWindowModality(Qt::NonModal);
    ui.setupUi(this);
    m_pl_manager = manager;
    connect(m_pl_manager, SIGNAL(playListsChanged()), SLOT(updateList()));
    connect(ui.newButton, SIGNAL(clicked()), m_pl_manager, SLOT(createPlayList()));
    updateList();
    //actions
    QAction *renameAct = new QAction(tr("Rename"), this);
    QAction *removeAct = new QAction(tr("Delete"), this);
    connect(renameAct,SIGNAL(triggered()), SLOT(rename()));
    connect(removeAct,SIGNAL(triggered()), SLOT(on_deleteButton_clicked()));
    ui.listWidget->setContextMenuPolicy(Qt::ActionsContextMenu);
    ui.listWidget->addAction(renameAct);
    ui.listWidget->addAction(removeAct);
    ui.downButton->setIcon(QApplication::style()->standardIcon(QStyle::SP_ArrowDown));
    ui.upButton->setIcon(QApplication::style()->standardIcon(QStyle::SP_ArrowUp));
}

PlayListBrowser::~PlayListBrowser()
{}

void PlayListBrowser::updateList()
{
    ui.listWidget->clear();
    foreach(PlayListModel *model, m_pl_manager->playLists())
        ui.listWidget->addItem(model->name());
    int c = m_pl_manager->indexOf(m_pl_manager->selectedPlayList());
    ui.listWidget->setCurrentRow (c);
    QFont font = ui.listWidget->currentItem()->font();
    font.setBold(true);
    ui.listWidget->currentItem()->setFont(font);
}

void PlayListBrowser::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    m_pl_manager->selectPlayList(ui.listWidget->row(item));
}

void PlayListBrowser::on_listWidget_itemChanged(QListWidgetItem *item)
{
    m_pl_manager->playListAt(ui.listWidget->row(item))->setName(item->text());
}

void PlayListBrowser::rename()
{
    QListWidgetItem *item = ui.listWidget->currentItem();
    if(item)
    {
        item->setFlags(Qt::ItemIsEditable | item->flags());
        ui.listWidget->editItem(item);
    }
}

void PlayListBrowser::on_deleteButton_clicked()
{
    QList <PlayListModel *> models;
    foreach(QListWidgetItem *item, ui.listWidget->selectedItems())
        models.append(m_pl_manager->playListAt(ui.listWidget->row (item)));
    foreach(PlayListModel *model, models)
        m_pl_manager->removePlayList(model);
}

void PlayListBrowser::on_downButton_clicked()
{
    int pos = m_pl_manager->indexOf(m_pl_manager->selectedPlayList());
    if(pos < m_pl_manager->count() - 1)
        m_pl_manager->move(pos, pos + 1);
}

void PlayListBrowser::on_upButton_clicked()
{
    int pos = m_pl_manager->indexOf(m_pl_manager->selectedPlayList());
    if(pos > 0)
        m_pl_manager->move(pos, pos - 1);
}
