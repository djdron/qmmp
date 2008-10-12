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

extern "C"{
#include <wavpack/wavpack.h>
}

#include "detailsdialog.h"
#include "decoder_wavpack.h"
#include "decoderwavpackfactory.h"


// DecoderWavPackFactory

bool DecoderWavPackFactory::supports(const QString &source) const
{

    return (source.right(3).toLower() == ".wv");
}

bool DecoderWavPackFactory::canDecode(QIODevice *) const
{
    return FALSE;
}

const DecoderProperties DecoderWavPackFactory::properties() const
{
    DecoderProperties properties;
    properties.name = tr("WavPack Plugin");
    properties.filter = "*.wv";
    properties.description = tr("WavPack Files");
    //properties.contentType = ;
    properties.hasAbout = TRUE;
    properties.hasSettings = FALSE;
    properties.noInput = TRUE;
    properties.protocols = "file";
    return properties;
}

Decoder *DecoderWavPackFactory::create(QObject *parent, QIODevice *input,
                                   Output *output, const QString &path)
{
    Q_UNUSED(input);
    return new DecoderWavPack(parent, this, output, path);
}

QList<FileInfo *> DecoderWavPackFactory::createPlayList(const QString &fileName)
{
    QList <FileInfo*> list;
    char err[80];
    WavpackContext *ctx = WavpackOpenFileInput (fileName.toLocal8Bit(), err,
                                      OPEN_WVC | OPEN_TAGS, 0);
    if(!ctx)
    {
        qWarning("DecoderWavPackFactory: error: %s", err);
        return list;
    }
    FileInfo *info = new FileInfo(fileName);
    char value[200];
    WavpackGetTagItem (ctx, "Album", value, sizeof(value));
    info->setMetaData(Qmmp::ALBUM, QString::fromUtf8(value));
    WavpackGetTagItem (ctx, "Artist", value, sizeof(value));
    info->setMetaData(Qmmp::ARTIST, QString::fromUtf8(value));
    WavpackGetTagItem (ctx, "Comment", value, sizeof(value));
    info->setMetaData(Qmmp::COMMENT, QString::fromUtf8(value));
    WavpackGetTagItem (ctx, "Genre", value, sizeof(value));
    info->setMetaData(Qmmp::GENRE, QString::fromUtf8(value));
    WavpackGetTagItem (ctx, "Title", value, sizeof(value));
    info->setMetaData(Qmmp::TITLE, QString::fromUtf8(value));
    WavpackGetTagItem (ctx, "Year", value, sizeof(value));
    info->setMetaData(Qmmp::YEAR, QString::fromUtf8(value).toInt());
    WavpackGetTagItem (ctx, "Track", value, sizeof(value));
    info->setMetaData(Qmmp::TRACK, QString::fromUtf8(value).toInt());
    info->setLength((int) WavpackGetNumSamples(ctx)/WavpackGetSampleRate(ctx));
    WavpackCloseFile (ctx);
    list << info;
    return list;
}

QObject* DecoderWavPackFactory::showDetails(QWidget *parent, const QString &path)
{
    DetailsDialog *d = new DetailsDialog(parent, path);
    d -> show();
    return d;
}

void DecoderWavPackFactory::showSettings(QWidget *)
{}

void DecoderWavPackFactory::showAbout(QWidget *parent)
{
    QMessageBox::about (parent, tr("About WavPack Audio Plugin"),
                        tr("Qmmp WavPack Audio Plugin")+"\n"+
                        tr("WavPack library version:") +
                        QString(" %1").arg(WavpackGetLibraryVersionString ())+"\n"+
                        tr("Writen by: Ilya Kotov <forkotov02@hotmail.ru>"));
}

QTranslator *DecoderWavPackFactory::createTranslator(QObject *parent)
{
    QTranslator *translator = new QTranslator(parent);
    QString locale = QLocale::system().name();
    translator->load(QString(":/wavpack_plugin_") + locale);
    return translator;
}

Q_EXPORT_PLUGIN(DecoderWavPackFactory)
