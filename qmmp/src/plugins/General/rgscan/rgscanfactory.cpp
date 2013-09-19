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
#include "rgscanhelper.h"
#include "rgscanfactory.h"

const GeneralProperties RGScanFactory::properties() const
{
    GeneralProperties properties;
    properties.name = tr("ReplayGain Scaner");
    properties.shortName = "rgscaner";
    properties.hasAbout = true;
    properties.hasSettings = false;
    properties.visibilityControl = false;
    return properties;
}

QObject *RGScanFactory::create(QObject *parent)
{
    return new RGScanHelper(parent);
}

QDialog *RGScanFactory::createConfigDialog(QWidget *parent)
{
    Q_UNUSED(parent);
    return 0;
}

void RGScanFactory::showAbout(QWidget *parent)
{
    /*QMessageBox::about (parent, tr("About Converter Plugin"),
                        tr("Qmmp Converter Plugin")+"\n"+
                        tr("This plugin converts supported audio files to other file formats "
                           "using external command-line encoders") +"\n"+
                        tr("Written by: Ilya Kotov <forkotov02@hotmail.ru>"));*/
}

QTranslator *RGScanFactory::createTranslator(QObject *parent)
{
    QTranslator *translator = new QTranslator(parent);
    QString locale = Qmmp::systemLanguageID();
    translator->load(QString(":/rgscan_plugin_") + locale);
    return translator;
}

Q_EXPORT_PLUGIN2(rgscaner, RGScanFactory)
