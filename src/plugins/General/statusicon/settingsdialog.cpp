/***************************************************************************
 *   Copyright (C) 2008 by Ilya Kotov                                      *
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
#include <QDir>

#include "settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent)
        : QDialog(parent)
{
    ui.setupUi(this);
    QSettings settings(QDir::homePath()+"/.qmmp/qmmprc", QSettings::IniFormat);
    settings.beginGroup("Tray");
    ui.messageCheckBox->setChecked(settings.value("show_message",TRUE).toBool());
    ui.messageDelaySpinBox->setValue(settings.value("message_delay", 2000).toInt());
    ui.toolTipCheckBox->setChecked(settings.value("show_tooltip",FALSE).toBool());
    settings.endGroup();
}


SettingsDialog::~SettingsDialog()
{}

void SettingsDialog::accept()
{
    QSettings settings(QDir::homePath()+"/.qmmp/qmmprc", QSettings::IniFormat);
    settings.beginGroup("Tray");
    settings.setValue ("show_message", ui.messageCheckBox->isChecked());
    settings.setValue ("message_delay", ui.messageDelaySpinBox->value());
    settings.setValue ("show_tooltip", ui.toolTipCheckBox->isChecked());
    settings.endGroup();
    QDialog::accept();
}
