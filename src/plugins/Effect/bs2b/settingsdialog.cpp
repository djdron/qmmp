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
#include <bs2b/bs2b.h>
#include <qmmp/qmmp.h>

#include "settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent)
        : QDialog(parent)
{
    ui.setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose, TRUE);
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    ui.levelComboBox->addItem (tr("low"), BS2B_LOW_CLEVEL);
    ui.levelComboBox->addItem (tr("middle"), BS2B_MIDDLE_CLEVEL);
    ui.levelComboBox->addItem (tr("high"), BS2B_HIGH_CLEVEL);
    ui.levelComboBox->addItem (tr("low (easy version)"), BS2B_LOW_ECLEVEL);
    ui.levelComboBox->addItem (tr("middle (easy version)"), BS2B_MIDDLE_ECLEVEL);
    ui.levelComboBox->addItem (tr("high (easy version) [default]"), BS2B_HIGH_ECLEVEL);
    int index = ui.levelComboBox->findData(settings.value("bs2b/level", BS2B_DEFAULT_CLEVEL).toInt());
    if (index >= 0)
        ui.levelComboBox->setCurrentIndex(index);
}

SettingsDialog::~SettingsDialog()
{
}

void SettingsDialog::accept()
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.setValue("bs2b/level", ui.levelComboBox->itemData(ui.levelComboBox->currentIndex()).toInt());
    QDialog::accept();
}
