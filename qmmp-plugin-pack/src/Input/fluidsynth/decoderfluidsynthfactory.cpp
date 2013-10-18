/***************************************************************************
 *   Copyright (C) 2013 by Ilya Kotov                                      *
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

#include <QtGui>
#include "decoder_fluidsynth.h"
#include "settingsdialog.h"
#include "decoderfluidsynthfactory.h"

// DecoderFluidSynthFactory

bool DecoderFluidSynthFactory::supports(const QString &source) const
{
    return source.endsWith(".mid", Qt::CaseInsensitive);
}

bool DecoderFluidSynthFactory::canDecode(QIODevice *) const
{
    return false;
}

const DecoderProperties DecoderFluidSynthFactory::properties() const
{
    DecoderProperties properties;
    properties.name = tr("FluidSynth Plugin");
    properties.filters << "*.mid";
    properties.description = tr("Midi Files");
    //properties.contentType = ;
    properties.shortName = "fluidsynth";
    properties.hasAbout = true;
    properties.hasSettings = true;
    properties.noInput = true;
    properties.protocols << "file";
    properties.priority = 10;
    //FluidSynthHelper::instance()->initialize();
    return properties;
}

Decoder *DecoderFluidSynthFactory::create(const QString &path, QIODevice *input)
{
    Q_UNUSED(input);
    return new DecoderFluidSynth(path);
}

QList<FileInfo *> DecoderFluidSynthFactory::createPlayList(const QString &fileName, bool useMetaData)
{
    Q_UNUSED(useMetaData);
    QList <FileInfo*> list;
    /*FileInfo *info = new FileInfo(fileName);

    if(FluidSynthHelper::instance()->initialize() && FluidSynthHelper::instance()->sampleRate())
    {
        void *midi_ptr = FluidSynth_Open (fileName.toLocal8Bit());
        if(midi_ptr)
        {
            FluidSynthHelper::instance()->addPtr(midi_ptr);
            _WM_Info *wm_info = FluidSynth_GetInfo(midi_ptr);
            info->setLength((qint64)wm_info->approx_total_samples
                            / FluidSynthHelper::instance()->sampleRate());
            FluidSynth_Close(midi_ptr);
            FluidSynthHelper::instance()->removePtr(midi_ptr);
        }
    }
    list << info;*/
    return list;
}

MetaDataModel* DecoderFluidSynthFactory::createMetaDataModel(const QString &path, QObject *parent)
{
    Q_UNUSED(path);
    Q_UNUSED(parent);
    return 0;
}

void DecoderFluidSynthFactory::showSettings(QWidget *parent)
{
    SettingsDialog *d = new SettingsDialog(parent);
    d->show();
}

void DecoderFluidSynthFactory::showAbout(QWidget *parent)
{
    QMessageBox::about (parent, tr("About FluidSynth Audio Plugin"),
                        tr("Qmmp FluidSynth Audio Plugin")+"\n"+
                        tr("This plugin uses FluidSynth library to play midi files")+"\n"+
                        tr("Written by: Ilya Kotov <forkotov02@hotmail.ru>"));
}

QTranslator *DecoderFluidSynthFactory::createTranslator(QObject *parent)
{
    QTranslator *translator = new QTranslator(parent);
    QString locale = QLocale::system().name();
    translator->load(QString(":/fluidsynth_plugin_") + locale);
    return translator;
}
Q_EXPORT_PLUGIN2(fluidsynth,DecoderFluidSynthFactory)
