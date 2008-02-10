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
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <QtGui>

#include "settingsdialog.h"
#include "visualanalyzerfactory.h"
#include "analyzer.h"

const VisualProperties VisualAnalyzerFactory::properties() const
{
    VisualProperties properties;
    properties.name = tr("Analyzer Plugin");
    properties.hasSettings = TRUE;
    properties.hasAbout = TRUE;
    return properties;
};

Visual *VisualAnalyzerFactory::create(QWidget *parent) 
{
    return new Analyzer(parent);
};

void VisualAnalyzerFactory::showSettings(QWidget *parent) 
{
    SettingsDialog *s = new SettingsDialog(parent);
    s -> show();
};

void VisualAnalyzerFactory::showAbout(QWidget *parent) 
{
     QMessageBox::about (parent, tr("About Analyzer Visual Plugin"),
                        tr("Qmmp Analyzer Visual Plugin")+"\n"+
                        tr("Writen by: Ilya Kotov <forkotov02@hotmail.ru>"));
};

QTranslator *VisualAnalyzerFactory::createTranslator(QObject *parent) 
{
    return 0;
};

Q_EXPORT_PLUGIN(VisualAnalyzerFactory)
