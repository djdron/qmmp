/***************************************************************************
 *   Copyright (C) 2010 by Ilya Kotov                                      *
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
#include <gme/Gme_File.h>
#include "gmehelper.h"
#include "decoder_gme.h"
#include "decodergmefactory.h"

// DecoderGmeFactory

bool DecoderGmeFactory::supports(const QString &source) const
{
    QString lExt = source.section(".",-1).toLower();
    lExt.prepend(".");
    QStringList lExtList;
    lExtList << ".ay" << ".gms" << ".gym" << ".hes" << ".kss" << ".nsf" << ".nsfe" << ".sap" << ".spc"
             << ".vgm" << ".vgz";
    if (lExtList.contains(lExt))
        return true;
    return false;
}

bool DecoderGmeFactory::canDecode(QIODevice *) const
{
    return false;
}

const DecoderProperties DecoderGmeFactory::properties() const
{
    DecoderProperties properties;
    properties.name = tr("GME Plugin");
    properties.filter = QString("*.ay *.gms *.gym *.hes *.kss *.nsf *.nsfe *.sap *.spc *.vgm *.vgz");
    properties.description = tr("Game Music Files");
    //properties.contentType = ;
    properties.shortName = "gme";
    properties.hasAbout = true;
    properties.hasSettings = true;
    properties.noInput = true;
    properties.protocols = "gme";
    return properties;
}

Decoder *DecoderGmeFactory::create(const QString &path, QIODevice *input)
{
    Q_UNUSED(input);
    return new DecoderGme(path);
}

QList<FileInfo *> DecoderGmeFactory::createPlayList(const QString &fileName, bool useMetaData)
{
    QList <FileInfo*> list;
    GmeHelper helper;
    Music_Emu *emu = helper.load(fileName);
    if(!emu)
    {
        qWarning("DecoderGmeFactory: unable to open file");
        return list;
    }
    list = helper.createPlayList(useMetaData);
    return list;
}

MetaDataModel* DecoderGmeFactory::createMetaDataModel(const QString &path, QObject *parent)
{
    Q_UNUSED(path);
    Q_UNUSED(parent);
    return 0;
}

void DecoderGmeFactory::showSettings(QWidget *parent)
{
    Q_UNUSED(parent);
}

void DecoderGmeFactory::showAbout(QWidget *parent)
{
    QMessageBox::about (parent, tr("About GME Audio Plugin"),
                        tr("Qmmp GME Audio Plugin")+"\n"+
                        tr("This plugin uses Game_Music_Emu library to play game music files")+"\n"+
                        tr("Writen by: Ilya Kotov <forkotov02@hotmail.ru>"));
}

QTranslator *DecoderGmeFactory::createTranslator(QObject *parent)
{
    QTranslator *translator = new QTranslator(parent);
    QString locale = QLocale::system().name();
    translator->load(QString(":/gme_plugin_") + locale);
    return translator;
}
Q_EXPORT_PLUGIN2(gme,DecoderGmeFactory)
