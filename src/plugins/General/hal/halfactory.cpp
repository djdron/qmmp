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

#include "halplugin.h"
#include "halfactory.h"

const GeneralProperties HalFactory::properties() const
{
    GeneralProperties properties;
    properties.name = tr("HAL Plugin");
    properties.hasAbout = FALSE;
    properties.hasSettings = FALSE;
    properties.visibilityControl = FALSE;
    return properties;
}

General *HalFactory::create(QObject *parent)
{
    return new HalPlugin(parent);
}

QDialog *HalFactory::createConfigDialog(QWidget *)
{
    return 0;
}

void HalFactory::showAbout(QWidget *parent)
{
    /*QMessageBox::about (parent, tr("About Hal Plugin"),
                        tr("Qmmp Hal Plugin")+"\n"+
                        tr("Writen by: Ilya Kotov <forkotov02@hotmail.ru>"));*/
    //TODO based on solid
}

QTranslator *HalFactory::createTranslator(QObject *parent)
{
    QTranslator *translator = new QTranslator(parent);
    QString locale = QLocale::system().name();
    translator->load(QString(":/hal_plugin_") + locale);
    return translator;
}

Q_EXPORT_PLUGIN(HalFactory)
