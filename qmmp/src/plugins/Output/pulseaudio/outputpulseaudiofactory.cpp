/***************************************************************************
 *   Copyright (C) 2007-2008 by Ilya Kotov                                 *
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

#include <QtGui>
#include <qmmp/qmmp.h>
#include "outputpulseaudio.h"
#include "outputpulseaudiofactory.h"


const OutputProperties OutputPulseAudioFactory::properties() const
{
    OutputProperties properties;
    properties.name = tr("PulseAudio Plugin");
    properties.hasAbout = TRUE;
    properties.hasSettings = FALSE;
    properties.shortName = "pulse";
    return properties;
}

Output* OutputPulseAudioFactory::create(QObject* parent)
{
    return new OutputPulseAudio(parent);
}

VolumeControl *OutputPulseAudioFactory::createVolumeControl(QObject *)
{
    return 0;
}

void OutputPulseAudioFactory::showSettings(QWidget* parent)
{
   Q_UNUSED(parent);
}

void OutputPulseAudioFactory::showAbout(QWidget *parent)
{
   QMessageBox::about (parent, tr("About PulseAudio Output Plugin"),
                        tr("Qmmp PulseAudio Output Plugin")+"\n"+
                        tr("Writen by: Ilya Kotov <forkotov02@hotmail.ru>"));
}

QTranslator *OutputPulseAudioFactory::createTranslator(QObject *parent)
{
    QTranslator *translator = new QTranslator(parent);
    QString locale = Qmmp::systemLanguageID();
    translator->load(QString(":/pulseaudio_plugin_") + locale);
    return translator;
}

Q_EXPORT_PLUGIN2(pulseaudio, OutputPulseAudioFactory)
