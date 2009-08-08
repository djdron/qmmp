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
#include <wavpack/wavpack.h>
}

#include <QFile>
#include <QFileInfo>

#include "detailsdialog.h"

#define QStringToTString_qt4(s) TagLib::String(s.toUtf8().constData(), TagLib::String::UTF8)
#define TStringToQString_qt4(s) QString::fromUtf8(s.toCString(TRUE)).trimmed()

DetailsDialog::DetailsDialog(QWidget *parent, const QString &path)
        : AbstractDetailsDialog(parent)
{
    m_path = path;
    if (QFile::exists(m_path))
    {
        loadWavPackInfo();
        loadTags();
        blockSaveButton(!QFileInfo(m_path).isWritable());
    }
    else
        blockSaveButton();
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
    QMap <QString, QString> ap;
    int length = (int) WavpackGetNumSamples(ctx)/WavpackGetSampleRate(ctx);
    QString text = QString("%1").arg(length/60);
    text +=":"+QString("%1").arg(length % 60, 2, 10, QChar('0'));
    ap.insert(tr("Length"), text);
    ap.insert(tr("Sample rate"), QString("%1 " + tr("Hz")).arg((int) WavpackGetSampleRate(ctx)));
    ap.insert(tr("Channels"), QString("%1").arg((int) WavpackGetNumChannels(ctx)));
    ap.insert(tr("Bitrate"), QString("%1 " + tr("kbps"))
           .arg((int) WavpackGetAverageBitrate(ctx, WavpackGetNumChannels(ctx))/1000));
    QFileInfo info(m_path);
    ap.insert(tr("File size"), QString("%1 "+tr("KB")).arg(info.size()/1024));
    ap.insert(tr("Ratio"), QString("%1").arg(WavpackGetRatio(ctx)));
    ap.insert(tr("Version"), QString("%1").arg(WavpackGetVersion(ctx)));
    WavpackCloseFile (ctx);
    setAudioProperties(ap);
}

void DetailsDialog::loadTags()
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
    setMetaData(Qmmp::TITLE, QString::fromUtf8(value));
    WavpackGetTagItem (ctx, "Artist", value, sizeof(value));
    setMetaData(Qmmp::ARTIST, QString::fromUtf8(value));
    WavpackGetTagItem (ctx, "Album", value, sizeof(value));
    setMetaData(Qmmp::ALBUM, QString::fromUtf8(value));
    WavpackGetTagItem (ctx, "Comment", value, sizeof(value));
    setMetaData(Qmmp::COMMENT, QString::fromUtf8(value));
    WavpackGetTagItem (ctx, "Year", value, sizeof(value));
    setMetaData(Qmmp::YEAR, QString::fromUtf8(value));
    WavpackGetTagItem (ctx, "Track", value, sizeof(value));
    setMetaData(Qmmp::TRACK, QString::fromUtf8(value));
    WavpackGetTagItem (ctx, "Disc", value, sizeof(value));
    setMetaData(Qmmp::DISCNUMBER, QString::fromUtf8(value));
    WavpackGetTagItem (ctx, "Genre", value, sizeof(value));
    setMetaData(Qmmp::GENRE, QString::fromUtf8(value));
    WavpackGetTagItem (ctx, "Composer", value, sizeof(value));
    setMetaData(Qmmp::COMPOSER, QString::fromUtf8(value));
    WavpackCloseFile (ctx);
}

void DetailsDialog::writeTags()
{
    char err[80];
    WavpackContext *ctx = WavpackOpenFileInput (m_path.toLocal8Bit(), err,
                          OPEN_WVC | OPEN_EDIT_TAGS, 0);
    if (!ctx)
    {
        qWarning("DetailsDialog: error: %s", err);
        return;
    }
    QByteArray value = strMetaData(Qmmp::TITLE).toUtf8();
    WavpackAppendTagItem(ctx, "Title", value, value.size());
    value = strMetaData(Qmmp::ARTIST).toUtf8();
    WavpackAppendTagItem(ctx, "Artist", value, value.size());
    value = strMetaData(Qmmp::ALBUM).toUtf8();
    WavpackAppendTagItem(ctx, "Album", value, value.size());
    value = strMetaData(Qmmp::COMMENT).toUtf8();
    WavpackAppendTagItem(ctx, "Comment", value, value.size());
    value = strMetaData(Qmmp::GENRE).toUtf8();
    WavpackAppendTagItem(ctx, "Genre", value, value.size());
    value = strMetaData(Qmmp::YEAR).toUtf8();
    WavpackAppendTagItem(ctx, "Year", value, value.size());
    value = strMetaData(Qmmp::TRACK).toUtf8();
    WavpackAppendTagItem(ctx, "Track", value, value.size());
    value = strMetaData(Qmmp::COMPOSER).toUtf8();
    WavpackAppendTagItem(ctx, "Composer", value, value.size());
    value = strMetaData(Qmmp::DISCNUMBER).toUtf8();
    WavpackAppendTagItem(ctx, "Disc", value, value.size());
    WavpackWriteTag (ctx);
    WavpackCloseFile (ctx);
}
