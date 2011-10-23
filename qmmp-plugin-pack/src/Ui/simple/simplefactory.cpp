/***************************************************************************
 *   Copyright (C) 2011 by Ilya Kotov                                      *
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

#include <QtPlugin>
#include <QMessageBox>
#include "mainwindow.h"
#include "simplefactory.h"

const UiProperties SimpleFactory::properties() const
{
    UiProperties props;
    props.hasAbout = false;
    props.name = tr("Simple User Interface");
    props.shortName = "simple";
    return props;
}

QObject *SimpleFactory::SimpleFactory::create()
{
    return new MainWindow();
}

void SimpleFactory::showAbout(QWidget *)
{}

QTranslator *SimpleFactory::createTranslator(QObject *parent)
{
    QTranslator *translator = new QTranslator(parent);
    QString locale = Qmmp::systemLanguageID();
    translator->load(QString(":/simple_plugin_") + locale);
    return translator;
}

Q_EXPORT_PLUGIN2(simple, SimpleFactory)
