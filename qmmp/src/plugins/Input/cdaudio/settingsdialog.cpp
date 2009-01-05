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

#include <qmmp/qmmp.h>

#include "settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent)
        : QDialog(parent)
{
    ui.setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("cdaudio");
    ui.deviceLineEdit->setText(settings.value("device").toString());
    ui.deviceCheckBox->setChecked(!ui.deviceLineEdit->text().isEmpty());
    int speed = settings.value("speed", 0).toInt();
    ui.speedCheckBox->setChecked(speed > 0);
    ui.speedSpinBox->setValue(speed);
    ui.cdtextCheckBox->setChecked(settings.value("cdtext", TRUE).toBool());
    settings.endGroup();
}


SettingsDialog::~SettingsDialog()
{}

void SettingsDialog::accept()
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("cdaudio");
    if(ui.deviceCheckBox->isChecked())
        settings.setValue("device", ui.deviceLineEdit->text());
    else
        settings.remove("device");
    if(ui.speedCheckBox->isChecked())
        settings.setValue("speed", ui.speedSpinBox->value());
    else
        settings.setValue("speed", 0);
    settings.setValue("cdtext", ui.cdtextCheckBox->isChecked());
    settings.endGroup();
    QDialog::accept();
}
