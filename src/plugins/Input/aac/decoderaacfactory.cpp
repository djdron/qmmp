/***************************************************************************
 *   Copyright (C) 2008-2011 by Ilya Kotov                                 *
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
#include "aacmetadatamodel.h"
#include "decoder_aac.h"
#include "decoderaacfactory.h"


// DecoderAACFactory

bool DecoderAACFactory::supports(const QString &source) const
{
    return (source.right(4).toLower() == ".aac");
}

bool DecoderAACFactory::canDecode(QIODevice *input) const
{
    uchar buf[4096];
    qint64 buf_at = input->peek((char *) buf, 4096);

    int tag_size = 0;
    if (!memcmp(buf, "ID3", 3)) //skip ID3 tag
    {
        /* high bit is not used */
        tag_size = (buf[6] << 21) | (buf[7] << 14) |
                   (buf[8] <<  7) | (buf[9] <<  0);

        tag_size += 10;
        if (buf_at - tag_size < 4)
            return false;

        memmove (buf, buf + tag_size, buf_at - tag_size);
    }
    //try to determnate header type;
    if (buf[0] == 0xff && ((buf[1] & 0xf6) == 0xf0)) //ADTS header
        return true;
    else if (!memcmp(buf, "ADIF", 4)) //ADIF header
        return true;
    return false;
}

const DecoderProperties DecoderAACFactory::properties() const
{
    DecoderProperties properties;
    properties.name = tr("AAC Plugin");
    properties.filters << "*.aac";
    properties.description = tr("AAC Files");
    //properties.contentType = ;
    properties.shortName = "aac";
    properties.hasAbout = true;
    properties.hasSettings = false;
    return properties;
}

Decoder *DecoderAACFactory::create(const QString &, QIODevice *input)
{
    return new DecoderAAC(input);
}

QList<FileInfo *> DecoderAACFactory::createPlayList(const QString &fileName, bool useMetaData)
{
    FileInfo *info = new FileInfo(fileName);

    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly))
    {
        AACFile aac_file(&file, useMetaData);
        if (useMetaData)
            info->setMetaData(aac_file.metaData());
        info->setLength(aac_file.length());
    }
    QList <FileInfo*> list;
    list << info;
    return list;
}

MetaDataModel* DecoderAACFactory::createMetaDataModel(const QString &path, QObject *parent)
{
    return new AACMetaDataModel(path, parent);
}

void DecoderAACFactory::showSettings(QWidget *)
{}

void DecoderAACFactory::showAbout(QWidget *parent)
{
    QMessageBox::about (parent, tr("About AAC Audio Plugin"),
                        tr("Qmmp AAC Audio Plugin")+"\n"+
                        tr("Written by: Ilya Kotov <forkotov02@hotmail.ru>"));
}

QTranslator *DecoderAACFactory::createTranslator(QObject *parent)
{
    QTranslator *translator = new QTranslator(parent);
    QString locale = Qmmp::systemLanguageID();
    translator->load(QString(":/aac_plugin_") + locale);
    return translator;
}

Q_EXPORT_PLUGIN2(aac,DecoderAACFactory)
