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
#include <QFile>

#include "aacfile.h"
#include "detailsdialog.h"
#include "decoder_aac.h"
#include "decoderaacfactory.h"


// DecoderAACFactory

bool DecoderAACFactory::supports(const QString &source) const
{

    return (source.right(4).toLower() == ".aac")
           || (source.right(4).toLower() == ".m4a");
}

bool DecoderAACFactory::canDecode(QIODevice *) const
{
    return FALSE;
}

const DecoderProperties DecoderAACFactory::properties() const
{
    DecoderProperties properties;
    properties.name = tr("AAC Plugin");
    properties.filter = "*.aac";
    properties.description = tr("AAC Files");
    //properties.contentType = ;
    properties.hasAbout = TRUE;
    properties.hasSettings = FALSE;
    return properties;
}

Decoder *DecoderAACFactory::create(QObject *parent, QIODevice *input,
                                   Output *output, const QString &)
{
    return new DecoderAAC(parent, this, input, output);
}

QList<FileInfo *> DecoderAACFactory::createPlayList(const QString &fileName)
{
    FileInfo *info = new FileInfo(fileName);

    /*TagLib::FileRef fileRef(fileName.toLocal8Bit ());
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
        info->setLength(fileRef.audioProperties()->length());*/
    QFile file(fileName);
    if(file.open(QIODevice::ReadOnly))
    {
        AACFile aac_file(&file);
        info->setLength(aac_file.length());
    }
    QList <FileInfo*> list;
    list << info;
    return list;
}

QObject* DecoderAACFactory::showDetails(QWidget *parent, const QString &path)
{
    /*DetailsDialog *d = new DetailsDialog(parent, path);
    d -> show();*/
    return 0;
}

void DecoderAACFactory::showSettings(QWidget *)
{}

void DecoderAACFactory::showAbout(QWidget *parent)
{
    QMessageBox::about (parent, tr("About AAC Audio Plugin"),
                        tr("Qmmp AAC Audio Plugin")+"\n"+
                        tr("Writen by: Ilya Kotov <forkotov02@hotmail.ru>"));
}

QTranslator *DecoderAACFactory::createTranslator(QObject *parent)
{
    QTranslator *translator = new QTranslator(parent);
    QString locale = QLocale::system().name();
    translator->load(QString(":/aac_plugin_") + locale);
    return translator;
}

Q_EXPORT_PLUGIN(DecoderAACFactory)
