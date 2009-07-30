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

#include <qmmp/qmmp.h>

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

    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
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
    m_inputs << ui.titleLineEdit;
    m_inputs << ui.artistLineEdit;
    m_inputs << ui.albumLineEdit;
    //m_inputs << ui.commentBrowser;
    m_inputs << ui.yearLineEdit;
    m_inputs << ui.trackLineEdit;
    m_inputs << ui.genreLineEdit;
    ui.coverWidget->setPixmap(findCover(path));
}


DetailsDialog::~DetailsDialog()
{}

void DetailsDialog::loadMPEGInfo()
{
    TagLib::MPEG::File f (m_path.toLocal8Bit().constData());
    QString text;
    QString v;
    switch((int)f.audioProperties()->version())
    {
        case TagLib::MPEG::Header::Version1:
        v = "1";
        break;
        case TagLib::MPEG::Header::Version2:
        v = "2";
        break;
        case TagLib::MPEG::Header::Version2_5:
        v = "2.5";
    }
    QMap <QString, QString> ap;
    text = QString("MPEG-%1 layer %2").arg(v).arg(f.audioProperties()->layer());
    ap.insert(tr("Format"), text);
    text = QString("%1").arg(f.audioProperties()->bitrate());
    ap.insert(tr("Bitrate"), text+" "+tr("kbps"));
    text = QString("%1").arg(f.audioProperties()->sampleRate());
    ap.insert(tr("Samplerate"), text+" "+tr("Hz"));
    switch (f.audioProperties()->channelMode())
    {
    case TagLib::MPEG::Header::Stereo:
        ap.insert(tr("Mode"), "Stereo");
        break;
    case TagLib::MPEG::Header::JointStereo:
        ap.insert(tr("Mode"), "Joint stereo");
        break;
    case TagLib::MPEG::Header::DualChannel:
        ap.insert(tr("Mode"), "Dual channel");
        break;
    case TagLib::MPEG::Header::SingleChannel:
        ap.insert(tr("Mode"), "Single channel");
        break;
    }
    text = QString("%1 "+tr("KB")).arg(f.length()/1024);
    ap.insert(tr("File size"), text);
    if (f.audioProperties()->protectionEnabled())
        ap.insert(tr("Protection"), tr("Yes"));
    else
        ap.insert(tr("Protection"), tr("No"));
    if (f.audioProperties()->isCopyrighted())
        ap.insert(tr("Copyright"), tr("Yes"));
    else
        ap.insert(tr("Copyright"), tr("No"));
    if (f.audioProperties()->isOriginal())
        ap.insert(tr("Original"), tr("Yes"));
    else
        ap.insert(tr("Original"), tr("No"));
    showAudioProperties(ap);
}

void DetailsDialog::loadTag()
{
    TagLib::MPEG::File f (m_path.toLocal8Bit().constData());
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
    foreach(QLineEdit *le, m_inputs)
    {
        le->setEnabled(tag);
        le->clear(); //clear old values
    }
    ui.commentBrowser->setEnabled(tag);
    ui.commentBrowser->clear();

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
        ui.commentBrowser->setText(string);
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
    TagLib::MPEG::File* f = new  TagLib::MPEG::File(m_path.toLocal8Bit().constData());
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
    tag->setComment(TagLib::String(codec->fromUnicode(ui.commentBrowser->toPlainText ())
                                                                     .constData(), type));
    tag->setGenre(TagLib::String(codec->fromUnicode(ui.genreLineEdit->text()).constData(), type));
    tag->setYear(ui.yearLineEdit->text().toUInt());
    tag->setTrack(ui.trackLineEdit->text().toUInt());

    f->save(selectedTag(), FALSE);
    delete f;
    loadTag();
}

void DetailsDialog::create()
{
    TagLib::MPEG::File *f = new TagLib::MPEG::File (m_path.toLocal8Bit().constData());
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
    foreach(QLineEdit *le, m_inputs)
        le->setEnabled(TRUE);
    ui.commentBrowser->setEnabled(TRUE);
    ui.saveButton->setEnabled(m_rw);
}

void DetailsDialog::deleteTag()
{
    TagLib::MPEG::File *f = new TagLib::MPEG::File (m_path.toLocal8Bit().constData());
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
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("MAD");
    if (ui.id3v1RadioButton->isChecked())
        settings.setValue("current_tag","ID3v1");
    else if (ui.id3v2RadioButton->isChecked())
        settings.setValue("current_tag","ID3v2");
    else if (ui.apeRadioButton->isChecked())
        settings.setValue("current_tag","APE");
    settings.endGroup();
}

void DetailsDialog::showAudioProperties(QMap <QString, QString> p)
{
    QString formattedText;
    formattedText.append("<TABLE>");
    foreach(QString key, p.keys())
    {
        formattedText.append("<tr>");
        formattedText.append("<td>" + key + ":</td> <td style=\"padding-left: 5px; \"><b>"
                             + p.value(key) + "</b></td>");
        formattedText.append("</tr>");
    }
    formattedText.append("</TABLE>");
    ui.propertiesLabel->setText(formattedText);
}

QPixmap DetailsDialog::findCover(const QString &path)
{
    QString p = QFileInfo(path).absolutePath();
    QDir dir(p);
    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    dir.setSorting(QDir::Name);
    QStringList filters;
    filters << "*.jpg" << "*.png";
    QFileInfoList file_list = dir.entryInfoList(filters);
    if(!file_list.isEmpty())
        return QPixmap (file_list.at(0).absoluteFilePath());
    return QPixmap();
}
