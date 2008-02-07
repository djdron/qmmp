/***************************************************************************
 *   Copyright (C) 2007 by Zhuravlev Uriy                                  *
 *   stalkerg@gmail.com                                                    *
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

SettingsDialog::SettingsDialog ( QWidget *parent )
        : QDialog ( parent )
{
    ui.setupUi ( this );
    setAttribute ( Qt::WA_DeleteOnClose );
    connect(ui.okButton, SIGNAL(clicked()), SLOT(writeSettings()));
    QSettings settings(QDir::homePath()+"/.qmmp/qmmprc", QSettings::IniFormat);
    settings.beginGroup("OSS");
    ui.lineEdit->insert(settings.value("device","/dev/dsp").toString());
    ui.lineEdit_2->insert(settings.value("mixer_device","/dev/mixer").toString());
    ui.bufferSpinBox->setValue(settings.value("buffer_time",500).toInt());
    ui.periodSpinBox->setValue(settings.value("period_time",100).toInt());

    settings.endGroup();
}


SettingsDialog::~SettingsDialog()
{}



void SettingsDialog::writeSettings()
{
    qDebug("SettingsDialog (OSS):: writeSettings()");
    QSettings settings(QDir::homePath()+"/.qmmp/qmmprc", QSettings::IniFormat);
    settings.beginGroup("OSS");
    settings.setValue("device", ui.lineEdit->text());
    settings.setValue("buffer_time",ui.bufferSpinBox->value());
    settings.setValue("period_time",ui.periodSpinBox->value());
    settings.setValue("mixer_device", ui.lineEdit_2->text());
    settings.endGroup();
    accept();
}


