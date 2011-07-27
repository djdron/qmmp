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
#include "skinnedfactory.h"

const UiProperties SkinnedFactory::properties() const
{
    UiProperties props;
    props.hasAbout = true;
    props.name = tr("Skinned User Interface");
    props.shortName = "skinned";
    return props;
}

QObject *SkinnedFactory::SkinnedFactory::create()
{
    return new MainWindow();
}

void SkinnedFactory::showAbout(QWidget *parent)
{
    QMessageBox::about (parent, tr("About Qmmp Skinned User Interface"),
                        tr("Qmmp Skinned User Interface")+"\n"+
                        tr("Simple user interface with Winamp-2.x/XMMS skins support")+ "\n" +
                        tr("Written by:\n"
                           "Vladimir Kuznetsov <vovanec@gmail.com>\n"
                           "Ilya Kotov <forkotov02@hotmail.ru>"));

}

QTranslator *SkinnedFactory::createTranslator(QObject *parent)
{
    QTranslator *translator = new QTranslator(parent);
    QString locale = Qmmp::systemLanguageID();
    translator->load(QString(":/skinned_plugin_") + locale);
    return translator;
}

Q_EXPORT_PLUGIN2(skinned, SkinnedFactory)
