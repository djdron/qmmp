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
    ui.fileNameButton->setMenu(menu);
    ui.fileNameButton->setPopupMode(QToolButton::InstantPopup);
    connect(menu, SIGNAL(triggered(QAction *)), SLOT(addTitleString(QAction *)));
}

void ConverterDialog::addTitleString(QAction *a)
{
    if (ui.outFileEdit->cursorPosition () < 1)
        ui.outFileEdit->insert(a->data().toString());
    else
        ui.outFileEdit->insert(" - "+a->data().toString());
}
