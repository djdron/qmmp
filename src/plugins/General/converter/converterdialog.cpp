/***************************************************************************
 *   Copyright (C) 2011-2015 by Ilya Kotov                                 *
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

#include <QSettings>
#include <QStandardPaths>
#include <QMenu>
#include <QFile>
#include <QDir>
#include <qmmpui/playlistitem.h>
#include <qmmpui/metadataformatter.h>
#include <qmmpui/filedialog.h>
#include "preseteditor.h"
#include "converterdialog.h"

ConverterDialog::ConverterDialog(QList <PlayListTrack *> items,  QWidget *parent) : QDialog(parent)
{
    m_ui.setupUi(this);
    MetaDataFormatter formatter("%p%if(%p&%t, - ,)%t - %l");
    foreach(PlayListTrack *item , items)
    {
        if(item->length() == 0)
            continue;
        QString text = formatter.format(item);
        QListWidgetItem *listItem = new QListWidgetItem(text);
        listItem->setData(Qt::UserRole, item->url());
        listItem->setCheckState(Qt::Checked);
        m_ui.itemsListWidget->addItem(listItem);
    }
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("Converter");
    QString music_path = QStandardPaths::writableLocation(QStandardPaths::MusicLocation);
    m_ui.outDirEdit->setText(settings.value("out_dir", music_path).toString());
    m_ui.outFileEdit->setText(settings.value("file_name","%p - %t").toString());
    m_ui.overwriteCheckBox->setChecked(settings.value("overwrite",false).toBool());
    settings.endGroup();
    createMenus();

    readPresets(":/converter/presets.conf");
    readPresets(Qmmp::configDir() + "converter/presets.conf");
}

ConverterDialog::~ConverterDialog()
{
    savePresets();
}

QStringList ConverterDialog::selectedUrls() const
{
    QStringList out;
    for(int i = 0; i < m_ui.itemsListWidget->count(); i++)
    {
        if(m_ui.itemsListWidget->item(i)->checkState() == Qt::Checked)
            out << m_ui.itemsListWidget->item(i)->data(Qt::UserRole).toString();
    }
    return out;
}

QVariantMap ConverterDialog::preset() const
{
    if(m_ui.presetComboBox->currentIndex() == -1)
        return QVariantMap();
    int index = m_ui.presetComboBox->currentIndex();
    //aditional parameters
    QVariantMap preset = m_ui.presetComboBox->itemData(index).toMap();
    preset["out_dir"] = m_ui.outDirEdit->text();
    preset["file_name"] = m_ui.outFileEdit->text();
    preset["overwrite"] = m_ui.overwriteCheckBox->isChecked();
    return preset;
}

void ConverterDialog::on_dirButton_clicked()
{
    QString dir = FileDialog::getExistingDirectory(this, tr("Choose a directory"),
                                        m_ui.outDirEdit->text());
    if(!dir.isEmpty())
        m_ui.outDirEdit->setText(dir);
}

void ConverterDialog::accept()
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("Converter");
    settings.setValue("out_dir", m_ui.outDirEdit->text());
    settings.value("file_name", m_ui.outFileEdit->text());
    settings.setValue("overwrite", m_ui.overwriteCheckBox->isChecked());
    settings.endGroup();
    QDialog::accept();
}

void ConverterDialog::createMenus()
{
    QMenu *fileNameMenu = new QMenu(this);
    fileNameMenu->addAction(tr("Artist"))->setData("%p");
    fileNameMenu->addAction(tr("Album"))->setData("%a");
    fileNameMenu->addAction(tr("Album Artist"))->setData("%aa");
    fileNameMenu->addAction(tr("Title"))->setData("%t");
    fileNameMenu->addAction(tr("Track Number"))->setData("%n");
    fileNameMenu->addAction(tr("Two-digit Track Number"))->setData("%NN");
    fileNameMenu->addAction(tr("Genre"))->setData("%g");
    fileNameMenu->addAction(tr("Comment"))->setData("%c");
    fileNameMenu->addAction(tr("Composer"))->setData("%C");
    fileNameMenu->addAction(tr("Duration"))->setData("%l");
    fileNameMenu->addAction(tr("Disc Number"))->setData("%D");
    fileNameMenu->addAction(tr("File Name"))->setData("%f");
    fileNameMenu->addAction(tr("File Path"))->setData("%F");
    fileNameMenu->addAction(tr("Year"))->setData("%y");
    fileNameMenu->addAction(tr("Condition"))->setData("%if(%p&%t,%p - %t,%f)");
    m_ui.fileNameButton->setMenu(fileNameMenu);
    m_ui.fileNameButton->setPopupMode(QToolButton::InstantPopup);
    connect(fileNameMenu, SIGNAL(triggered(QAction *)), SLOT(addTitleString(QAction *)));

    QMenu *presetMenu = new QMenu(this);
    presetMenu->addAction(tr("Create"), this, SLOT(createPreset()));
    presetMenu->addAction(tr("Edit"), this, SLOT(editPreset()));
    presetMenu->addAction(tr("Create a Copy"), this, SLOT(copyPreset()));
    presetMenu->addAction(tr("Delete"), this, SLOT(deletePreset()));
    m_ui.presetButton->setMenu(presetMenu);
    m_ui.presetButton->setPopupMode(QToolButton::InstantPopup);
}

void ConverterDialog::addTitleString(QAction *a)
{
    if (m_ui.outFileEdit->cursorPosition () < 1)
        m_ui.outFileEdit->insert(a->data().toString());
    else
        m_ui.outFileEdit->insert(" - "+a->data().toString());
}

void ConverterDialog::createPreset()
{
    PresetEditor *editor = new PresetEditor(QVariantMap(), this);
    if(editor->exec() == QDialog::Accepted)
    {
        QVariantMap data = editor->data();
        data["name"] = uniqueName(data["name"].toString());
        if(data["name"].isValid() && data["ext"].isValid() && data["command"].isValid())
            m_ui.presetComboBox->addItem (data["name"].toString(), data);
    }
    editor->deleteLater();
}

void ConverterDialog::editPreset()
{
    if(m_ui.presetComboBox->currentIndex() == -1)
        return;
    int index = m_ui.presetComboBox->currentIndex();

    PresetEditor *editor = new PresetEditor(m_ui.presetComboBox->itemData(index).toMap(), this);
    if(editor->exec() == QDialog::Accepted)
    {
        QVariantMap data = editor->data();
        if(m_ui.presetComboBox->currentText() != data["name"].toString())
            data["name"] = uniqueName(data["name"].toString());
        if(data["name"].isValid() && data["ext"].isValid() && data["command"].isValid())
        {
            m_ui.presetComboBox->setItemText(index, data["name"].toString());
            m_ui.presetComboBox->setItemData(index, data);
        }
    }
    editor->deleteLater();
}

void ConverterDialog::copyPreset()
{
    if(m_ui.presetComboBox->currentIndex() == -1)
        return;
    int index = m_ui.presetComboBox->currentIndex();
    QVariantMap data = m_ui.presetComboBox->itemData(index).toMap();
    data["name"] = uniqueName(data["name"].toString());
    data["read_only"] = false;
    m_ui.presetComboBox->addItem (data["name"].toString(), data);
}

void ConverterDialog::deletePreset()
{
    if(m_ui.presetComboBox->currentIndex() == -1)
        return;
    if(m_ui.presetComboBox->itemData(m_ui.presetComboBox->currentIndex()).toMap()["read_only"].toBool())
        return;
    m_ui.presetComboBox->removeItem(m_ui.presetComboBox->currentIndex());
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
        m_ui.presetComboBox->addItem (title, data);
    }
}

void ConverterDialog::savePresets()
{
    QDir dir(Qmmp::configDir());
    dir.mkdir("converter");

    QFile file(Qmmp::configDir() + "converter/presets.conf");
    if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        qWarning("ConverterDialog: unable to save presets; error %s", qPrintable(file.errorString()));
        return;
    }

    for(int i = 0; i < m_ui.presetComboBox->count(); ++i)
    {
        QVariantMap data = m_ui.presetComboBox->itemData(i).toMap();
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
        if(m_ui.presetComboBox->findText(unique_name) == -1)
            break;
        unique_name = name + QString("_%1").arg(++i);
    }
    return unique_name;
}
