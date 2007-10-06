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
#include <QTextCodec>
#include <QSettings>
#include <QDir>
#include <QFile>
#include <QFileInfo>

#include <taglib/tag.h>
#include <taglib/fileref.h>
#include <taglib/id3v1tag.h>
#include <taglib/id3v2tag.h>
#include <taglib/tfile.h>
#include <taglib/mpegfile.h>
#include <taglib/mpegheader.h>
#include <taglib/mpegproperties.h>

#include "detailsdialog.h"

DetailsDialog::DetailsDialog(QWidget *parent, const QString &path)
        : QDialog(parent)
{
    ui.setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    m_path = path;
    setWindowTitle (path.section('/',-1));
    ui.pathLineEdit->setText(m_path);

    if (!QFile::exists(m_path))
        return;

    QSettings settings(QDir::homePath()+"/.qmmp/qmmprc", QSettings::IniFormat);
    settings.beginGroup("MAD");
    m_codec_v1 =
        QTextCodec::codecForName(settings.value("ID3v1_encoding","UTF-8" )
                                 .toByteArray ());
    m_codec_v2 =
        QTextCodec::codecForName(settings.value("ID3v2_encoding","UTF-8" )
                                 .toByteArray ());
    if (!m_codec_v1)
        m_codec_v1 = QTextCodec::codecForName ("UTF-8");
    if (!m_codec_v2)
        m_codec_v2 = QTextCodec::codecForName ("UTF-8");
    settings.endGroup();

    loadMPEGInfo();
    loadID3v1Tag();
    loadID3v2Tag();
    QFileInfo info(m_path);
    ui.saveV1Button->setEnabled(info.isWritable());
    ui.saveV2Button->setEnabled(info.isWritable());
}


DetailsDialog::~DetailsDialog()
{}

void DetailsDialog::loadMPEGInfo()
{
    TagLib::MPEG::File f (m_path.toLocal8Bit());
    //l.label
    //ui. f.audioProperties()->level();
    QString text;
    text = QString("%1").arg(f.audioProperties()->layer());
    ui.levelLabel->setText("MPEG layer "+text); //TODO: add MPEG version
    text = QString("%1").arg(f.audioProperties()->bitrate());
    ui.bitRateLabel->setText(text+" "+tr("kbps"));
    text = QString("%1").arg(f.audioProperties()->sampleRate());
    ui.sampleRateLabel->setText(text+" "+tr("Hz"));
    switch (f.audioProperties()->channelMode())
    {
    case TagLib::MPEG::Header::Stereo:
        ui.modeLabel->setText("Stereo");
        break;
    case TagLib::MPEG::Header::JointStereo:
        ui.modeLabel->setText("Joint stereo");
        break;
    case TagLib::MPEG::Header::DualChannel:
        ui.modeLabel->setText("Dual channel");
        break;
    case TagLib::MPEG::Header::SingleChannel:
        ui.modeLabel->setText("Single channel");
        break;
    }
    text = QString("%1 "+tr("KB")).arg(f.length()/1024);
    ui.fileSizeLabel->setText(text);
    /*if (f.audioProperties()->protectionEnabled())
        ui.errProtectionLabel->setText(tr("Yes"));
    else
        ui.errProtectionLabel->setText(tr("No"));*/
    if (f.audioProperties()->isCopyrighted())
        ui.copyrightLabel->setText(tr("Yes"));
    else
        ui.copyrightLabel->setText(tr("No"));
    if (f.audioProperties()->isOriginal())
        ui.originalLabel->setText(tr("Yes"));
    else
        ui.originalLabel->setText(tr("No"));
}

void DetailsDialog::loadID3v1Tag()
{
    TagLib::MPEG::File f (m_path.toLocal8Bit());

    if (f.ID3v1Tag())
    {
        bool utf = m_codec_v1->name().contains("UTF");
        TagLib::String title = f.ID3v1Tag()->title();
        TagLib::String artist = f.ID3v1Tag()->artist();
        TagLib::String album = f.ID3v1Tag()->album();
        TagLib::String comment = f.ID3v1Tag()->comment();
        TagLib::String genre = f.ID3v1Tag()->genre();
        QString string = m_codec_v1->toUnicode(title.toCString(utf)).trimmed();
        ui.titleLineEdit_v1->setText(string);
        string = m_codec_v1->toUnicode(artist.toCString(utf)).trimmed();
        ui.artistLineEdit_v1->setText(string);
        string = m_codec_v1->toUnicode(album.toCString(utf)).trimmed();
        ui.albumLineEdit_v1->setText(string);
        string = m_codec_v1->toUnicode(comment.toCString(utf)).trimmed();
        ui.commentLineEdit_v1->setText(string);
        string = QString("%1").arg(f.ID3v1Tag()->year());
        ui.yearLineEdit_v1->setText(string);
        string = QString("%1").arg(f.ID3v1Tag()->track());
        ui.trackLineEdit_v1->setText(string);
        string = m_codec_v1->toUnicode(genre.toCString(utf)).trimmed();
        ui.genreLineEdit_v1->setText(string);
    }
    connect(ui.saveV1Button, SIGNAL(clicked()), SLOT(saveID3v1Tag()));
}

