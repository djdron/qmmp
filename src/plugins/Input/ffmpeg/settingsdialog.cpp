/***************************************************************************
 *   Copyright (C) 2008-2011 by Ilya Kotov                                 *
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

extern "C"{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}
#include "settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent)
        : QDialog(parent)
{
    ui.setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    QStringList filters;
    filters << "*.wma";
    filters << "*.ape";
    filters << "*.shn";
    filters = settings.value("FFMPEG/filters", filters).toStringList();
    avcodec_init();
    avcodec_register_all();
    av_register_all();
    ui.wmaCheckBox->setEnabled(avcodec_find_decoder(CODEC_ID_WMAV1));
    ui.wmaCheckBox->setChecked(filters.contains("*.wma") && avcodec_find_decoder(CODEC_ID_WMAV1));
    ui.apeCheckBox->setEnabled(avcodec_find_decoder(CODEC_ID_APE));
    ui.apeCheckBox->setChecked(filters.contains("*.ape") && avcodec_find_decoder(CODEC_ID_APE));
    ui.ttaCheckBox->setEnabled(avcodec_find_decoder(CODEC_ID_TTA));
    ui.ttaCheckBox->setChecked(filters.contains("*.tta") && avcodec_find_decoder(CODEC_ID_TTA));
    ui.alacCheckBox->setEnabled(avcodec_find_decoder(CODEC_ID_ALAC));
    ui.alacCheckBox->setChecked(filters.contains("*.alac") && avcodec_find_decoder(CODEC_ID_ALAC));
    ui.aacCheckBox->setEnabled(avcodec_find_decoder(CODEC_ID_AAC));
    ui.aacCheckBox->setChecked(filters.contains("*.aac") && avcodec_find_decoder(CODEC_ID_AAC));
    ui.mp3CheckBox->setEnabled(avcodec_find_decoder(CODEC_ID_MP3));
    ui.mp3CheckBox->setChecked(filters.contains("*.mp3") && avcodec_find_decoder(CODEC_ID_MP3));
    ui.mp4CheckBox->setEnabled(avcodec_find_decoder(CODEC_ID_AAC));
    ui.mp4CheckBox->setChecked(filters.contains("*.m4a") && avcodec_find_decoder(CODEC_ID_AAC));
    ui.raCheckBox->setEnabled(avcodec_find_decoder(CODEC_ID_RA_288));
    ui.raCheckBox->setChecked(filters.contains("*.ra") && avcodec_find_decoder(CODEC_ID_RA_288));
    ui.shCheckBox->setChecked(filters.contains("*.shn") && avcodec_find_decoder(CODEC_ID_SHORTEN));
    ui.ac3CheckBox->setChecked(filters.contains("*.ac3") && avcodec_find_decoder(CODEC_ID_AC3));
}

SettingsDialog::~SettingsDialog()
{
}

void SettingsDialog::accept()
{
    QStringList filters;
    if (ui.mp3CheckBox->isChecked())
        filters << "*.mp3";
    if (ui.wmaCheckBox->isChecked())
        filters << "*.wma";
    if (ui.apeCheckBox->isChecked())
        filters << "*.ape";
    if (ui.ttaCheckBox->isChecked())
        filters << "*.tta";
    if (ui.alacCheckBox->isChecked())
        filters << "*.alac";
    if (ui.aacCheckBox->isChecked())
        filters << "*.aac";
    if (ui.mp4CheckBox->isChecked())
        filters << "*.m4a";
    if (ui.raCheckBox->isChecked())
        filters << "*.ra";
    if (ui.shCheckBox->isChecked())
        filters << "*.shn";
    if (ui.ac3CheckBox->isChecked())
        filters << "*.ac3";
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.setValue("FFMPEG/filters", filters);
    QDialog::accept();
}
