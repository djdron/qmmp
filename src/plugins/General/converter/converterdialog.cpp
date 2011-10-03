/***************************************************************************
 *   Copyright (C) 2011 by Ilya Kotov                                      *
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
#include <QDesktopServices>
#include <QMenu>
#include <qmmpui/playlistitem.h>
#include <qmmpui/metadataformatter.h>
#include <qmmpui/filedialog.h>
#include "converterdialog.h"

ConverterDialog::ConverterDialog(QList <PlayListItem *> items,  QWidget *parent) : QDialog(parent)
{
    ui.setupUi(this);
    MetaDataFormatter formatter("%p%if(%p&%t, - ,)%t - %l");
    foreach(PlayListItem *item , items)
    {
        if(item->length() == 0)
            continue;
        QString text = formatter.parse(item);
        QListWidgetItem *listItem = new QListWidgetItem(text);
        listItem->setData(Qt::UserRole, item->url());
        listItem->setCheckState(Qt::Checked);
        ui.itemsListWidget->addItem(listItem);
    }
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("Converter");
    QString music_path = QDesktopServices::storageLocation(QDesktopServices::MusicLocation);
    ui.outDirEdit->setText(settings.value("out_dir", music_path).toString());
    ui.outFileEdit->setText(settings.value("file_name","%p - %t").toString());
    settings.endGroup();
    createMenus();
}

QStringList ConverterDialog::selectedUrls() const
{
    QStringList out;
    for(int i = 0; i < ui.itemsListWidget->count(); i++)
    {
        if(ui.itemsListWidget->item(i)->checkState() == Qt::Checked)
            out << ui.itemsListWidget->item(i)->data(Qt::UserRole).toString();
    }
    return out;
}

void ConverterDialog::on_dirButton_clicked()
{
    QString dir = FileDialog::getExistingDirectory(this, tr("Choose a directory"),
                                        ui.outDirEdit->text());
    if(!dir.isEmpty())
        ui.outDirEdit->setText(dir);
}

void ConverterDialog::accept()
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("Converter");
    settings.setValue("out_dir", ui.outDirEdit->text());
    settings.value("file_name", ui.outFileEdit->text());
    settings.endGroup();
    QDialog::accept();
}

void ConverterDialog::createMenus()
{
    QMenu *fileNameMenu = new QMenu(this);
    fileNameMenu->addAction(tr("Artist"))->setData("%p");
    fileNameMenu->addAction(tr("Album"))->setData("%a");
    fileNameMenu->addAction(tr("Title"))->setData("%t");
    fileNameMenu->addAction(tr("Track number"))->setData("%n");
    fileNameMenu->addAction(tr("Two-digit track number"))->setData("%NN");
    fileNameMenu->addAction(tr("Genre"))->setData("%g");
    fileNameMenu->addAction(tr("Comment"))->setData("%c");
    fileNameMenu->addAction(tr("Composer"))->setData("%C");
    fileNameMenu->addAction(tr("Duration"))->setData("%l");
    fileNameMenu->addAction(tr("Disc number"))->setData("%D");
    fileNameMenu->addAction(tr("File name"))->setData("%f");
    fileNameMenu->addAction(tr("File path"))->setData("%F");
    fileNameMenu->addAction(tr("Year"))->setData("%y");
    fileNameMenu->addAction(tr("Condition"))->setData("%if(%p&%t,%p - %t,%f)");
    ui.fileNameButton->setMenu(fileNameMenu);
    ui.fileNameButton->setPopupMode(QToolButton::InstantPopup);
    connect(fileNameMenu, SIGNAL(triggered(QAction *)), SLOT(addTitleString(QAction *)));

    QMenu *presetMenu = new QMenu(this);
    presetMenu->addAction(tr("Create"), this, SLOT(copyPreset()));
    presetMenu->addAction(tr("Edit"), this, SLOT(editPreset()));
    presetMenu->addAction(tr("Copy"), this, SLOT(copyPreset()));
    presetMenu->addAction(tr("Delete"), this, SLOT(deletePreset()));
    ui.presetButton->setMenu(presetMenu);
    ui.presetButton->setPopupMode(QToolButton::InstantPopup);
}

void ConverterDialog::addTitleString(QAction *a)
{
    if (ui.outFileEdit->cursorPosition () < 1)
        ui.outFileEdit->insert(a->data().toString());
    else
        ui.outFileEdit->insert(" - "+a->data().toString());
}

void ConverterDialog::createPreset()
{

}

void ConverterDialog::editPreset()
{

}

void ConverterDialog::copyPreset()
{

}

void ConverterDialog::deletePreset()
{

}

void ConverterDialog::readSettings()
{

}
