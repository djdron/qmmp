/***************************************************************************
 *   Copyright (C) 2008-2010 by Ilya Kotov                                 *
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

SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent)
{
    ui.setupUi(this);
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("Scrobbler");
    ui.lastfmGroupBox->setChecked(settings.value("use_lastfm", false).toBool());
    ui.sessionLineEdit_lastfm->setText(settings.value("lastfm_session").toString());
    ui.librefmGroupBox->setChecked(settings.value("use_librefm", false).toBool());
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
    if(ui.newSessionCheckBox_lastfm->isChecked())
        ui.sessionLineEdit_lastfm->clear();
    settings.setValue("lastfm_session",ui.sessionLineEdit_lastfm->text());
    settings.setValue("use_librefm", ui.librefmGroupBox->isChecked());
    settings.setValue("librefm_login",ui.userLineEdit_libre->text());
    settings.setValue("librefm_password", ui.passwordLineEdit_libre->text());
    settings.endGroup();
    QDialog::accept();
}
