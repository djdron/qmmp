/***************************************************************************
 *   Copyright (C) 2007 by Zhuravlev Uriy                                  *
 *   stalkerg@gmail.com                                                    *
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

#include "outputoss.h"
#include "outputossfactory.h"


const QString& OutputOSSFactory::name() const
{
    static QString name(tr("OSS Plugin"));
    return name;
}

Output* OutputOSSFactory::create(QObject* parent)
{
    return new OutputOSS(parent);
}

void OutputOSSFactory::showSettings(QWidget*)
{
}

void OutputOSSFactory::showAbout(QWidget *parent)
{
QMessageBox::about (parent, tr("About OSS Output Plugin"),
                        tr("Qmmp OSS Output Plugin")+"\n"+
                        tr("Writen by: Yuriy Zhuravlev <slalkerg@gmail.com>")+"\n"+
                        tr("Based on code by:Brad Hughes <bhughes@trolltech.com>"));
}

QTranslator *OutputOSSFactory::createTranslator(QObject *parent)
{
    QTranslator *translator = new QTranslator(parent);
    QString locale = QLocale::system().name();
    translator->load(QString(":/oss_plugin_") + locale);
    return translator;
}

Q_EXPORT_PLUGIN(OutputOSSFactory)
