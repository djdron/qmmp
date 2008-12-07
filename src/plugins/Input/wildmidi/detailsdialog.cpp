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

#include <QFile>
#include <QFileInfo>

extern "C"
{
#include <wavpack/wavpack.h>
}

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
    if (QFile::exists(m_path))
    {
        loadWavPackInfo();
        loadTag();
    }
}


DetailsDialog::~DetailsDialog()
{}

void DetailsDialog::loadWavPackInfo()
{
    char err[80];
    WavpackContext *ctx = WavpackOpenFileInput (m_path.toLocal8Bit(), err,
                          OPEN_WVC | OPEN_TAGS, 0);
    if (!ctx)
    {
        qWarning("DetailsDialog: error: %s", err);
        return;
    }

    QString text;
    int length = (int) WavpackGetNumSamples(ctx)/WavpackGetSampleRate(ctx);
    text = QString("%1").arg(length/60);
    text +=":"+QString("%1").arg(length % 60, 2, 10, QChar('0'));
    ui.lengthLabel->setText(text);
    text = QString("%1").arg((int) WavpackGetSampleRate(ctx));
    ui.sampleRateLabel->setText(text+" "+tr("Hz"));
    text = QString("%1").arg((int) WavpackGetNumChannels(ctx));
    ui.channelsLabel->setText(text);
    text = QString("%1")
           .arg((int) WavpackGetAverageBitrate(ctx, WavpackGetNumChannels(ctx))/1000);
    ui.bitrateLabel->setText(text+" "+tr("kbps"));
    QFileInfo info(m_path);
    text = QString("%1 "+tr("KB")).arg((int) info.size()/1024);
    ui.fileSizeLabel->setText(text);
    ui.ratioLabel->setText(QString("%1").arg(WavpackGetRatio(ctx)));
    ui.versionLabel->setText(QString("%1").arg(WavpackGetVersion(ctx)));
    WavpackCloseFile (ctx);
}

void DetailsDialog::loadTag()
{
    char err[80];
    WavpackContext *ctx = WavpackOpenFileInput (m_path.toLocal8Bit(), err,
                          OPEN_WVC | OPEN_TAGS, 0);
    if (!ctx)
    {
        qWarning("DetailsDialog: error: %s", err);
        return;
    }

    char value[200];
    WavpackGetTagItem (ctx, "Title", value, sizeof(value));
    ui.titleLineEdit->setText(QString::fromUtf8(value));
    WavpackGetTagItem (ctx, "Artist", value, sizeof(value));
    ui.artistLineEdit->setText(QString::fromUtf8(value));
    WavpackGetTagItem (ctx, "Album", value, sizeof(value));
    ui.albumLineEdit->setText(QString::fromUtf8(value));
    WavpackGetTagItem (ctx, "Comment", value, sizeof(value));
    ui.commentLineEdit->setText(QString::fromUtf8(value));
    WavpackGetTagItem (ctx, "Year", value, sizeof(value));
    ui.yearLineEdit->setText(QString::fromUtf8(value));
    WavpackGetTagItem (ctx, "Track", value, sizeof(value));
    ui.trackLineEdit->setText(QString::fromUtf8(value));
    WavpackGetTagItem (ctx, "Genre", value, sizeof(value));
    ui.genreLineEdit->setText(QString::fromUtf8(value));

    QFileInfo info(m_path);
    ui.saveButton->setEnabled(info.isWritable());
    connect(ui.saveButton, SIGNAL(clicked()), SLOT(saveTag()));
    WavpackCloseFile (ctx);
}

void DetailsDialog::saveTag()
{
    char err[80];
    WavpackContext *ctx = WavpackOpenFileInput (m_path.toLocal8Bit(), err,
                          OPEN_WVC | OPEN_EDIT_TAGS, 0);
    if (!ctx)
    {
        qWarning("DetailsDialog: error: %s", err);
        return;
    }
    QByteArray value = ui.titleLineEdit->text().toUtf8();
    WavpackAppendTagItem(ctx, "Title", value, value.size());
    value = ui.artistLineEdit->text().toUtf8();
    WavpackAppendTagItem(ctx, "Artist", value, value.size());
    value = ui.albumLineEdit->text().toUtf8();
    WavpackAppendTagItem(ctx, "Album", value, value.size());
    value = ui.commentLineEdit->text().toUtf8();
    WavpackAppendTagItem(ctx, "Comment", value, value.size());
    value = ui.genreLineEdit->text().toUtf8();
    WavpackAppendTagItem(ctx, "Genre", value, value.size());
    value = ui.yearLineEdit->text().toUtf8();
    WavpackAppendTagItem(ctx, "Year", value, value.size());
    value = ui.trackLineEdit->text().toUtf8();
    WavpackAppendTagItem(ctx, "Track", value, value.size()); 

    WavpackWriteTag (ctx);
    WavpackCloseFile (ctx);
}
