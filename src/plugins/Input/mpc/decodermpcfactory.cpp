/***************************************************************************
 *   Copyright (C) 2008-2010 by Ilya Kotov                                 *
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
#include <taglib/tag.h>
#include <taglib/fileref.h>
#include <taglib/mpcfile.h>
#include <taglib/apetag.h>
#include "mpcmetadatamodel.h"
#include "decoder_mpc.h"
#include "decodermpcfactory.h"


// DecoderMPCFactory

bool DecoderMPCFactory::supports(const QString &source) const
{

    return (source.right(4).toLower() == ".mpc");
}

bool DecoderMPCFactory::canDecode(QIODevice *) const
{
    return false;
}

const DecoderProperties DecoderMPCFactory::properties() const
{
    DecoderProperties properties;
    properties.name = tr("Musepack Plugin");
    properties.filters << "*.mpc";
    properties.description = tr("Musepack Files");
    //properties.contentType = ;
    properties.shortName = "mpc";
    properties.hasAbout = true;
    properties.hasSettings = false;
    return properties;
}

Decoder *DecoderMPCFactory::create(const QString &, QIODevice *i)
{
    return new DecoderMPC(i);
}

QList<FileInfo *> DecoderMPCFactory::createPlayList(const QString &fileName, bool useMetaData)
{
    FileInfo *info = new FileInfo(fileName);

    TagLib::MPC::File fileRef(fileName.toLocal8Bit ());
    TagLib::APE::Tag *tag = useMetaData ? fileRef.APETag() : 0;
    if (tag && !tag->isEmpty())
    {
        info->setMetaData(Qmmp::ALBUM,
                       QString::fromUtf8(tag->album().toCString(true)).trimmed());
        info->setMetaData(Qmmp::ARTIST,
                       QString::fromUtf8(tag->artist().toCString(true)).trimmed());
        info->setMetaData(Qmmp::COMMENT,
                       QString::fromUtf8(tag->comment().toCString(true)).trimmed());
        info->setMetaData(Qmmp::GENRE,
                       QString::fromUtf8(tag->genre().toCString(true)).trimmed());
        info->setMetaData(Qmmp::TITLE,
                       QString::fromUtf8(tag->title().toCString(true)).trimmed());
        info->setMetaData(Qmmp::YEAR, tag->year());
        info->setMetaData(Qmmp::TRACK, tag->track());
    }
    if (fileRef.audioProperties())
        info->setLength(fileRef.audioProperties()->length());
    //additional metadata
    if(tag)
    {
        TagLib::APE::Item fld;
        if(!(fld = tag->itemListMap()["COMPOSER"]).isEmpty())
            info->setMetaData(Qmmp::COMPOSER,
                              QString::fromUtf8(fld.toString().toCString(true)).trimmed());
    }

    QList <FileInfo*> list;
    list << info;
    return list;
}

MetaDataModel* DecoderMPCFactory::createMetaDataModel(const QString &path, QObject *parent)
{
    return new MPCMetaDataModel(path, parent);
}

void DecoderMPCFactory::showSettings(QWidget *)
{}

void DecoderMPCFactory::showAbout(QWidget *parent)
{
    QMessageBox::about (parent, tr("About Musepack Audio Plugin"),
                        tr("Qmmp Musepack Audio Plugin")+"\n"+
                        tr("Written by: Ilya Kotov <forkotov02@hotmail.ru>"));
}

QTranslator *DecoderMPCFactory::createTranslator(QObject *parent)
{
    QTranslator *translator = new QTranslator(parent);
    QString locale = Qmmp::systemLanguageID();
    translator->load(QString(":/mpc_plugin_") + locale);
    return translator;
}

Q_EXPORT_PLUGIN2(mpc,DecoderMPCFactory)
