/***************************************************************************
 *   Copyright (C) 2008-2013 by Ilya Kotov                                 *
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
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#include <QSettings>
#include <QMessageBox>
#include <qmmp/qmmp.h>
#include "lastfmscrobbler.h"
#include "settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent)
{
    m_ui.setupUi(this);
    m_lastfmAuth = new LastfmAuth(this);
    connect(m_lastfmAuth, SIGNAL(tokenRequestFinished(int)), SLOT(processTokenResponse(int)));
    connect(m_lastfmAuth, SIGNAL(sessionRequestFinished(int)), SLOT(processSessionResponse(int)));
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("Scrobbler");
    m_ui.lastfmGroupBox->setChecked(settings.value("use_lastfm", false).toBool());
    m_ui.sessionLineEdit_lastfm->setText(settings.value("lastfm_session").toString());
    m_ui.librefmGroupBox->setChecked(settings.value("use_librefm", false).toBool());
    m_ui.userLineEdit_libre->setText(settings.value("librefm_login").toString());
    m_ui.passwordLineEdit_libre->setText(settings.value("librefm_password").toString());
    settings.endGroup();
}

SettingsDialog::~SettingsDialog()
{}

void SettingsDialog::accept()
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("Scrobbler");
    settings.setValue("use_lastfm", m_ui.lastfmGroupBox->isChecked());
    settings.setValue("lastfm_session",m_ui.sessionLineEdit_lastfm->text());
    settings.setValue("use_librefm", m_ui.librefmGroupBox->isChecked());
    settings.setValue("librefm_login",m_ui.userLineEdit_libre->text());
    settings.setValue("librefm_password", m_ui.passwordLineEdit_libre->text());
    settings.endGroup();
    QDialog::accept();
}

void SettingsDialog::on_newSessionButton_lastfm_clicked()
{
    m_lastfmAuth->getToken();
}

void SettingsDialog::processTokenResponse(int error)
{
    switch(error)
    {
    case LastfmAuth::NO_ERROR:
        QMessageBox::information(this,
                                 tr("Message"),
                                 tr("1. Wait for browser startup.") + "\n" +
                                 tr("2. Allow Qmmp to scrobble tracks to your Last.fm account.") + "\n" +
                                 tr("3. Press \"OK\"."));
        m_lastfmAuth->getSession();
        break;
    case LastfmAuth::NETWORK_ERROR:
        QMessageBox::warning(this, tr("Error"), tr("Network error."));
        break;
    case LastfmAuth::LASTFM_ERROR:
    default:
        QMessageBox::warning(this, tr("Error"), tr("Unable to register new session."));
    }
}

void SettingsDialog::processSessionResponse(int error)
{
    switch(error)
    {
    case LastfmAuth::NO_ERROR:
        QMessageBox::information(this, tr("Message"), tr("New session has been received successfully."));
        m_ui.sessionLineEdit_lastfm->setText(m_lastfmAuth->session());
        QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
        settings.setValue("lastfm_session",m_ui.sessionLineEdit_lastfm->text());
        break;
    case LastfmAuth::NETWORK_ERROR:
        QMessageBox::warning(this, tr("Error"), tr("Network error."));
        break;
    case LastfmAuth::LASTFM_ERROR:
    default:
        QMessageBox::warning(this, tr("Error"), tr("Unable to register new session."));
    }
}
