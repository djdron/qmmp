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
#include <qmmp/qmmp.h>
#include "visualprojectmfactory.h"
#include "projectmplugin.h"

const VisualProperties VisualProjectMFactory::properties() const
{
    VisualProperties properties;
    properties.name = tr("ProjectM");
    properties.shortName = "projectm";
    properties.hasSettings = FALSE;
    properties.hasAbout = TRUE;
    return properties;
}

Visual *VisualProjectMFactory::create(QWidget *parent)
{
    return new ProjectMPlugin(parent);
}

QDialog *VisualProjectMFactory::createConfigDialog(QWidget *parent)
{
    Q_UNUSED(parent);
    return 0;
}

void VisualProjectMFactory::showAbout(QWidget *parent)
{
    QMessageBox::about (parent, tr("About ProjectM Visual Plugin"),
                        tr("Qmmp ProjectM Visual Plugin")+"\n"+
                        tr("This plugins adds projectM visualization")+"\n"+
                        tr("Writen by: Ilya Kotov <forkotov02@hotmail.ru>")+"\n"+
                        tr("Based on llibrojectM-qt library"));
}

QTranslator *VisualProjectMFactory::createTranslator(QObject *parent)
{
    QTranslator *translator = new QTranslator(parent);
    QString locale = Qmmp::systemLanguageID();
    translator->load(QString(":/projectm_plugin_") + locale);
    return translator;
}

Q_EXPORT_PLUGIN2(projectm,VisualProjectMFactory)
