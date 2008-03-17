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

#include "dbuscontrol.h"
#include "dbuscontrolfactory.h"

const GeneralProperties DBUSControlFactory::properties() const
{
    GeneralProperties properties;
    properties.name = tr("D-Bus Plugin");
    properties.hasAbout = TRUE;
    properties.hasSettings = FALSE;
    properties.visibilityControl = FALSE;
    return properties;
}

General *DBUSControlFactory::create(Control *control, QObject *parent)
{
    return new DBUSControl(control, parent);
}

QDialog *DBUSControlFactory::createConfigDialog(QWidget *)
{
    return 0;
}

void DBUSControlFactory::showAbout(QWidget *parent)
{
    QMessageBox::about (parent, tr("About D-Bus Plugin"),
                        tr("Qmmp D-Bus Plugin")+"\n"+
                        tr("Writen by: Ilya Kotov <forkotov02@hotmail.ru>"));
}

QTranslator *DBUSControlFactory::createTranslator(QObject *)
{
    return 0;
}

Q_EXPORT_PLUGIN(DBUSControlFactory)
