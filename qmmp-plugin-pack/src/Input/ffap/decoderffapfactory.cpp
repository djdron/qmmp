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

#include <QtGui>
#include <taglib/apefile.h>
#include <taglib/apetag.h>
#include "ffapmetadatamodel.h"
#include "decoder_ffap.h"
#include "decoderffapfactory.h"
#include "cueparser.h"

// DecoderFFapFactory

bool DecoderFFapFactory::supports(const QString &source) const
{

    return (source.right(4).toLower() == ".ape");
}

bool DecoderFFapFactory::canDecode(QIODevice *input) const
{
    /*char buf[4];
    return (input->peek(buf, 4) == 4 && !memcmp(buf, "wvpk", 4));*/
    Q_UNUSED(input);
    return false;
}

const DecoderProperties DecoderFFapFactory::properties() const
{
    DecoderProperties properties;
    properties.name = tr("FFap Plugin");
    properties.filters << "*.ape";
    properties.description = tr("Monkey's Audio Files");
    //properties.contentType = ;
    properties.shortName = "ffap";
    properties.hasAbout = true;
    properties.hasSettings = false;
    properties.noInput = false;
    //properties.protocols << "ape";
    properties.priority = 10;
    return properties;
}

Decoder *DecoderFFapFactory::create(const QString &p, QIODevice *i)
{
    return new DecoderFFap(p, i);
}

QList<FileInfo *> DecoderFFapFactory::createPlayList(const QString &fileName, bool useMetaData)
{
    QList <FileInfo*> list;
    TagLib::APE::Tag *tag = 0;
    TagLib::APE::File *file = 0;
    TagLib::APE::Properties *ap = 0;

    //extract metadata of one cue track
    if(fileName.contains("://"))
    {
        QString path = QUrl(fileName).path();
        path.replace(QString(QUrl::toPercentEncoding("#")), "#");
        path.replace(QString(QUrl::toPercentEncoding("?")), "?");
        path.replace(QString(QUrl::toPercentEncoding("%")), "%");
        path.replace(QString(QUrl::toPercentEncoding(":")), ":");
        int track = fileName.section("#", -1).toInt();
        list = createPlayList(path, true);
        if (list.isEmpty() || track <= 0 || track > list.count())
        {
            qDeleteAll(list);
            list.clear();
            return list;
        }
        FileInfo *info = list.takeAt(track - 1);
        qDeleteAll(list);
        return QList<FileInfo *>() << info;
    }

    file = new TagLib::APE::File(fileName.toLocal8Bit().constData());
    tag = useMetaData ? file->APETag() : 0;
    ap = file->audioProperties();

    FileInfo *info = new FileInfo(fileName);

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

        if (tag->itemListMap().contains("CUESHEET"))
        {
            CUEParser parser(tag->itemListMap()["CUESHEET"].toString().toCString(true), fileName);
            list = parser.createPlayList();
            delete info;
            delete file;
            return list;
        }

        //additional metadata
        TagLib::APE::Item fld;
        if(!(fld = tag->itemListMap()["COMPOSER"]).isEmpty())
            info->setMetaData(Qmmp::COMPOSER, QString::fromUtf8(fld.toString().toCString(true)).trimmed());
    }
    if(ap)
        info->setLength(ap->length());
    list << info;
    delete file;
    return list;
}

MetaDataModel* DecoderFFapFactory::createMetaDataModel(const QString &path, QObject *parent)
{
    return new FFapMetaDataModel(path, parent);
}

void DecoderFFapFactory::showSettings(QWidget *)
{}

void DecoderFFapFactory::showAbout(QWidget *parent)
{
    Q_UNUSED(parent);
    /*QMessageBox::about (parent, tr("About FFap Audio Plugin"),
                        tr("Qmmp FFap Audio Plugin")+"\n"+
                        tr("FFap library version:") +
                        QString(" %1").arg(FFapGetLibraryVersionString ())+"\n"+
                        tr("Written by: Ilya Kotov <forkotov02@hotmail.ru>"));*/
}

QTranslator *DecoderFFapFactory::createTranslator(QObject *parent)
{
    QTranslator *translator = new QTranslator(parent);
    QString locale = Qmmp::systemLanguageID();
    translator->load(QString(":/ffap_plugin_") + locale);
    return translator;
}

Q_EXPORT_PLUGIN2(ffap,DecoderFFapFactory)
