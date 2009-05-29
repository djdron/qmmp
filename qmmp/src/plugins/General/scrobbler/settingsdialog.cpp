/***************************************************************************
 *   Copyright (C) 2008-2009 by Ilya Kotov                                 *
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
    settings.beginGroup("Scrobbler");
    ui.lastfmGroupBox->setChecked(settings.value("use_lastfm", FALSE).toBool());
    ui.userLineEdit->setText(settings.value("lastfm_login").toString());
    ui.passwordLineEdit->setText(settings.value("lastfm_password").toString());
    ui.librefmGroupBox->setChecked(settings.value("use_librefm", FALSE).toBool());
    ui.userLineEdit_libre->setText(settings.value("librefm_login").toString());
    ui.passwordLineEdit_libre->setText(settings.value("librefm_password").toString());
    settings.endGroup();
}


SettingsDialog::~SettingsDialog()
{}

void SettingsDialog::accept()
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("Scrobbler");
    settings.setValue("use_lastfm", ui.lastfmGroupBox->isChecked());
    settings.setValue("lastfm_login",ui.userLineEdit->text());
    settings.setValue("lastfm_password", ui.passwordLineEdit->text());
    settings.setValue("use_librefm", ui.librefmGroupBox->isChecked());
    settings.setValue("librefm_login",ui.userLineEdit_libre->text());
    settings.setValue("librefm_password", ui.passwordLineEdit_libre->text());
    settings.endGroup();
    QDialog::accept();
}
