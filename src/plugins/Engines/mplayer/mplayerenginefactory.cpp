/***************************************************************************
 *   Copyright (C) 2008-2009 by Ilya Kotov                                 *
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

#include "mplayermetadatamodel.h"
#include "settingsdialog.h"
#include "mplayerengine.h"
#include "mplayerenginefactory.h"


// MplayerEngineFactory

const EngineProperties MplayerEngineFactory::properties() const
{
    EngineProperties properties;
    properties.name = tr("Mplayer Plugin");
    properties.shortName = "mplayer";
    properties.filter = MplayerInfo::filters().join(" ");
    properties.description = tr("Video Files");
    //properties.contentType = "application/ogg;audio/x-vorbis+ogg";
    properties.protocols = "file";
    properties.hasAbout = TRUE;
    properties.hasSettings = TRUE;
    return properties;
}

bool MplayerEngineFactory::supports(const QString &source) const
{
    QStringList filters = MplayerInfo::filters();
    foreach(QString filter, filters)
    {
        QRegExp regexp(filter, Qt::CaseInsensitive, QRegExp::Wildcard);
        if (regexp.exactMatch(source))
            return TRUE;
    }
    return FALSE;
}

AbstractEngine *MplayerEngineFactory::create(QObject *parent)
{
    return new MplayerEngine(parent);
}

QList<FileInfo *> MplayerEngineFactory::createPlayList(const QString &fileName, bool useMetaData)
{
    Q_UNUSED(useMetaData);
    QList<FileInfo *> info;
    info << MplayerInfo::createFileInfo(fileName);
    return info;
}

MetaDataModel* MplayerEngineFactory::createMetaDataModel(const QString &path, QObject *parent)
{
    return new MplayerMetaDataModel(path, parent);
}

void MplayerEngineFactory::showSettings(QWidget *parent)
{
    SettingsDialog *s = new SettingsDialog(parent);
    s->show();
}

void MplayerEngineFactory::showAbout(QWidget *parent)
{
    QMessageBox::about (parent, tr("About MPlayer Plugin"),
                        tr("Qmmp MPlayer Plugin")+"\n"+
                        tr("This plugin uses MPlayer as backend")+"\n"+
                        tr("Writen by: Ilya Kotov <forkotov02@hotmail.ru>"));
}

QTranslator *MplayerEngineFactory::createTranslator(QObject *parent)
{
    QTranslator *translator = new QTranslator(parent);
    QString locale = Qmmp::systemLanguageID();
    translator->load(QString(":/mplayer_plugin_") + locale);
    return translator;
}

Q_EXPORT_PLUGIN(MplayerEngineFactory)
