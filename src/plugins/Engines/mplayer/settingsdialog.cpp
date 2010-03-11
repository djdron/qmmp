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
#include <QSettings>

#include <qmmp/qmmp.h>

#include "settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent)
        : QDialog(parent)
{
    ui.setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    ui.videoComboBox->addItem(tr("default"));
    ui.videoComboBox->addItem("xv");
    ui.videoComboBox->addItem("x11");
    ui.videoComboBox->addItem("gl");
    ui.videoComboBox->addItem("gl2");
    ui.videoComboBox->addItem("dga");
    ui.videoComboBox->addItem("sdl");
    ui.videoComboBox->addItem("null");
    ui.audioComboBox->addItem(tr("default"));
    ui.audioComboBox->addItem("oss");
    ui.audioComboBox->addItem("alsa");
    ui.audioComboBox->addItem("pulse");
    ui.audioComboBox->addItem("jack");
    ui.audioComboBox->addItem("nas");
    ui.audioComboBox->addItem("null");
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("mplayer");
    ui.audioComboBox->setEditText(settings.value("ao","default").toString().replace("default", tr("default")));
    ui.videoComboBox->setEditText(settings.value("vo","default").toString().replace("default", tr("default")));
    ui.autoSyncCheckBox->setChecked(settings.value("autosync", false).toBool());
    ui.syncFactorSpinBox->setValue(settings.value("autosync_factor", 100).toInt());
    settings.endGroup();
}


SettingsDialog::~SettingsDialog()
{}

void SettingsDialog::accept()
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("mplayer");
    settings.setValue("ao",ui.audioComboBox->currentText().replace(tr("default"), "default"));
    settings.setValue("vo",ui.videoComboBox->currentText().replace(tr("default"), "default"));
    settings.setValue("autosync",ui.autoSyncCheckBox->isChecked());
    settings.setValue("autosync_factor",ui.syncFactorSpinBox->value());
    settings.endGroup();
    QDialog::accept();
}
