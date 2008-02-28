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

#include "notifier.h"
#include "settingsdialog.h"
#include "notifierfactory.h"

const GeneralProperties NotifierFactory::properties() const
{
    GeneralProperties properties;
    properties.name = tr("Notifier Plugin");
    properties.hasAbout = TRUE;
    properties.hasSettings = TRUE;
    return properties;
}

General *NotifierFactory::create(QObject *parent)
{
    return new Notifier(parent);
}

QDialog *NotifierFactory::createConfigDialog(QWidget *parent)
{
    return new SettingsDialog(parent);
}

void NotifierFactory::showAbout(QWidget *parent)
{
    QMessageBox::about (parent, tr("About Notifier Plugin"),
                        tr("Qmmp Desktop Notifier Plugin")+"\n"+
                        tr("Writen by: Ilya Kotov <forkotov02@hotmail.ru>"));
}

QTranslator *NotifierFactory::createTranslator(QObject *parent)
{
    return 0;
}

Q_EXPORT_PLUGIN(NotifierFactory)
