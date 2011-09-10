/***************************************************************************
 *   Copyright (C) 2006-2011 by Ilya Kotov                                 *
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

#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include <QLibraryInfo>
#include <QIcon>
#include <stdio.h>
#include <stdlib.h>
#include <qmmp/qmmp.h>
#include "lxdesupport.h"
#include "qmmpstarter.h"

int main(int argc, char *argv[])
{
    QApplication a (argc, argv );
    a.setApplicationName("qmmp");
    a.setWindowIcon(QIcon(":/32x32/qmmp.png"));

    LXDESupport::load(); //load lxde icons

    QTranslator translator;
    QString locale = Qmmp::systemLanguageID();
    translator.load(QString(":/qmmp_") + locale);
    a.installTranslator(&translator);

    QTranslator qt_translator;
    qt_translator.load(QLibraryInfo::location (QLibraryInfo::TranslationsPath) + "/qt_" + locale);
    a.installTranslator(&qt_translator);

    QMMPStarter starter(argc,argv);
    Q_UNUSED(starter)

    a.setQuitOnLastWindowClosed(false);
    return a.exec();
}
