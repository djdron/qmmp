/***************************************************************************
 *   Copyright (C) 2006 by Ilya Kotov                                      *
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

#include "detailsdialog.h"

DetailsDialog::DetailsDialog(QWidget *parent, const QString &path)
        : QDialog(parent)
{
    ui.setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    m_path = path;
    setWindowTitle (path.section('/',-1));
    path.section('/',-1);

    ui.pathLineEdit->setText(m_path);
    if(QFile::exists(m_path))
        loadInfo();
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
    QString string = QString::fromUtf8(in->title).trimmed();
    ui.titleLineEdit->setText(string);
    string = QString::fromUtf8(in->author).trimmed();
    ui.artistLineEdit->setText(string);
    string = QString::fromUtf8(in->album).trimmed();
    ui.albumLineEdit->setText(string);
    string = QString::fromUtf8(in->comment).trimmed();
    ui.commentLineEdit->setText(string);
    string = QString("%1").arg(in->year);
    ui.yearLineEdit->setText(string);
    string = QString("%1").arg(in->track);
    ui.trackLineEdit->setText(string);
    string = QString::fromUtf8(in->genre).trimmed();
    ui.genreLineEdit->setText(string);

    QString text;
    text = QString("%1").arg(int(in->duration/AV_TIME_BASE)/60);
    text +=":"+QString("%1").arg(int(in->duration/AV_TIME_BASE)%60,2,10,QChar('0'));
    ui.lengthLabel->setText(text);


    text = QString("%1").arg(in->file_size/1024)+" "+tr("KB");
    ui.fileSizeLabel->setText(text);
    text = QString("%1").arg(in->bit_rate/1000);
    ui.bitrateLabel->setText(text+" "+tr("kbps"));

    AVCodecContext *c = 0;
    uint wma_idx;

    for (wma_idx = 0; wma_idx < in->nb_streams; wma_idx++)
    {
        c = in->streams[wma_idx]->codec;
        if (c->codec_type == CODEC_TYPE_AUDIO) break;
    }

    if (c)
    {
        text = QString("%1").arg(c->sample_rate);
        ui.sampleRateLabel->setText(text+" "+tr("Hz"));
        text = QString("%1").arg(c->channels);
        ui.channelsLabel->setText(text);
    }

    av_close_input_file(in);
}


