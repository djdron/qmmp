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

#include "jumptotrackdialog.h"
#include <qmmpui/playlistmanager.h>

#include <QStringListModel>
#include <QSortFilterProxyModel>
#include <QShortcut>
#include <QKeySequence>

JumpToTrackDialog::JumpToTrackDialog(PlayListManager *manager, QWidget* parent)
        : QDialog (parent)
{
    setupUi(this);
    setAttribute(Qt::WA_QuitOnClose, FALSE);
    m_pl_manager = manager;
    m_listModel = new QStringListModel(this);

    m_proxyModel = new QSortFilterProxyModel;
    m_proxyModel->setDynamicSortFilter(true);
    m_proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    m_proxyModel->setSourceModel(m_listModel);
    songsListView->setModel(m_proxyModel);

    connect(songsListView,SIGNAL(doubleClicked(const QModelIndex &)),
            this,SLOT(jumpTo(const QModelIndex&)));
    connect(songsListView,SIGNAL(activated(const QModelIndex &)),
            this,SLOT(jumpTo(const QModelIndex&)));
    connect(songsListView,SIGNAL(activated(const QModelIndex &)),
            this,SLOT(accept()));
    connect(songsListView->selectionModel(),
            SIGNAL(currentRowChanged(const QModelIndex&,const QModelIndex&)),
            this,SLOT(queueUnqueue(const QModelIndex&,const QModelIndex&)));

    new QShortcut(QKeySequence("Q"),this,SLOT(on_queuePushButton_clicked()));
    new QShortcut(QKeySequence("J"),this,SLOT(on_jumpToPushButton_clicked()));
    new QShortcut(QKeySequence("F5"),this,SLOT(on_refreshPushButton_clicked()));
}

JumpToTrackDialog::~JumpToTrackDialog()
{
}


void JumpToTrackDialog::on_closePushButton_clicked()
{
    hide();
}

void JumpToTrackDialog::on_refreshPushButton_clicked()
{
    refresh();
}

void JumpToTrackDialog::on_queuePushButton_clicked()
{
    QModelIndexList mi_list = songsListView->selectionModel()->selectedRows();
    if (!mi_list.isEmpty())
    {
        int selected = (m_proxyModel->mapToSource(mi_list.at(0))).row();
        m_pl_manager->selectedPlayList()->setQueued(m_pl_manager->selectedPlayList()->item(selected));
        if (m_pl_manager->selectedPlayList()->isQueued(m_pl_manager->selectedPlayList()->item(selected)))
            queuePushButton->setText(tr("Unqueue"));
        else
            queuePushButton->setText(tr("Queue"));
    }
}

void JumpToTrackDialog::on_jumpToPushButton_clicked()
{
    QModelIndexList mi_list = songsListView->selectionModel()->selectedRows();
    if (!mi_list.isEmpty())
    {
        jumpTo(mi_list.at(0));
    }
}

void JumpToTrackDialog::refresh()
{
    filterLineEdit->clear();
    QStringList titles = m_pl_manager->selectedPlayList()->getTitles(0, m_pl_manager->selectedPlayList()->count());
    m_listModel->setStringList(titles);
    filterLineEdit->setFocus();
}

void JumpToTrackDialog::on_filterLineEdit_textChanged(const QString &str)
{
    m_proxyModel->setFilterFixedString(str);
    if (m_proxyModel->hasIndex(0,0))
        songsListView->setCurrentIndex (m_proxyModel->index (0,0));
}

void JumpToTrackDialog::on_filterLineEdit_returnPressed ()
{
    QModelIndexList mi_list = songsListView->selectionModel()->selectedRows();
    if (!mi_list.isEmpty())
    {
        jumpTo(mi_list.at(0));
        accept();
    }
}

void JumpToTrackDialog::jumpTo(const QModelIndex & index)
{
    int selected = (m_proxyModel->mapToSource(index)).row();
    m_pl_manager->selectedPlayList()->setCurrent(selected);
    emit playRequest();
}

void JumpToTrackDialog::queueUnqueue(const QModelIndex& curr,const QModelIndex&)
{
    int row = m_proxyModel->mapToSource(curr).row();
    if (m_pl_manager->selectedPlayList()->isQueued(m_pl_manager->selectedPlayList()->item(row)))
        queuePushButton->setText(tr("Unqueue"));
    else
        queuePushButton->setText(tr("Queue"));
}


