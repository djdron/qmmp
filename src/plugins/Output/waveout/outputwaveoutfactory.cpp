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

#include <QtGui>

#include "outputwaveout.h"
#include "outputwaveoutfactory.h"


const OutputProperties OutputWaveOutFactory::properties() const
{
    OutputProperties properties;
    properties.name = tr("WaveOut Plugin");
    properties.hasAbout = TRUE;
    properties.hasSettings = FALSE;
    return properties;
}

Output* OutputWaveOutFactory::create(QObject* parent)
{
    return new OutputWaveOut(parent);
}

VolumeControl *OutputWaveOutFactory::createVolumeControl(QObject *)
{
    return 0;
}

void OutputWaveOutFactory::showSettings(QWidget* parent)
{
   Q_UNUSED(parent);
}

void OutputWaveOutFactory::showAbout(QWidget *parent)
{
   QMessageBox::about (parent, tr("About WaveOut Output Plugin"),
                        tr("Qmmp WaveOut Output Plugin")+"\n"+
                        tr("Writen by: Ilya Kotov <forkotov02@hotmail.ru>") + "\n" +
                        tr("Based on aacDECdrop player"));
}

QTranslator *OutputWaveOutFactory::createTranslator(QObject *parent)
{
    QTranslator *translator = new QTranslator(parent);
    QString locale = QLocale::system().name();
    translator->load(QString(":/waveout_plugin_") + locale);
    return translator;
}

Q_EXPORT_PLUGIN(OutputWaveOutFactory)
