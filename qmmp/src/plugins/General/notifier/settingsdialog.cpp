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
#include <QFontDialog>

#include <qmmp/qmmp.h>

#include "popupwidget.h"
#include "settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent)
        : QDialog(parent)
{
    ui.setupUi(this);

    m_buttons.insert(PopupWidget::TOPLEFT, ui.topLeftButton);
    m_buttons.insert(PopupWidget::TOP, ui.topButton);
    m_buttons.insert(PopupWidget::TOPRIGHT, ui.topRightButton);
    m_buttons.insert(PopupWidget::RIGHT, ui.rightButton);
    m_buttons.insert(PopupWidget::BOTTOMRIGHT, ui.bottomRightButton);
    m_buttons.insert(PopupWidget::BOTTOM, ui.bottomButton);
    m_buttons.insert(PopupWidget::BOTTOMLEFT, ui.bottomLeftButton);
    m_buttons.insert(PopupWidget::LEFT, ui.leftButton);
    m_buttons.insert(PopupWidget::CENTER, ui.centerButton);

    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("Notifier");
    ui.messageDelaySpinBox->setValue(settings.value("message_delay", 2000).toInt());
    uint pos = settings.value("message_pos", PopupWidget::BOTTOMLEFT).toUInt();
    m_buttons.value(pos)->setChecked(TRUE);
    ui.psiCheckBox->setChecked(settings.value("psi_notification", FALSE).toBool());
    ui.songCheckBox->setChecked(settings.value("song_notification", TRUE).toBool());
    ui.volumeCheckBox->setChecked(settings.value("volume_notification", TRUE).toBool());
    ui.transparencySlider->setValue(100 - settings.value("opacity", 1.0).toDouble()*100);
    QString fontname = settings.value ("font").toString();
    ui.sizeSpinBox->setValue(settings.value ("cover_size", 64).toInt());
    settings.endGroup();
    QFont font;
    if(!fontname.isEmpty())
        font.fromString(fontname);
    ui.fontLabel->setText (font.family () + " " + QString::number(font.pointSize ()));
    ui.fontLabel->setFont(font);
    connect (ui.fontButton, SIGNAL (clicked()), SLOT (setFont()));
}


SettingsDialog::~SettingsDialog()
{}

void SettingsDialog::accept()
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("Notifier");
    settings.setValue ("message_delay", ui.messageDelaySpinBox->value());
    uint pos = PopupWidget::BOTTOMLEFT;
    foreach (QPushButton *button, m_buttons.values())
    {
        if(button->isChecked())
            pos = m_buttons.key(button);
    }
    settings.setValue("message_pos", pos);
    settings.setValue("psi_notification", ui.psiCheckBox->isChecked());
    settings.setValue("song_notification", ui.songCheckBox->isChecked());
    settings.setValue("volume_notification", ui.volumeCheckBox->isChecked());
    settings.setValue ("opacity", 1.0 -  (double)ui.transparencySlider->value()/100);
    settings.setValue ("font", ui.fontLabel->font().toString());
    settings.setValue ("cover_size", ui.sizeSpinBox->value());
    settings.endGroup();
    QDialog::accept();
}

void SettingsDialog::setFont()
{
    bool ok;
    QFont font = ui.fontLabel->font();
    font = QFontDialog::getFont (&ok, font, this);
    if (ok)
    {
        ui.fontLabel->setText (font.family () + " " + QString::number(font.pointSize ()));
        ui.fontLabel->setFont(font);
    }
}
