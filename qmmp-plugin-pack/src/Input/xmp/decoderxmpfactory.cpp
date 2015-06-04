/***************************************************************************
 *   Copyright (C) 2015 by Ilya Kotov                                      *
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
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#include <QtGui>
#include <QStringList>
#include <QRegExp>
#include <xmp.h>
#include "decoder_xmp.h"
#include "xmpmetadatamodel.h"
#include "decoderxmpfactory.h"


// DecodeXmpFactory

bool DecoderXmpFactory::supports(const QString &source) const
{
    foreach(QString filter, properties().filters)
    {
        QRegExp regexp(filter, Qt::CaseInsensitive, QRegExp::Wildcard);
        if (regexp.exactMatch(source))
            return true;
    }
    return false;
}

bool DecoderXmpFactory::canDecode(QIODevice *) const
{
    return false;
}

const DecoderProperties DecoderXmpFactory::properties() const
{
    DecoderProperties properties;
    properties.name = tr("XMP Plugin");
    properties.filters << "*.amf" << "*.ams" << "*.dbm" << "*.dbf" << "*.dsm" << "*.far" << "*.mdl";
    properties.filters << "*.stm" << "*.ult" << "*.j2b" << "*.mt2" << "*.mdz" << "*.mdr" << "*.mdgz";
    properties.filters << "*.mdbz" << "*.mod" << "*.s3z" << "*.s3r" << "*.s3gz" << "*.s3m" << "*.xmz";
    properties.filters << "*.xmr" << "*.xmgz" << "*.itz" << "*.itr" << "*.itgz" << "*.dmf" "*.umx";
    properties.filters << "*.it" << "*.669" << "*.xm" << "*.mtm" << "*.psm" << "*.ft2";
    properties.description = tr("Module Files");
    //properties.contentType = ;
    properties.shortName = "xmp";
    properties.hasAbout = true;
    properties.hasSettings = false;
    properties.noInput = true;
    properties.protocols << "file";
    return properties;
}

Decoder *DecoderXmpFactory::create(const QString &path, QIODevice *input)
{
    Q_UNUSED(input);
    return new DecoderXmp(path);
}

QList<FileInfo *> DecoderXmpFactory::createPlayList(const QString &fileName, bool useMetaData, QStringList *)
{
    QList <FileInfo*> list;
    FileInfo *info = new FileInfo(fileName);
    if(useMetaData)
    {
        xmp_context ctx = xmp_create_context();
        if(xmp_load_module(ctx, fileName.toLocal8Bit().data()) != 0)
        {
            qWarning("DecoderXmpFactory: unable to load module");
            xmp_free_context(ctx);
            delete info;
            return list;
        }
        xmp_module_info mi;
        xmp_get_module_info(ctx, &mi);
        info->setMetaData(Qmmp::TITLE, mi.mod->name);
        info->setLength(mi.seq_data[0].duration / 1000);
        xmp_release_module(ctx);
        xmp_free_context(ctx);
    }
    list << info;
    return list;
}

MetaDataModel* DecoderXmpFactory::createMetaDataModel(const QString &path, QObject *parent)
{
    return new XmpMetaDataModel(path, parent);
}

void DecoderXmpFactory::showSettings(QWidget *parent)
{
    //SettingsDialog *d = new SettingsDialog(parent);
    //d->show();
}

void DecoderXmpFactory::showAbout(QWidget *parent)
{
    QMessageBox::about (parent, tr("About XMP Audio Plugin"),
                        tr("Qmmp XMP Audio Plugin")+"\n"+
                        tr("Written by: Ilya Kotov <forkotov02@hotmail.ru>")+"\n"+
                        tr("Compiled against libxmp-%1").arg(XMP_VERSION));
}

QTranslator *DecoderXmpFactory::createTranslator(QObject *parent)
{
    QTranslator *translator = new QTranslator(parent);
    QString locale = Qmmp::systemLanguageID();
    translator->load(QString(":/xmp_plugin_") + locale);
    return translator;
}

Q_EXPORT_PLUGIN2(xmp,DecoderXmpFactory)
