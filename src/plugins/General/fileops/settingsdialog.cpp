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
#include <QTextCodec>
#include <QSettings>
#include <QCheckBox>
#include <QComboBox>

#include <qmmp/qmmp.h>
#include "fileops.h"
#include "settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent)
        : QDialog(parent)
{
    ui.setupUi(this);
    ui.tableWidget->verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    ui.tableWidget->verticalHeader()->hide();
    connect (ui.newButton,SIGNAL(pressed()), SLOT(createAction()));
    connect (ui.deleteButton,SIGNAL(pressed()), SLOT(deleteAction()));
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("FileOps");
    int count = settings.value("count", 0).toInt();
    for (int i = 0; i < count; ++i)
    {
        ui.tableWidget->insertRow (i);
        QCheckBox *checkBox = new QCheckBox;
        checkBox->setFocusPolicy (Qt::NoFocus);

        QComboBox *comboBox = new QComboBox;
        comboBox->addItem (tr("Copy"), FileOps::COPY);
        comboBox->addItem (tr("Rename"), FileOps::RENAME);
        comboBox->addItem (tr("Move"), FileOps::MOVE);
        comboBox->addItem (tr("Remove"), FileOps::REMOVE);
        comboBox->setFocusPolicy (Qt::NoFocus);

        checkBox->setChecked(settings.value(QString("enabled_%1").arg(i), TRUE).toBool());
        int ci = comboBox->findData (settings.value(QString("action_%1").arg(i), FileOps::COPY).toInt());
        if (ci >= 0)
            comboBox->setCurrentIndex(ci);

        ActionItem *item = new ActionItem(settings.value(QString("name_%1").arg(i), "Action").toString());
        item->setPattern(settings.value(QString("pattern_%1").arg(i)).toString());
        item->setDestination(settings.value(QString("destination_%1").arg(i)).toString());

        ui.tableWidget->setCellWidget (i, 0, checkBox);
        ui.tableWidget->setCellWidget (i, 1, comboBox);
        ui.tableWidget->setItem (i, 2, item);
    }
    settings.endGroup();
}


SettingsDialog::~SettingsDialog()
{}

void SettingsDialog::accept()
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    //settings.beginGroup("CUE");
    //settings.setValue("encoding", ui.cueEncComboBox->currentText());
    settings.endGroup();
    QDialog::accept();
}

void SettingsDialog::createAction()
{
    int row = ui.tableWidget->rowCount ();
    ui.tableWidget->insertRow (row);
    QCheckBox *checkBox = new QCheckBox;
    checkBox->setFocusPolicy (Qt::NoFocus);
    checkBox->setChecked(TRUE);

    QComboBox *comboBox = new QComboBox;
    comboBox->addItem (tr("Copy"), FileOps::COPY);
    comboBox->addItem (tr("Rename"), FileOps::RENAME);
    comboBox->addItem (tr("Move"), FileOps::MOVE);
    comboBox->addItem (tr("Remove"), FileOps::REMOVE);
    comboBox->setFocusPolicy (Qt::NoFocus);

    ActionItem *item = new ActionItem("New action");
    //item->setPattern(settings.value(QString("pattern_%1").arg(i)).toString());
    //item->setDestination(settings.value(QString("destination_%1").arg(i)).toString());

    ui.tableWidget->setCellWidget (row, 0, checkBox);
    ui.tableWidget->setCellWidget (row, 1, comboBox);
    ui.tableWidget->setItem (row, 2, item);
}

void SettingsDialog::deleteAction()
{
    if (ui.tableWidget->currentRow () >= 0)
        ui.tableWidget->removeRow (ui.tableWidget->currentRow ());
}
