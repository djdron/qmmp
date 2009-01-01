/***************************************************************************
 *   Copyright (C) 2009 by Ilya Kotov                                      *
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

#include <QProcess>
#include <QRegExp>
#include <QFileInfo>

#include "detailsdialog.h"

DetailsDialog::DetailsDialog(const QString &path, QWidget *parent)
        : QDialog(parent)
{
    ui.setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    ui.pathLineEdit->setText(path);
    ui.sizeLabel->setText(QString("%1 ").arg(QFileInfo(path).size ()/1024)+tr("KB"));
    setWindowTitle(QFileInfo(path).fileName());
    //regular expressions
    QRegExp rx_id_length("^ID_LENGTH=([0-9,.]+)*");
    QRegExp rx_id_demuxer("^ID_DEMUXER=(.*)");
    QRegExp rx_id_video_bitrate("^ID_VIDEO_BITRATE=([0-9,.]+)*");
    QRegExp rx_id_width("^ID_VIDEO_WIDTH=([0-9,.]+)*");
    QRegExp rx_id_height("^ID_VIDEO_HEIGHT=([0-9,.]+)*");
    QRegExp rx_id_video_format("^ID_VIDEO_FORMAT=(.*)");
    QRegExp rx_id_video_fps("^ID_VIDEO_FPS=([0-9,.]+)*");
    QRegExp rx_id_video_codec("^ID_VIDEO_CODEC=(.*)");
    QRegExp rx_id_video_aspect("^ID_VIDEO_ASPECT=([0-9,.]+)*");
    QRegExp rx_id_audio_bitrate("^ID_AUDIO_BITRATE=([0-9,.]+)*");
    QRegExp rx_id_audio_rate("^ID_AUDIO_RATE=([0-9,.]+)*");
    QRegExp rx_id_audio_nch("^ID_AUDIO_NCH=([0-9,.]+)*");
    QRegExp rx_id_audio_codec("^ID_AUDIO_CODEC=(.*)");
    //prepare and start mplayer process
    QStringList args;
    args << "-slave";
    args << "-identify";
    args << "-frames";
    args << "0";
    args << "-vo";
    args << "null";
    args << "-ao";
    args << "null";
    args << path;
    QProcess mplayer_process;
    mplayer_process.start("mplayer", args);
    mplayer_process.waitForFinished();
    QString str = QString::fromLocal8Bit(mplayer_process.readAll()).trimmed();
    QStringList lines = str.split("\n");
    int height = 0, width = 0;
    //mplayer std output parsing
    foreach(QString line, lines)
    {
        //general info
        if (rx_id_length.indexIn(line) > -1)
            ui.lengthLabel->setText(rx_id_length.cap(1)); //TODO use hh:mm:ss format
        else if (rx_id_demuxer.indexIn(line) > -1)
            ui.demuxerLabel->setText(rx_id_demuxer.cap(1));
        //video info
        else if (rx_id_video_format.indexIn(line) > -1)
            ui.videoFormatLabel->setText(rx_id_video_format.cap(1));
        else if (rx_id_video_fps.indexIn(line) > -1)
            ui.fpsLabel->setText(rx_id_video_fps.cap(1));
        else if (rx_id_video_codec.indexIn(line) > -1)
            ui.videoCodecLabel->setText(rx_id_video_codec.cap(1));
        else if (rx_id_video_aspect.indexIn(line) > -1)
            ui. ratioLabel->setText(rx_id_video_aspect.cap(1));
        else if (rx_id_video_bitrate.indexIn(line) > -1)
            ui.videoBitrateLabel->setText(rx_id_video_bitrate.cap(1));
        else if (rx_id_width.indexIn(line) > -1)
            width = rx_id_width.cap(1).toInt();
        else if (rx_id_height.indexIn(line) > -1)
            height = rx_id_height.cap(1).toInt();
        //audio info
        else if (rx_id_audio_codec.indexIn(line) > -1)
            ui.audioCodecLabel->setText(rx_id_audio_codec.cap(1));
        else if (rx_id_audio_rate.indexIn(line) > -1)
            ui.sampleRateLabel->setText(rx_id_audio_rate.cap(1));
        else if (rx_id_audio_bitrate.indexIn(line) > -1)
            ui.audioBitrateLabel->setText(rx_id_audio_bitrate.cap(1));
        else if (rx_id_audio_nch.indexIn(line) > -1)
            ui.channelsLabel->setText(rx_id_audio_nch.cap(1));
    }
    ui.resolutionLabel->setText(QString("%1x%2").arg(width).arg(height));
}


DetailsDialog::~DetailsDialog()
{
}


