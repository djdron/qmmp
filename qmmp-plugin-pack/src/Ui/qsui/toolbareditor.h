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

#ifndef TOOLBAREDITOR_H
#define TOOLBAREDITOR_H

#include <QDialog>
#include <QModelIndex>
#include "actionmanager.h"

namespace Ui {
class ToolBarEditor;
}

/**
    @author Ilya Kotov <forkotov02@hotmail.ru>
*/
class ToolBarEditor : public QDialog
{
    Q_OBJECT

public:
    explicit ToolBarEditor(QWidget *parent = 0);
    ~ToolBarEditor();

public slots:
    void accept();

private slots:
    void on_addToolButton_clicked();
    void on_removeToolButton_clicked();
    void on_upToolButton_clicked();
    void on_downToolButton_clicked();
    void on_resetPushButton_clicked();
    void onRowsAboutToBeRemoved(const QModelIndex &, int start, int);

private:
    void populateActionList(bool reset = false);
    Ui::ToolBarEditor *m_ui;
};

#endif // TOOLBAREDITOR_H
