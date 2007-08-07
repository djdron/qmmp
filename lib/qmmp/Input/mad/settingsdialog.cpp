/***************************************************************************
 *   Copyright (C) 2006 by Ilya Kotov                                      *
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
#include <QTextCodec>
#include <QSettings>
#include <QDir>

#include "settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent)
        : QDialog(parent)
{
    ui.setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    findCodecs();
    foreach (QTextCodec *codec, codecs)
    {
        ui.id3v1EncComboBox->addItem(codec->name());
        ui.id3v2EncComboBox->addItem(codec->name());
    }
    QSettings settings(QDir::homePath()+"/.qmmp/qmmprc", QSettings::IniFormat);
    settings.beginGroup("MAD");
    int pos = ui.id3v1EncComboBox->findText
        (settings.value("ID3v1_encoding","UTF-8").toString());
    ui.id3v1EncComboBox->setCurrentIndex(pos);
    pos = ui.id3v2EncComboBox->findText
        (settings.value("ID3v2_encoding","UTF-8").toString());
    ui.id3v2EncComboBox->setCurrentIndex(pos);

    ui.id3v1CheckBox->setChecked(settings.value("ID3v1_enable", TRUE).toBool());
    ui.id3v2CheckBox->setChecked(settings.value("ID3v2_enable", TRUE).toBool());
    ui.id3v1EncComboBox->setEnabled(ui.id3v1CheckBox->isChecked());
    ui.id3v2EncComboBox->setEnabled(ui.id3v2CheckBox->isChecked());
    ui.defTagComboBox->setCurrentIndex(settings.value("ID3_version", 2).toInt() - 1);

    settings.endGroup();
    connect(ui.okButton, SIGNAL(clicked()), SLOT(writeSettings()));
}


SettingsDialog::~SettingsDialog()
{}

void SettingsDialog::writeSettings()
{
    QSettings settings(QDir::homePath()+"/.qmmp/qmmprc", QSettings::IniFormat);
    settings.beginGroup("MAD");
    settings.setValue("ID3v1_encoding", ui.id3v1EncComboBox->currentText());
    settings.setValue("ID3v2_encoding", ui.id3v2EncComboBox->currentText());
    settings.setValue("ID3v1_enable", ui.id3v1CheckBox->isChecked());
    settings.setValue("ID3v2_enable", ui.id3v2CheckBox->isChecked());
    settings.setValue("ID3_version", ui.defTagComboBox->currentIndex()+1);

    settings.endGroup();
    accept();
}

void SettingsDialog::findCodecs()
{
    QMap<QString, QTextCodec *> codecMap;
    QRegExp iso8859RegExp("ISO[- ]8859-([0-9]+).*");

    foreach (int mib, QTextCodec::availableMibs())
    {
        QTextCodec *codec = QTextCodec::codecForMib(mib);

        QString sortKey = codec->name().toUpper();
        int rank;

        if (sortKey.startsWith("UTF-8"))
        {
            rank = 1;
        }
        else if (sortKey.startsWith("UTF-16"))
        {
            rank = 2;
        }
        else if (iso8859RegExp.exactMatch(sortKey))
        {
            if (iso8859RegExp.cap(1).size() == 1)
                rank = 3;
            else
                rank = 4;
        }
        else
        {
            rank = 5;
        }
        sortKey.prepend(QChar('0' + rank));

        codecMap.insert(sortKey, codec);
    }
    codecs = codecMap.values();
}
