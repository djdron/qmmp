/***************************************************************************
 *   Copyright (C) 2006-2008 by Ilya Kotov                                 *
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
#include <taglib/apetag.h>
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
        QTextCodec::codecForName(settings.value("ID3v1_encoding", "ISO-8859-1" )
                                 .toByteArray ());
    m_codec_v2 =
        QTextCodec::codecForName(settings.value("ID3v2_encoding","UTF-8" )
                                 .toByteArray ());
    if (!m_codec_v1)
        m_codec_v1 = QTextCodec::codecForName ("ISO-8859-1");
    if (!m_codec_v2)
        m_codec_v2 = QTextCodec::codecForName ("UTF-8");

    QString tag_name = settings.value("current_tag","ID3v2").toString ();

    if (tag_name == "ID3v1")
        ui.id3v1RadioButton->setChecked(TRUE);
    else if (tag_name == "ID3v2")
        ui.id3v2RadioButton->setChecked(TRUE);
    else if (tag_name == "APE")
        ui.apeRadioButton->setChecked(TRUE);
    else
        ui.id3v2RadioButton->setChecked(TRUE);

    settings.endGroup();

    loadMPEGInfo();
    QFileInfo info(m_path);
    m_rw = info.isWritable();
    loadTag();
    connect(ui.saveButton, SIGNAL(clicked()), SLOT(save()));
    connect(ui.createButton, SIGNAL(clicked()), SLOT(create()));
    connect(ui.deleteButton, SIGNAL(clicked()), SLOT(deleteTag()));
    connect(ui.id3v1RadioButton, SIGNAL(clicked()), SLOT(loadTag()));
    connect(ui.id3v2RadioButton, SIGNAL(clicked()), SLOT(loadTag()));
    connect(ui.apeRadioButton, SIGNAL(clicked()), SLOT(loadTag()));
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

void DetailsDialog::loadTag()
{
    TagLib::MPEG::File f (m_path.toLocal8Bit());
    QTextCodec *codec = QTextCodec::codecForName ("UTF-8");
    TagLib::Tag *tag = 0;

    if (selectedTag() == TagLib::MPEG::File::ID3v1)
    {
        tag = f.ID3v1Tag();
        codec = m_codec_v1;
        ui.tagGroupBox->setTitle(tr("ID3v1 Tag"));
    }
    else if (selectedTag() == TagLib::MPEG::File::ID3v2)
    {
        tag = f.ID3v2Tag();
        codec = m_codec_v2;
        ui.tagGroupBox->setTitle(tr("ID3v2 Tag"));
    }
    else if (selectedTag() == TagLib::MPEG::File::APE)
    {
        ui.tagGroupBox->setTitle(tr("APE Tag"));
        tag = f.APETag();
    }
    ui.saveButton->setEnabled(tag && m_rw);
    ui.createButton->setEnabled(!tag && m_rw);
    ui.deleteButton->setEnabled(tag && m_rw);
    ui.tagsWidget->setEnabled(tag);
    //clear old values
    ui.titleLineEdit->clear();
    ui.artistLineEdit->clear();
    ui.albumLineEdit->clear();
    ui.commentLineEdit->clear();
    ui.yearLineEdit->clear();
    ui.trackLineEdit->clear();
    ui.genreLineEdit->clear();

    if (tag)
    {
        bool utf = codec->name().contains("UTF");
        if (utf)
            codec = QTextCodec::codecForName ("UTF-8");
        TagLib::String title = tag->title();
        TagLib::String artist = tag->artist();
        TagLib::String album = tag->album();
        TagLib::String comment = tag->comment();
        TagLib::String genre = tag->genre();
        QString string = codec->toUnicode(title.toCString(utf)).trimmed();
        ui.titleLineEdit->setText(string);
        string = codec->toUnicode(artist.toCString(utf)).trimmed();
        ui.artistLineEdit->setText(string);
        string = codec->toUnicode(album.toCString(utf)).trimmed();
        ui.albumLineEdit->setText(string);
        string = codec->toUnicode(comment.toCString(utf)).trimmed();
        ui.commentLineEdit->setText(string);
        string = QString("%1").arg(tag->year());
        ui.yearLineEdit->setText(string);
        string = QString("%1").arg(tag->track());
        ui.trackLineEdit->setText(string);
        string = codec->toUnicode(genre.toCString(utf)).trimmed();
        ui.genreLineEdit->setText(string);
    }
}

void DetailsDialog::save()
{
    TagLib::MPEG::File* f = new  TagLib::MPEG::File(m_path.toLocal8Bit());
    TagLib::String::Type type = TagLib::String::Latin1;

    QTextCodec *codec = 0;
    TagLib::Tag *tag = 0;

    if (selectedTag() == TagLib::MPEG::File::ID3v1)
    {
        codec = m_codec_v1;
        tag = f->ID3v1Tag(TRUE);
        if (codec->name().contains("UTF"))
        {
            delete f;
            loadTag();
        }
    }
    if (selectedTag() == TagLib::MPEG::File::ID3v2)
    {
        codec = m_codec_v2;
        tag = f->ID3v2Tag(TRUE);

        if (codec->name().contains("UTF"))
        {
            type = TagLib::String::UTF8;
            if (codec->name().contains("UTF-16"))
                type = TagLib::String::UTF16;
            else if (codec->name().contains("UTF-16LE"))
                type = TagLib::String::UTF16LE;
            else if (codec->name().contains("UTF-16BE"))
                type = TagLib::String::UTF16BE;

            codec = QTextCodec::codecForName ("UTF-8");
            TagLib::ID3v2::FrameFactory *factory = TagLib::ID3v2::FrameFactory::instance();
            factory->setDefaultTextEncoding(type);
            f->setID3v2FrameFactory(factory);
            type = TagLib::String::UTF8;
        }
    }
    if (selectedTag() == TagLib::MPEG::File::APE)
    {
        codec = QTextCodec::codecForName ("UTF-8");
        tag = f->APETag(TRUE);
        type = TagLib::String::UTF8;
    }

    tag->setTitle(TagLib::String(codec->fromUnicode(ui.titleLineEdit->text()).constData(), type));
    tag->setArtist(TagLib::String(codec->fromUnicode(ui.artistLineEdit->text()).constData(), type));
    tag->setAlbum(TagLib::String(codec->fromUnicode(ui.albumLineEdit->text()).constData(), type));
    tag->setComment(TagLib::String(codec->fromUnicode(ui.commentLineEdit->text()).constData(), type));
    tag->setGenre(TagLib::String(codec->fromUnicode(ui.genreLineEdit->text()).constData(), type));
    tag->setYear(ui.yearLineEdit->text().toUInt());
    tag->setTrack(ui.trackLineEdit->text().toUInt());

    f->save(selectedTag(), FALSE);
    delete f;
    loadTag();
}

void DetailsDialog::create()
{
    TagLib::MPEG::File *f = new TagLib::MPEG::File (m_path.toLocal8Bit());
    TagLib::Tag *tag = 0;
    if (selectedTag() == TagLib::MPEG::File::ID3v1)
        tag = f->ID3v1Tag(TRUE);
    else if (selectedTag() == TagLib::MPEG::File::ID3v2)
        tag = f->ID3v2Tag(TRUE);
    else if (selectedTag() == TagLib::MPEG::File::APE)
        tag = f->APETag(TRUE);

    f->save(selectedTag(), FALSE);
    delete f;
    loadTag();
    ui.tagsWidget->setEnabled(TRUE);
    ui.saveButton->setEnabled(m_rw);
}

void DetailsDialog::deleteTag()
{
    TagLib::MPEG::File *f = new TagLib::MPEG::File (m_path.toLocal8Bit());
    f->strip(selectedTag());
    delete f;
    loadTag();
}

uint DetailsDialog::selectedTag()
{
    if (ui.id3v1RadioButton->isChecked())
        return TagLib::MPEG::File::ID3v1;
    else if (ui.id3v2RadioButton->isChecked())
        return TagLib::MPEG::File::ID3v2;
    else if (ui.apeRadioButton->isChecked())
        return TagLib::MPEG::File::APE;
    return TagLib::MPEG::File::ID3v2;
}

void DetailsDialog::closeEvent (QCloseEvent *)
{
    QSettings settings(QDir::homePath()+"/.qmmp/qmmprc", QSettings::IniFormat);
    settings.beginGroup("MAD");
    if (ui.id3v1RadioButton->isChecked())
        settings.setValue("current_tag","ID3v1");
    else if (ui.id3v2RadioButton->isChecked())
        settings.setValue("current_tag","ID3v2");
    else if (ui.apeRadioButton->isChecked())
        settings.setValue("current_tag","APE");
    settings.endGroup();
}
