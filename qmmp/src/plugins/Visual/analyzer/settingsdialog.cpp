/***************************************************************************
 *   Copyright (C) 2007 by Ilya Kotov                                      *
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
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    ui.analyzerComboBox->setCurrentIndex(settings.value("Analyzer/analyzer_falloff", 3).toInt()-1);
    ui.peaksCheckBox->setChecked(settings.value("Analyzer/show_peaks", true).toBool());
    ui.peaksComboBox->setCurrentIndex(settings.value("Analyzer/peaks_falloff", 3).toInt()-1);
    ui.fpsComboBox->setCurrentIndex(settings.value("Analyzer/refresh_rate", 2).toInt()-1);
    ui.colorWidget1->setColor(settings.value("Analyzer/color1", "Green").toString());
    ui.colorWidget2->setColor(settings.value("Analyzer/color2", "Yellow").toString());
    ui.colorWidget3->setColor(settings.value("Analyzer/color3", "Red").toString());
    ui.bgColorWidget->setColor(settings.value("Analyzer/bg_color", "Black").toString());
    ui.peakColorWidget->setColor(settings.value("Analyzer/peak_color", "Cyan").toString());
}


SettingsDialog::~SettingsDialog()
{
}

void SettingsDialog::accept()
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.setValue("Analyzer/analyzer_falloff", ui.analyzerComboBox->currentIndex() + 1);
    settings.setValue("Analyzer/peaks_falloff", ui.peaksComboBox->currentIndex() + 1);
    settings.setValue("Analyzer/refresh_rate", ui.fpsComboBox->currentIndex() + 1);
    settings.setValue("Analyzer/show_peaks", ui.peaksCheckBox->isChecked());
    settings.setValue("Analyzer/color1", ui.colorWidget1->colorName());
    settings.setValue("Analyzer/color2", ui.colorWidget2->colorName());
    settings.setValue("Analyzer/color3", ui.colorWidget3->colorName());
    settings.setValue("Analyzer/bg_color", ui.bgColorWidget->colorName());
    settings.setValue("Analyzer/peak_color", ui.peakColorWidget->colorName());
    QDialog::accept();
}
