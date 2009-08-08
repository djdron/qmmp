/***************************************************************************
 *   Copyright (C) 2006-2009 by Ilya Kotov                                 *
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
extern "C"{
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

#include <QFile>
#include <QFileInfo>

#include "detailsdialog.h"

DetailsDialog::DetailsDialog(QWidget *parent, const QString &path)
        : AbstractDetailsDialog(parent)
{
    m_path = path;
    if (QFile::exists(m_path))
        loadInfo();
    hideSaveButton();
}

DetailsDialog::~DetailsDialog()
{}

void DetailsDialog::loadInfo()
{
    AVFormatContext *in;
    avcodec_init();
    avcodec_register_all();
    av_register_all();
    if (av_open_input_file(&in, m_path.toLocal8Bit(), NULL,0, NULL) < 0)
        return;
    av_find_stream_info(in);
    av_read_play(in);

    //tags
    setMetaData(Qmmp::TITLE, QString::fromUtf8(in->title).trimmed());
    setMetaData(Qmmp::ARTIST, QString::fromUtf8(in->author).trimmed());
    setMetaData(Qmmp::ALBUM, QString::fromUtf8(in->album).trimmed());
    setMetaData(Qmmp::COMMENT, QString::fromUtf8(in->comment).trimmed());
    setMetaData(Qmmp::GENRE, QString::fromUtf8(in->genre).trimmed());
    setMetaData(Qmmp::URL, m_path);
    setMetaData(Qmmp::YEAR, in->year);
    setMetaData(Qmmp::TRACK, in->track);

    //audio properties
    QMap <QString, QString> ap;
    QString text = QString("%1").arg(int(in->duration/AV_TIME_BASE)/60);
    text +=":"+QString("%1").arg(int(in->duration/AV_TIME_BASE)%60,2,10,QChar('0'));
    ap.insert(tr("Length"), text);
    ap.insert(tr("File size"),  QString("%1 ").arg(in->file_size/1024)+" "+tr("KB"));
    ap.insert(tr("Bitrate"), QString("%1 "+tr("kbps")).arg(in->bit_rate/1000));

    AVCodecContext *c = 0;
    uint wma_idx;
    for (wma_idx = 0; wma_idx < in->nb_streams; wma_idx++)
    {
        c = in->streams[wma_idx]->codec;
        if (c->codec_type == CODEC_TYPE_AUDIO) break;
    }
    if (c)
    {
        ap.insert(tr("Samplerate"), QString("%1 " + tr("Hz")).arg(c->sample_rate));
        ap.insert(tr("Channels"), QString("%1").arg(c->channels));
    }
    av_close_input_file(in);
    setAudioProperties(ap);
}
