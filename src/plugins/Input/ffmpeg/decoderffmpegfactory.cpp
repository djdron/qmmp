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
#include <QSettings>

extern "C"
{
#if defined HAVE_FFMPEG_AVFORMAT_H
#include <ffmpeg/avformat.h>
#elif defined HAVE_LIBAVFORMAT_AVFORMAT_H
#include <libavformat/avformat.h>
#else
#include <avformat.h>
#endif


#if defined HAVE_FFMPEG_AVCODEC_H
#include <ffmpeg/avcodec.h>
#elif defined HAVE_LIBAVCODEC_AVCODEC_H
#include <libavcodec/avcodec.h>
#else
#include <avcodec.h>
#endif
}

#include "ffmpegmetadatamodel.h"
#include "settingsdialog.h"
#include "decoder_ffmpeg.h"
#include "decoderffmpegfactory.h"


// DecoderFFmpegFactory

bool DecoderFFmpegFactory::supports(const QString &source) const
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    QStringList filters;
    filters << "*.wma" << "*.ape";
    filters = settings.value("FFMPEG/filters", filters).toStringList();
    foreach(QString filter, filters)
    {
        QRegExp regexp(filter, Qt::CaseInsensitive, QRegExp::Wildcard);
        if (regexp.exactMatch(source))
            return TRUE;
    }
    return FALSE;
}

bool DecoderFFmpegFactory::canDecode(QIODevice *) const
{
    return FALSE;
}

const DecoderProperties DecoderFFmpegFactory::properties() const
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    QStringList filters;
#if (LIBAVCODEC_VERSION_INT >= ((51<<16)+(44<<8)+0))
    filters << "*.wma" << "*.ape";
#else
    filters << "*.wma";
#endif
    filters = settings.value("FFMPEG/filters", filters).toStringList();
    DecoderProperties properties;
    properties.name = tr("FFmpeg Plugin");
    properties.filter = filters.join(" ");
    properties.description = tr("FFmpeg Formats");
    if(filters.contains("*.wma"))
        properties.contentType += "audio/x-ms-wma";
    if(filters.contains("*.mp3"))
        properties.contentType += " audio/mpeg";
    properties.shortName = "ffmpeg";
    properties.hasAbout = TRUE;
    properties.hasSettings = TRUE;
    properties.noInput = FALSE;
    //properties.protocols = "file";
    return properties;
}

Decoder *DecoderFFmpegFactory::create(const QString &path, QIODevice *input)
{
    return new DecoderFFmpeg(path, input);
}

QList<FileInfo *> DecoderFFmpegFactory::createPlayList(const QString &fileName, bool useMetaData)
{
    QList <FileInfo*> list;
    avcodec_init();
    avcodec_register_all();
    av_register_all();
    AVFormatContext *in = 0;

    if (av_open_input_file(&in, fileName.toLocal8Bit(), 0, 0, 0) < 0)
    {
        qDebug("DecoderFFmpegFactory: unable to open file");
        return list;
    }
    FileInfo *info = new FileInfo(fileName);
    av_find_stream_info(in);

    if (useMetaData)
    {
        info->setMetaData(Qmmp::ALBUM, QString::fromUtf8(in->album).trimmed());
        info->setMetaData(Qmmp::ARTIST, QString::fromUtf8(in->author).trimmed());
        info->setMetaData(Qmmp::COMMENT, QString::fromUtf8(in->comment).trimmed());
        info->setMetaData(Qmmp::GENRE, QString::fromUtf8(in->genre).trimmed());
        info->setMetaData(Qmmp::TITLE, QString::fromUtf8(in->title).trimmed());
        info->setMetaData(Qmmp::YEAR, in->year);
        info->setMetaData(Qmmp::TRACK, in->track);
    }
    info->setLength(in->duration/AV_TIME_BASE);
    av_close_input_file(in);
    list << info;
    return list;
}

MetaDataModel* DecoderFFmpegFactory::createMetaDataModel(const QString &path, QObject *parent)
{
    return new FFmpegMetaDataModel(path,parent);
}

void DecoderFFmpegFactory::showSettings(QWidget *parent)
{
    SettingsDialog *s = new SettingsDialog(parent);
    s->show();
}

void DecoderFFmpegFactory::showAbout(QWidget *parent)
{
    QMessageBox::about (parent, tr("About FFmpeg Audio Plugin"),

                        tr("Qmmp FFmpeg Audio Plugin")+"\n"+
#if (LIBAVFORMAT_VERSION_INT >= ((52<<16)+(17<<8)+0)) && (LIBAVCODEC_VERSION_INT >= ((51<<16)+(60<<8)+0))
                        QString(tr("Compiled against libavformat-%1.%2.%3 and libavcodec-%4.%5.%6"))
                        .arg(LIBAVFORMAT_VERSION_MAJOR)
                        .arg(LIBAVFORMAT_VERSION_MINOR)
                        .arg(LIBAVFORMAT_VERSION_MICRO)
                        .arg(LIBAVCODEC_VERSION_MAJOR)
                        .arg(LIBAVCODEC_VERSION_MINOR)
                        .arg(LIBAVCODEC_VERSION_MICRO)+"\n"+
#endif
                        tr("Writen by: Ilya Kotov <forkotov02@hotmail.ru>"));
}

QTranslator *DecoderFFmpegFactory::createTranslator(QObject *parent)
{
    QTranslator *translator = new QTranslator(parent);
    QString locale = Qmmp::systemLanguageID();
    translator->load(QString(":/ffmpeg_plugin_") + locale);
    return translator;
}

Q_EXPORT_PLUGIN2(ffmpeg,DecoderFFmpegFactory)
