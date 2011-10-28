/***************************************************************************
 *   Copyright (C) 2011 by Ilya Kotov                                      *
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
#include <QDir>
#include <QFontDialog>
#include <qmmp/qmmp.h>
#include <qmmpui/filedialog.h>
#include <qmmpui/uihelper.h>
#include "simplesettings.h"

SimpleSettings::SimpleSettings(QWidget *parent) : QWidget(parent)
{
    ui.setupUi(this);
    readSettings();
    loadFonts();
    //setup icons
    ui.popupTemplateButton->setIcon(QIcon::fromTheme("configure"));
}

SimpleSettings::~SimpleSettings()
{}

void SimpleSettings::on_plFontButton_clicked()
{
    bool ok;
    QFont font = ui.plFontLabel->font();
    font = QFontDialog::getFont (&ok, font, this);
    if (ok)
    {
        ui.plFontLabel->setText (font.family () + " " + QString::number(font.pointSize ()));
        ui.plFontLabel->setFont(font);
        QSettings settings (Qmmp::configFile(), QSettings::IniFormat);
        settings.setValue ("Simple/pl_font", font.toString());
    }
}

void SimpleSettings::showEvent(QShowEvent *)
{
    ui.hiddenCheckBox->setEnabled(UiHelper::instance()->visibilityControl());
    ui.hideOnCloseCheckBox->setEnabled(UiHelper::instance()->visibilityControl());
}

void SimpleSettings::loadFonts()
{
    QSettings settings (Qmmp::configFile(), QSettings::IniFormat);
    QString fontname = settings.value ("Simple/pl_font").toString();
    QFont font = QApplication::font();
    if(!fontname.isEmpty())
        font.fromString(fontname);
    ui.plFontLabel->setText (font.family () + " " + QString::number(font.pointSize ()));
    ui.plFontLabel->setFont(font);
}

void SimpleSettings::on_popupTemplateButton_clicked()
{
    /*PopupSettings *p = new PopupSettings(this);
    p->exec();
    p->deleteLater();*/
}

void SimpleSettings::readSettings()
{
    QSettings settings (Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("Simple");
    //playlist
    ui.protocolCheckBox->setChecked(settings.value ("pl_show_protocol", false).toBool());
    ui.numbersCheckBox->setChecked(settings.value ("pl_show_numbers", true).toBool());
    ui.alignCheckBox->setChecked(settings.value ("pl_align_numbers", false).toBool());
    ui.anchorCheckBox->setChecked(settings.value("pl_show_anchor", false).toBool());
    ui.popupCheckBox->setChecked(settings.value("pl_show_popup", false).toBool());
    //view
    ui.hiddenCheckBox->setChecked(settings.value("start_hidden", false).toBool());
    ui.hideOnCloseCheckBox->setChecked(settings.value("hide_on_close", false).toBool());
    settings.endGroup();
}

void SimpleSettings::writeSettings()
{
    QSettings settings (Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("Simple");
    settings.setValue ("pl_show_protocol", ui.protocolCheckBox->isChecked());
    settings.setValue ("pl_show_numbers", ui.numbersCheckBox->isChecked());
    settings.setValue ("pl_align_numbers", ui.alignCheckBox->isChecked());
    settings.setValue ("pl_show_anchor", ui.anchorCheckBox->isChecked());
    settings.setValue ("pl_show_popup", ui.popupCheckBox->isChecked());
    settings.setValue ("start_hidden", ui.hiddenCheckBox->isChecked());
    settings.setValue ("hide_on_close", ui.hideOnCloseCheckBox->isChecked());
    settings.endGroup();
}
