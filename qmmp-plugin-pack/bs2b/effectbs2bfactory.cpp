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

#include "effectbs2bfactory.h"
#include "bs2bplugin.h"

const EffectProperties EffectBs2bFactory::properties() const
{
    EffectProperties properties;
    properties.name = tr("BS2B Plugin");
    properties.shortName = "bs2b";
    properties.hasSettings = FALSE;
    properties.hasAbout = FALSE;
    return properties;
};

Effect *EffectBs2bFactory::create(QObject *parent) 
{
    return new Bs2bPlugin(parent);
};

void EffectBs2bFactory::showSettings(QWidget *parent) 
{};

void EffectBs2bFactory::showAbout(QWidget *parent) 
{};

QTranslator *EffectBs2bFactory::createTranslator(QObject *parent) 
{
    QTranslator *translator = new QTranslator(parent);
    QString locale = QLocale::system().name();
    translator->load(QString(":/bs2b_plugin_") + locale);
    return translator;
};

Q_EXPORT_PLUGIN(EffectBs2bFactory)
