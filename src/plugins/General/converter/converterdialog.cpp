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
#include <QFile>
#include <QDir>
#include <qmmpui/playlistitem.h>
#include <qmmpui/metadataformatter.h>
#include <qmmpui/filedialog.h>
#include "preseteditor.h"
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

    readPresets(":/converter/presets.conf");
    readPresets(QDir::homePath() + "/.qmmp/converter/presets.conf");
}

ConverterDialog::~ConverterDialog()
{
    savePresets();
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

    /*QSettings preset_settings(QDir::homePath() + "/.qmmp/converterrc", QSettings::IniFormat);
    preset_settings.clear();
    for(int i = 0; i < ui.presetComboBox->count(); ++i)
    {
        QString name = ui.presetComboBox->itemText(i);
        QVariantMap data = ui.presetComboBox->itemData(i).toMap();
        if(data["read_only"].toBool())
            continue;
        preset_settings.beginGroup(name);
        preset_settings.setValue("ext", data["ext"].toString());
        preset_settings.setValue("command", data["command"].toString());
        preset_settings.setValue("use_16bit", data["use_16bit"].toBool());
        preset_settings.setValue("tags", data["tags"].toBool());
        preset_settings.endGroup();
    }*/
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
    fileNameMenu->addAction(tr("Year"))->setData("%y");
    fileNameMenu->addAction(tr("Condition"))->setData("%if(%p&%t,%p - %t,%f)");
    ui.fileNameButton->setMenu(fileNameMenu);
    ui.fileNameButton->setPopupMode(QToolButton::InstantPopup);
    connect(fileNameMenu, SIGNAL(triggered(QAction *)), SLOT(addTitleString(QAction *)));

    QMenu *presetMenu = new QMenu(this);
    presetMenu->addAction(tr("Create"), this, SLOT(createPreset()));
    presetMenu->addAction(tr("Edit"), this, SLOT(editPreset()));
    presetMenu->addAction(tr("Create a copy"), this, SLOT(copyPreset()));
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
    PresetEditor *editor = new PresetEditor(QVariantMap(), this);
    if(editor->exec() == QDialog::Accepted)
    {
        QVariantMap data = editor->data();
        data["name"] = uniqueName(data["name"].toString());
        if(data["name"].isValid() && data["ext"].isValid() && data["command"].isValid())
            ui.presetComboBox->addItem (data["name"].toString(), data);
    }
    editor->deleteLater();
}

void ConverterDialog::editPreset()
{
    if(ui.presetComboBox->currentIndex() == -1)
        return;
    int index = ui.presetComboBox->currentIndex();
    if(ui.presetComboBox->itemData(index).toMap()["read_only"].toBool())
        return;
    PresetEditor *editor = new PresetEditor(ui.presetComboBox->itemData(index).toMap(), this);
    if(editor->exec() == QDialog::Accepted)
    {
        QVariantMap data = editor->data();
        if(ui.presetComboBox->currentText() != data["name"].toString())
            data["name"] = uniqueName(data["name"].toString());
        if(data["name"].isValid() && data["ext"].isValid() && data["command"].isValid())
        {
            ui.presetComboBox->setItemText(index, data["name"].toString());
            ui.presetComboBox->setItemData(index, data);
        }
    }
    editor->deleteLater();
}

void ConverterDialog::copyPreset()
{
    if(ui.presetComboBox->currentIndex() == -1)
        return;
    int index = ui.presetComboBox->currentIndex();
    QVariantMap data = ui.presetComboBox->itemData(index).toMap();
    data["name"] = uniqueName(data["name"].toString());
    ui.presetComboBox->addItem (data["name"].toString(), data);
}

void ConverterDialog::deletePreset()
{
    if(ui.presetComboBox->currentIndex() == -1)
        return;
    if(ui.presetComboBox->itemData(ui.presetComboBox->currentIndex()).toMap()["read_only"].toBool())
        return;
    ui.presetComboBox->removeItem(ui.presetComboBox->currentIndex());
}

void ConverterDialog::readPresets(const QString &path)
{
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly))
        return;

    QList <QVariantMap> dataList;
    while(!file.atEnd())
    {
        QString line = QString::fromUtf8(file.readLine().trimmed());
        if(!line.contains("="))
            continue;
        QString key = line.split("=").at(0);
        QString value = line.split("=").at(1);
        if(key == "name")
            dataList.append(QVariantMap());
        if(dataList.isEmpty())
            continue;
        if(key == "use_16bit" && key == "tags") //boolean keys
            dataList.last()[key] = (value == "true");
        else
            dataList.last()[key] = value;
    }

    foreach(QVariantMap data, dataList)
    {
        data["read_only"] = path.startsWith(":/");
        QString title = data["name"].toString();
        if(data["read_only"].toBool())
            title += " *" ;
        ui.presetComboBox->addItem (title, data);
    }
}

void ConverterDialog::savePresets()
{
    QDir dir(QDir::homePath() + "/.qmmp/");
    dir.mkdir("converter");

    QFile file(QDir::homePath() + "/.qmmp/converter/presets.conf");
    if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        qWarning("ConverterDialog: unable to save presets; error %s", qPrintable(file.errorString()));
        return;
    }

    for(int i = 0; i < ui.presetComboBox->count(); ++i)
    {
        QVariantMap data = ui.presetComboBox->itemData(i).toMap();
        if(data["read_only"].toBool())
            continue;
        file.write(QString("%1=%2\n").arg("name").arg(data["name"].toString()).toUtf8());
        file.write(QString("%1=%2\n").arg("ext").arg(data["ext"].toString()).toUtf8());
        file.write(QString("%1=%2\n").arg("command").arg(data["command"].toString()).toUtf8());
        file.write(QString("%1=%2\n").arg("use_16bit")
                   .arg(data["use_16bit"].toBool() ? "true" : "false" ).toUtf8());
        file.write(QString("%1=%2\n").arg("tags")
                   .arg(data["tags"].toBool() ? "true" : "false" ).toUtf8());
        file.write("\n");
    }
}

QString ConverterDialog::uniqueName(const QString &name)
{
    QString unique_name = name;
    int i = 0;
    forever
    {
        if(ui.presetComboBox->findText(unique_name) == -1)
            break;
        unique_name = name + QString("_%1").arg(++i);
    }
    return unique_name;
}
