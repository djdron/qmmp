/***************************************************************************
 *   Copyright (C) 2007-2011 by Ilya Kotov                                 *
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

#ifndef JUMPTOTRACKDIALOG_P_H
#define JUMPTOTRACKDIALOG_P_H

#include <QDialog>
#include "ui_jumptotrackdialog.h"

class QStringListModel;
class QSortFilterProxyModel;
class PlayListManager;
class PlayListModel;

/**
   @author Vladimir Kuznetsov <vovanec@gmail.com>
 */
class JumpToTrackDialog : public QDialog, private Ui::JumpToTrackDialog
{
    Q_OBJECT

public:
    JumpToTrackDialog(PlayListModel *model, QWidget* parent = 0);
    ~JumpToTrackDialog();
    void refresh();

private slots:
    void on_refreshPushButton_clicked();
    void on_queuePushButton_clicked();
    void on_jumpToPushButton_clicked();
    void on_filterLineEdit_textChanged(const QString&);
    void on_filterLineEdit_returnPressed ();
    void jumpTo(const QModelIndex&);
    void queueUnqueue(const QModelIndex&,const QModelIndex&);

private:
    QStringListModel* m_listModel;
    QSortFilterProxyModel* m_proxyModel;
    PlayListManager *m_pl_manager;
    PlayListModel *m_model;
};

#endif //JUMPTOTRACKDIALOG_P_H

