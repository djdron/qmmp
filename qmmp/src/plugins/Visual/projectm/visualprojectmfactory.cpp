/***************************************************************************
 *   Copyright (C) 2008 by Ilya Kotov                                      *
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

//#include "settingsdialog.h"
#include "visualprojectmfactory.h"
#include "analyzer.h"

const VisualProperties VisualProjectMFactory::properties() const
{
    VisualProperties properties;
    properties.name = tr("ProjectM Plugin");
    properties.shortName = "projectm";
    properties.hasSettings = FALSE;
    properties.hasAbout = TRUE;
    return properties;
};

Visual *VisualProjectMFactory::create(QWidget *parent)
{
    return new Analyzer(parent);
};

QDialog *VisualProjectMFactory::createConfigDialog(QWidget *parent)
{
    return 0;
};

void VisualProjectMFactory::showAbout(QWidget *parent)
{
    QMessageBox::about (parent, tr("About Analyzer Visual Plugin"),
                        tr("Qmmp Analyzer Visual Plugin")+"\n"+
                        tr("Writen by: Ilya Kotov <forkotov02@hotmail.ru>"));
};

QTranslator *VisualProjectMFactory::createTranslator(QObject *parent)
{
    QTranslator *translator = new QTranslator(parent);
    QString locale = QLocale::system().name();
    translator->load(QString(":/analyzer_plugin_") + locale);
    return translator;
};

Q_EXPORT_PLUGIN(VisualProjectMFactory)
