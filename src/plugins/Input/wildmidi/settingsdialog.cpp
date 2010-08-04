/***************************************************************************
 *   Copyright (C) 2010 by Ilya Kotov                                      *
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
#include <QStringList>
#include <qmmp/qmmp.h>
#include "wildmidihelper.h"
#include "settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent)
        : QDialog(parent)
{
    ui.setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("Midi");
    ui.confPathComboBox->setEditText(settings.value("conf_path", "/etc/timidity/timidity.cfg").toString());
    ui.sampleRateComboBox->addItem(tr("44100 Hz"), 44100);
    ui.sampleRateComboBox->addItem(tr("48000 Hz"), 48000);
    int i = ui.sampleRateComboBox->findData(settings.value("sample_rate", 44100).toInt());
    ui.sampleRateComboBox->setCurrentIndex(i);
    ui.enhancedResemplingCheckBox->setChecked(settings.value("enhanced_resampling", false).toBool());
    ui.reverbCheckBox->setChecked(settings.value("reverberation", false).toBool());
    settings.endGroup();
}

SettingsDialog::~SettingsDialog()
{
}

void SettingsDialog::accept()
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("Midi");
    settings.setValue("conf_path", ui.confPathComboBox->currentText());
    settings.setValue("sample_rate",
                      ui.sampleRateComboBox->itemData(ui.sampleRateComboBox->currentIndex()));
    settings.setValue("enhanced_resampling", ui.enhancedResemplingCheckBox->isChecked());
    settings.setValue("reverberation", ui.reverbCheckBox->isChecked());
    settings.endGroup();
    WildMidiHelper::instance()->readSettings();
    QDialog::accept();
}
