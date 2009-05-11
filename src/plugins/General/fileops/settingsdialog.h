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
#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

#include "ui_settingsdialog.h"

/**
    @author Ilya Kotov <forkotov02@hotmail.ru>
*/
class ActionItem;

class SettingsDialog : public QDialog
{
    Q_OBJECT
public:
    SettingsDialog(QWidget *parent = 0);

    ~SettingsDialog();


public slots:
    virtual void accept();

private slots:
    void createAction();
    void deleteAction();
    void updateLineEdits();
    void changeDestination(const QString &dest);
    void changePattern(const QString &pattern);
    void addTitleString(QAction *action);
    void selectDirectory();
    void on_tableWidget_itemDoubleClicked (QTableWidgetItem *item);

private:
    void createMenus();
    Ui::SettingsDialog ui;
};

class ActionItem: public QTableWidgetItem
{
public:
    ActionItem (const QString &text): QTableWidgetItem(text){};

    QString pattern()
    {
        return m_pattern;
    };

    QString destination()
    {
        return m_destination;
    };

    void setPattern(const QString &pattern)
    {
        m_pattern = pattern;
    };

    void setDestination(const QString &dest)
    {
        m_destination = dest;
    };

private:
    QString m_pattern, m_destination;
};


#endif
