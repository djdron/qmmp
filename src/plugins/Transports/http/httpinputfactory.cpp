/***************************************************************************
 *   Copyright (C) 2009-2010 by Ilya Kotov                                 *
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
#include <QTranslator>
#include <curl/curlver.h>
#include <qmmp/qmmp.h>
#include "settingsdialog.h"
#include "httpinputsource.h"
#include "httpinputfactory.h"

const InputSourceProperties HTTPInputFactory::properties() const
{
    InputSourceProperties p;
    p.protocols = "http";
    p.name = tr("HTTP Plugin");
    p.shortName = "http";
    p.hasAbout = TRUE;
    p.hasSettings = TRUE;
    return p;
}

InputSource *HTTPInputFactory::create(const QString &url, QObject *parent)
{
    return new HTTPInputSource(url, parent);
}

void HTTPInputFactory::showSettings(QWidget *parent)
{
    SettingsDialog *s = new SettingsDialog(parent);
    s->show();
}

void HTTPInputFactory::showAbout(QWidget *parent)
{
    QMessageBox::about (parent, tr("About HTTP Transport Plugin"),
                        tr("Qmmp HTTP Transport Plugin")+"\n"+
                        tr("Compiled against libcurl-%1").arg(LIBCURL_VERSION) + "\n" +
                        tr("Writen by: Ilya Kotov <forkotov02@hotmail.ru>"));
}

QTranslator *HTTPInputFactory::createTranslator(QObject *parent)
{
    QTranslator *translator = new QTranslator(parent);
    QString locale = Qmmp::systemLanguageID();
    translator->load(QString(":/http_plugin_") + locale);
    return translator;
}
Q_EXPORT_PLUGIN2(http, HTTPInputFactory);
