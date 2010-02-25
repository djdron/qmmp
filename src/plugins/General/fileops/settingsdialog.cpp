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
#include <QMenu>
#include <QHeaderView>
#include <QApplication>
#include <qmmp/qmmp.h>
#include <qmmpui/filedialog.h>
#include "fileops.h"
#include "hotkeydialog.h"
#include "settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent)
        : QDialog(parent)
{
    ui.setupUi(this);
    ui.tableWidget->verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    ui.tableWidget->verticalHeader()->hide();
    ui.tableWidget->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
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
        //comboBox->addItem (tr("Move"), FileOps::MOVE);
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
        QTableWidgetItem *item2 = new QTableWidgetItem();
        item2->setText(settings.value(QString("hotkey_%1").arg(i)).toString());
        ui.tableWidget->setItem (i, 3, item2);
        ui.tableWidget->item (i, 3)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    }
    settings.endGroup();
    connect (ui.tableWidget, SIGNAL(currentItemChanged (QTableWidgetItem *, QTableWidgetItem *)),
             SLOT(updateLineEdits()));
    updateLineEdits();
    connect (ui.destinationEdit, SIGNAL(textChanged (const QString&)), SLOT(changeDestination(const QString&)));
    connect (ui.patternEdit, SIGNAL(textChanged (const QString&)), SLOT(changePattern(const QString&)));
    connect (ui.destButton, SIGNAL(clicked()), SLOT(selectDirectory()));
    createMenus();
}


SettingsDialog::~SettingsDialog()
{}

void SettingsDialog::accept()
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("FileOps");
    //remove unused keys
    int count = settings.value("count", 0).toInt();
    for (int i = ui.tableWidget->rowCount() - 1; i < count; ++i)
    {
        settings.remove (QString("enabled_%1").arg(i));
        settings.remove (QString("action_%1").arg(i));
        settings.remove (QString("name_%1").arg(i));
        settings.remove (QString("pattern_%1").arg(i));
        settings.remove (QString("destination_%1").arg(i));
    }
    //save actions
    settings.setValue("count", ui.tableWidget->rowCount());
    for (int i = 0; i < ui.tableWidget->rowCount(); ++i)
    {
        QCheckBox *checkBox = qobject_cast<QCheckBox *>(ui.tableWidget->cellWidget (i, 0));
        settings.setValue (QString("enabled_%1").arg(i), checkBox->isChecked());

        QComboBox *comboBox = qobject_cast<QComboBox *>(ui.tableWidget->cellWidget (i, 1));
        settings.setValue (QString("action_%1").arg(i), comboBox->itemData (comboBox->currentIndex()));

        ActionItem *item = (ActionItem *) ui.tableWidget->item(i,2);
        settings.setValue (QString("name_%1").arg(i), item->text());
        settings.setValue (QString("pattern_%1").arg(i), item->pattern());
        settings.setValue (QString("destination_%1").arg(i), item->destination());
        settings.setValue (QString("hotkey_%1").arg(i), ui.tableWidget->item(i,3)->text());
    }
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
    //comboBox->addItem (tr("Move"), FileOps::MOVE);
    comboBox->addItem (tr("Remove"), FileOps::REMOVE);
    comboBox->setFocusPolicy (Qt::NoFocus);

    ActionItem *item = new ActionItem(tr("New action"));
    //item->setPattern(settings.value(QString("pattern_%1").arg(i)).toString());
    //item->setDestination(settings.value(QString("destination_%1").arg(i)).toString());

    ui.tableWidget->setCellWidget (row, 0, checkBox);
    ui.tableWidget->setCellWidget (row, 1, comboBox);
    ui.tableWidget->setItem (row, 2, item);
    QTableWidgetItem *item2 = new QTableWidgetItem();
    ui.tableWidget->setItem (row, 3, item2);
    ui.tableWidget->item (row, 3)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
}

void SettingsDialog::deleteAction()
{
    if (ui.tableWidget->currentRow () >= 0)
        ui.tableWidget->removeRow (ui.tableWidget->currentRow ());
}

void SettingsDialog::updateLineEdits()
{
    if (ui.tableWidget->currentRow () >= 0)
    {
        ActionItem *item = (ActionItem *) ui.tableWidget->item(ui.tableWidget->currentRow (), 2);
        ui.destinationEdit->setText(item->destination());
        ui.patternEdit->setText(item->pattern());
    }
    else
    {
        ui.destinationEdit->clear();
        ui.patternEdit->clear();
    }
}

void SettingsDialog::changeDestination(const QString &dest)
{
    if (ui.tableWidget->currentRow () >= 0)
    {
        ActionItem *item = (ActionItem *) ui.tableWidget->item(ui.tableWidget->currentRow (), 2);
        item->setDestination(dest);
    }
}

void SettingsDialog::changePattern(const QString &pattern)
{
    if (ui.tableWidget->currentRow () >= 0)
    {
        ActionItem *item = (ActionItem *) ui.tableWidget->item(ui.tableWidget->currentRow (), 2);
        item->setPattern(pattern);
    }
}

void SettingsDialog::createMenus()
{
    QMenu *menu = new QMenu(this);
    menu->addAction(tr("Artist"))->setData("%p");
    menu->addAction(tr("Album"))->setData("%a");
    menu->addAction(tr("Title"))->setData("%t");
    menu->addAction(tr("Track number"))->setData("%n");
    menu->addAction(tr("Two-digit track number"))->setData("%NN");
    menu->addAction(tr("Genre"))->setData("%g");
    menu->addAction(tr("Comment"))->setData("%c");
    menu->addAction(tr("Composer"))->setData("%C");
    menu->addAction(tr("Duration"))->setData("%l");
    menu->addAction(tr("Disc number"))->setData("%D");
    menu->addAction(tr("File name"))->setData("%f");
    menu->addAction(tr("File path"))->setData("%F");
    menu->addAction(tr("Year"))->setData("%y");
    menu->addAction(tr("Condition"))->setData("%if(%p&%t,%p - %t,%f)");
    ui.patternButton->setMenu(menu);
    ui.patternButton->setPopupMode(QToolButton::InstantPopup);
    connect(menu, SIGNAL(triggered (QAction *)), SLOT(addTitleString( QAction *)));
}

void SettingsDialog::addTitleString(QAction *a)
{
    if (ui.patternEdit->cursorPosition () < 1)
        ui.patternEdit->insert(a->data().toString());
    else
        ui.patternEdit->insert(" - "+a->data().toString());
}

void SettingsDialog::selectDirectory()
{
    QString dir = FileDialog::getExistingDirectory(this, tr("Choose a directory"),
                                        ui.destinationEdit->text());
    if(!dir.isEmpty())
        ui.destinationEdit->setText(dir);
}

void SettingsDialog::on_tableWidget_itemDoubleClicked (QTableWidgetItem *item)
{
    HotkeyDialog *dialog = new HotkeyDialog(item->text(), this);
    if (ui.tableWidget->column (item) == 3 && dialog->exec() == QDialog::Accepted)
        item->setText(dialog->key());
    dialog->deleteLater();
}
