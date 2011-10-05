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

#include "preseteditor.h"
#include "ui_preseteditor.h"

PresetEditor::PresetEditor(const QVariantMap &data, QWidget *parent) :
    QDialog(parent), m_ui(new Ui::PresetEditor)
{
    m_ui->setupUi(this);
    m_ui->nameLineEdit->setText(data.value("name").toString());
    m_ui->extensionLineEdit->setText(data.value("ext").toString());
    m_ui->commandLineEdit->setText(data.value("command").toString());
    m_ui->us16bitCheckBox->setChecked(data.value("use_16bit").toBool());
    m_ui->tagsCheckBox->setChecked(data.value("tags").toBool());
}

PresetEditor::~PresetEditor()
{
    delete m_ui;
}

const QVariantMap PresetEditor::data() const
{
    QVariantMap data;
    data.insert("name", m_ui->nameLineEdit->text());
    data.insert("ext", m_ui->extensionLineEdit->text());
    data.insert("command", m_ui->commandLineEdit->text());
    data.insert("use_16bit",  m_ui->us16bitCheckBox->isChecked());
    data.insert("tags", m_ui->tagsCheckBox->isChecked());
    data.insert("read_only", false);
    return data;
}