void DetailsDialog::loadID3v2Tag()
{
    TagLib::MPEG::File f (m_path.toLocal8Bit());

    if (f.ID3v2Tag())
    {
        bool utf = m_codec_v2->name().contains("UTF");
        TagLib::String title = f.ID3v2Tag()->title();
        TagLib::String artist = f.ID3v2Tag()->artist();
        TagLib::String album = f.ID3v2Tag()->album();
        TagLib::String comment = f.ID3v2Tag()->comment();
        TagLib::String genre = f.ID3v2Tag()->genre();
        QString string = m_codec_v2->toUnicode(title.toCString(utf)).trimmed();
        ui.titleLineEdit_v2->setText(string);
        string = m_codec_v2->toUnicode(artist.toCString(utf)).trimmed();
        ui.artistLineEdit_v2->setText(string);
        string = m_codec_v2->toUnicode(album.toCString(utf)).trimmed();
        ui.albumLineEdit_v2->setText(string);
        string = m_codec_v2->toUnicode(comment.toCString(utf)).trimmed();
        ui.commentLineEdit_v2->setText(string);
        string = QString("%1").arg(f.ID3v2Tag()->year());
        ui.yearLineEdit_v2->setText(string);
        string = QString("%1").arg(f.ID3v2Tag()->track());
        ui.trackLineEdit_v2->setText(string);
        string = m_codec_v2->toUnicode(genre.toCString(utf)).trimmed();
        ui.genreLineEdit_v2->setText(string);
    }
    connect(ui.saveV2Button, SIGNAL(clicked()), SLOT(saveID3v2Tag()));
}

void DetailsDialog::saveID3v1Tag()
{
    TagLib::MPEG::File f (m_path.toLocal8Bit());

    TagLib::String::Type type = TagLib::String::Latin1;

    if (m_codec_v1->name().contains("UTF"))
        return;

    f.ID3v1Tag(TRUE)->setTitle(TagLib::String(m_codec_v1->fromUnicode(ui.titleLineEdit_v1->text()).constData(), type));
    f.ID3v1Tag()->setArtist(TagLib::String(m_codec_v1->fromUnicode(ui.artistLineEdit_v1->text()).constData(), type));
    f.ID3v1Tag()->setAlbum(TagLib::String(m_codec_v1->fromUnicode(ui.albumLineEdit_v1->text()).constData(), type));
    f.ID3v1Tag()->setComment(TagLib::String(m_codec_v1->fromUnicode(ui.commentLineEdit_v2->text()).constData(), type));
    f.ID3v1Tag()->setGenre(TagLib::String(m_codec_v1->fromUnicode(ui.genreLineEdit_v1->text()).constData(), type));
    f.ID3v1Tag()->setYear(ui.yearLineEdit_v1->text().toUInt());
    f.ID3v1Tag()->setTrack(ui.trackLineEdit_v1->text().toUInt());

    f.save();
}

void DetailsDialog::saveID3v2Tag()
{
    TagLib::MPEG::File f (m_path.toLocal8Bit());

    TagLib::String::Type type = TagLib::String::Latin1;

    if (m_codec_v2->name().contains("UTF"))
    {
        TagLib::ID3v2::FrameFactory *factory = TagLib::ID3v2::FrameFactory::instance();
        factory->setDefaultTextEncoding(TagLib::String::UTF8);
        f.setID3v2FrameFactory(factory);
        type = TagLib::String::UTF8;
    }

    f.ID3v2Tag(TRUE)->setTitle(TagLib::String(m_codec_v2->fromUnicode(ui.titleLineEdit_v2->text()).constData(), type));
    f.ID3v2Tag()->setArtist(TagLib::String(m_codec_v2->fromUnicode(ui.artistLineEdit_v2->text()).constData(), type));
    f.ID3v2Tag()->setAlbum(TagLib::String(m_codec_v2->fromUnicode(ui.albumLineEdit_v2->text()).constData(), type));
    f.ID3v2Tag()->setComment(TagLib::String(m_codec_v2->fromUnicode(ui.commentLineEdit_v2->text()).constData(), type));
    f.ID3v2Tag()->setGenre(TagLib::String(m_codec_v2->fromUnicode(ui.genreLineEdit_v2->text()).constData(), type));
    f.ID3v2Tag()->setYear(ui.yearLineEdit_v2->text().toUInt());
    f.ID3v2Tag()->setTrack(ui.trackLineEdit_v2->text().toUInt());

    f.save();
}
