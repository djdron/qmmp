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
#include <taglib/tag.h>
#include <taglib/fileref.h>
#include <tag.h>

#include "detailsdialog.h"
#include "decoder_vorbis.h"
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
        return TRUE;

    return FALSE;
}

const DecoderProperties DecoderVorbisFactory::properties() const
{
    DecoderProperties properties;
    properties.name = tr("Ogg Vorbis Plugin");
    properties.shortName = "vorbis";
    properties.filter = "*.ogg";
    properties.description = tr("Ogg Vorbis Files");
    properties.contentType = "application/ogg;audio/x-vorbis+ogg";
    properties.hasAbout = TRUE;
    properties.hasSettings = FALSE;
    return properties;
}

Decoder *DecoderVorbisFactory::create(QObject *parent, QIODevice *input,
                                      Output *output, const QString&)
{
    return new DecoderVorbis(parent, this, input, output);
}

//FileInfo *DecoderVorbisFactory::createFileInfo(const QString &source)
QList<FileInfo *> DecoderVorbisFactory::createPlayList(const QString &fileName)
{
    FileInfo *info = new FileInfo(fileName);

    TagLib::FileRef fileRef(fileName.toLocal8Bit ());
    TagLib::Tag *tag = fileRef.tag();

    if (tag && !tag->isEmpty())
    {
        info->setMetaData(Qmmp::ALBUM,
                          QString::fromUtf8(tag->album().toCString(TRUE)).trimmed());
        info->setMetaData(Qmmp::ARTIST,
                          QString::fromUtf8(tag->artist().toCString(TRUE)).trimmed());
        info->setMetaData(Qmmp::COMMENT,
                          QString::fromUtf8(tag->comment().toCString(TRUE)).trimmed());
        info->setMetaData(Qmmp::GENRE,
                          QString::fromUtf8(tag->genre().toCString(TRUE)).trimmed());
        info->setMetaData(Qmmp::TITLE,
                          QString::fromUtf8(tag->title().toCString(TRUE)).trimmed());
        info->setMetaData(Qmmp::YEAR, tag->year());
        info->setMetaData(Qmmp::TRACK, tag->track());
    }

    if (fileRef.audioProperties())
        info->setLength(fileRef.audioProperties()->length());

    QList <FileInfo*> list;
    list << info;
    return list;
}

QObject* DecoderVorbisFactory::showDetails(QWidget *parent, const QString &path)
{
    DetailsDialog *d = new DetailsDialog(parent, path);
    d -> show();
    return d;
}

void DecoderVorbisFactory::showSettings(QWidget *)
{
    /*SettingsDialog *s = new SettingsDialog(parent);
    s -> show();*/
}

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
    QString locale = QLocale::system().name();
    translator->load(QString(":/vorbis_plugin_") + locale);
    return translator;
}

Q_EXPORT_PLUGIN(DecoderVorbisFactory)
