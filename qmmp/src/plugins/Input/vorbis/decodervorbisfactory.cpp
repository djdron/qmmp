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
#include <taglib/vorbisfile.h>
#include "replaygainreader.h"
#include "decoder_vorbis.h"
#include "vorbismetadatamodel.h"
#include "decodervorbisfactory.h"


// DecoderOggFactory

bool DecoderVorbisFactory::supports(const QString &source) const
{
    return source.right(4).toLower() == ".ogg";
}

bool DecoderVorbisFactory::canDecode(QIODevice *input) const
{
    char buf[36];
    if (input->peek(buf, 36) == 36 && !memcmp(buf, "OggS", 4)
            && !memcmp(buf + 29, "vorbis", 6))
        return true;

    return false;
}

const DecoderProperties DecoderVorbisFactory::properties() const
{
    DecoderProperties properties;
    properties.name = tr("Ogg Vorbis Plugin");
    properties.shortName = "vorbis";
    properties.filters << "*.ogg";
    properties.description = tr("Ogg Vorbis Files");
    properties.contentTypes << "application/ogg" << "audio/x-vorbis+ogg";
    properties.hasAbout = true;
    properties.hasSettings = false;
    properties.noInput = false;
    return properties;
}

Decoder *DecoderVorbisFactory::create(const QString &url, QIODevice *input)
{
    Decoder *d = new DecoderVorbis(url, input);
    if(!url.contains("://")) //local file
    {
        ReplayGainReader rg(url);
        d->setReplayGainInfo(rg.replayGainInfo());
    }
    return d;
}

MetaDataModel* DecoderVorbisFactory::createMetaDataModel(const QString &path, QObject *parent)
{
    return new VorbisMetaDataModel(path, parent);
}

QList<FileInfo *> DecoderVorbisFactory::createPlayList(const QString &fileName, bool useMetaData)
{
    FileInfo *info = new FileInfo(fileName);

    TagLib::Ogg::Vorbis::File fileRef(fileName.toLocal8Bit ());
    TagLib::Ogg::XiphComment *tag = useMetaData ? fileRef.tag() : 0;

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
        TagLib::StringList fld;
        if(!(fld = tag->fieldListMap()["COMPOSER"]).isEmpty())
            info->setMetaData(Qmmp::COMPOSER,
                              QString::fromUtf8(fld.toString().toCString(true)).trimmed());
        if(!(fld = tag->fieldListMap()["DISCNUMBER"]).isEmpty())
            info->setMetaData(Qmmp::DISCNUMBER,
                              QString::fromUtf8(fld.toString().toCString(true)).trimmed());
    }

    QList <FileInfo*> list;
    list << info;
    return list;
}

void DecoderVorbisFactory::showSettings(QWidget *)
{}

void DecoderVorbisFactory::showAbout(QWidget *parent)
{
    QMessageBox::about (parent, tr("About Ogg Vorbis Audio Plugin"),
                        tr("Qmmp Ogg Vorbis Audio Plugin")+"\n"+
                        tr("Writen by: Ilya Kotov <forkotov02@hotmail.ru>")+"\n"+
                        tr("Source code based on mq3 progect")
                       );
}

QTranslator *DecoderVorbisFactory::createTranslator(QObject *parent)
{
    QTranslator *translator = new QTranslator(parent);
    QString locale = Qmmp::systemLanguageID();
    translator->load(QString(":/vorbis_plugin_") + locale);
    return translator;
}

Q_EXPORT_PLUGIN2(vorbis,DecoderVorbisFactory)
