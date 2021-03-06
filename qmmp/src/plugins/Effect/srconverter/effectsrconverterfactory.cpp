/***************************************************************************
 *   Copyright (C) 2007 by Ilya Kotov                                      *
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
#include <qmmp/qmmp.h>
#include "settingsdialog.h"
#include "srconverter.h"
#include "effectsrconverterfactory.h"

const EffectProperties EffectSRConverterFactory::properties() const
{
    EffectProperties properties;
    properties.name = tr("SRC Plugin");
    properties.shortName = "SRC";
    properties.hasSettings = true;
    properties.hasAbout = true;
    properties.priority = EffectProperties::EFFECT_PRIORITY_HIGH;
    return properties;
}

Effect *EffectSRConverterFactory::create()
{
    return new SRConverter();
}

void EffectSRConverterFactory::showSettings(QWidget *parent) 
{
    SettingsDialog *s = new SettingsDialog(parent);
    s ->show();
}

void EffectSRConverterFactory::showAbout(QWidget *parent) 
{
     QMessageBox::about (parent, tr("About Sample Rate Converter Plugin"),
                        tr("Qmmp Sample Rate Converter Plugin")+"\n"+
                        tr("Written by: Ilya Kotov <forkotov02@hotmail.ru>"));
}

QTranslator *EffectSRConverterFactory::createTranslator(QObject *parent) 
{
    QTranslator *translator = new QTranslator(parent);
    QString locale = Qmmp::systemLanguageID();
    translator->load(QString(":/srconverter_plugin_") + locale);
    return translator;
}

Q_EXPORT_PLUGIN2(srconverter, EffectSRConverterFactory)
