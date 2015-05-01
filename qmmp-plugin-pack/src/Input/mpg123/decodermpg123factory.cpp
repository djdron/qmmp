/***************************************************************************
 *   Copyright (C) 2011-2015 by Ilya Kotov                                 *
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
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#include <QtGui>
#include <QDialog>
#include <QMessageBox>
#include <QFile>
#include <taglib/tag.h>
#include <taglib/fileref.h>
#include <taglib/id3v1tag.h>
#include <taglib/id3v2tag.h>
#include <taglib/apetag.h>
#include <taglib/tfile.h>
#include <taglib/mpegfile.h>
#if (TAGLIB_MAJOR_VERSION > 1) || ((TAGLIB_MAJOR_VERSION == 1) && (TAGLIB_MINOR_VERSION >= 8))
#include <taglib/tfilestream.h>
#endif
#include "mpegmetadatamodel.h"
#include "replaygainreader.h"
#include "settingsdialog.h"
#include "decoder_mpg123.h"
#include "decodermpg123factory.h"

// DecoderMPG123Factory

DecoderMPG123Factory::DecoderMPG123Factory()
{
    //detecting rusxmms patch
    m_using_rusxmms = false;
    char str[] = { char(0xF2), char(0xE5), char(0xF1), char(0xF2), '\0'};
    QTextCodec *codec = QTextCodec::codecForName ("windows-1251");
    TagLib::String tstr(str);
    if(codec->toUnicode(str) == QString::fromUtf8(tstr.toCString(true)))
    {
        qDebug("DecoderMPG123Factory: found taglib with rusxmms patch");
        m_using_rusxmms = true;
    }
}

bool DecoderMPG123Factory::supports(const QString &source) const
{
    QString ext = source.right(4).toLower();
    if (ext == ".mp1" || ext == ".mp2" || ext == ".mp3")
        return true;
    else if (ext == ".wav") //check for mp3 wav files
    {
        QFile file(source);
        file.open(QIODevice::ReadOnly);
        char buf[22];
        file.peek(buf,sizeof(buf));
        file.close();
        if (!memcmp(buf + 8, "WAVE", 4) && !memcmp(buf + 20, "U" ,1))
        {
            return true;
        }
    }
    return false;
}

bool DecoderMPG123Factory::canDecode(QIODevice *input) const
{
    char buf[16 * 512];
    if (input->peek(buf,sizeof(buf)) == sizeof(buf))
    {
        mpg123_init();
        mpg123_handle *handle = mpg123_new(0, 0);
        if (!handle)
            return false;
        if(mpg123_open_feed(handle) != MPG123_OK)
        {
            mpg123_delete(handle);
            return false;
        }
        if (mpg123_format(handle, 44100, MPG123_STEREO, MPG123_ENC_SIGNED_16) != MPG123_OK)
        {
            mpg123_close(handle);
            mpg123_delete(handle);
            return false;
        }
        size_t out_size = 0;
        int ret = mpg123_decode(handle, (unsigned char*) buf, sizeof(buf), 0, 0, &out_size);
        mpg123_close(handle);
        mpg123_delete(handle);
        if (ret != MPG123_DONE && ret != MPG123_NEW_FORMAT)
            return false;
        return true;
    }
    return false;
}

const DecoderProperties DecoderMPG123Factory::properties() const
{
    DecoderProperties properties;
    properties.name = tr("MPG123 Plugin");
    properties.shortName = "mpg123";
    properties.filters << "*.mp1" << "*.mp2" << "*.mp3" << "*.wav";
    properties.description = tr("MPEG Files");
    properties.contentTypes << "audio/mp3" << "audio/mpeg";
    properties.hasAbout = true;
    properties.hasSettings = true;
    properties.noInput = false;
    properties.priority = 10; //lowest priority
    return properties;
}

Decoder *DecoderMPG123Factory::create(const QString &url, QIODevice *input)
{
    Decoder *d = new DecoderMPG123(url, input);
    if(!url.contains("://")) //local file
    {
        ReplayGainReader rg(url);
        d->setReplayGainInfo(rg.replayGainInfo(), true);
    }
    return d;
}

QList<FileInfo *> DecoderMPG123Factory::createPlayList(const QString &fileName, bool useMetaData, QStringList *)
{
    FileInfo *info = new FileInfo(fileName);
    TagLib::Tag *tag = 0;
#if (TAGLIB_MAJOR_VERSION > 1) || ((TAGLIB_MAJOR_VERSION == 1) && (TAGLIB_MINOR_VERSION >= 8))
    TagLib::FileStream stream(fileName.toLocal8Bit().constData(), true);
    TagLib::MPEG::File fileRef(&stream, TagLib::ID3v2::FrameFactory::instance());
#else
    TagLib::MPEG::File fileRef(fileName.toLocal8Bit ().constData());
#endif

    if (useMetaData)
    {
        QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
        settings.beginGroup("MPG123");

        QTextCodec *codec = 0;

        uint tag_array[3];
        tag_array[0] = settings.value("tag_1", SettingsDialog::ID3v2).toInt();
        tag_array[1] = settings.value("tag_2", SettingsDialog::Disabled).toInt();
        tag_array[2] = settings.value("tag_3", SettingsDialog::Disabled).toInt();

        QByteArray name;
        for (int i = 0; i < 3; ++i)
        {
            switch ((uint) tag_array[i])
            {
            case SettingsDialog::ID3v1:
                codec = QTextCodec::codecForName(settings.value("ID3v1_encoding","ISO-8859-1")
                                                 .toByteArray ());
                tag = fileRef.ID3v1Tag();
                break;
            case SettingsDialog::ID3v2:
                name = settings.value("ID3v2_encoding","UTF-8").toByteArray ();
                if (name.contains("UTF"))
                    codec = QTextCodec::codecForName ("UTF-8");
                else
                    codec = QTextCodec::codecForName(name);
                tag = fileRef.ID3v2Tag();
                break;
            case SettingsDialog::APE:
                codec = QTextCodec::codecForName ("UTF-8");
                tag = fileRef.APETag();
                break;
            case SettingsDialog::Disabled:
                break;
            }
            if (tag && !tag->isEmpty())
                break;
        }
        settings.endGroup();

        if(m_using_rusxmms)
            codec = QTextCodec::codecForName ("UTF-8");

        if (!codec)
            codec = QTextCodec::codecForName ("UTF-8");

        if (tag && codec)
        {
            bool utf = codec->name ().contains("UTF");
            TagLib::String album = tag->album();
            TagLib::String artist = tag->artist();
            TagLib::String comment = tag->comment();
            TagLib::String genre = tag->genre();
            TagLib::String title = tag->title();

            info->setMetaData(Qmmp::ALBUM,
                              codec->toUnicode(album.toCString(utf)).trimmed());
            info->setMetaData(Qmmp::ARTIST,
                              codec->toUnicode(artist.toCString(utf)).trimmed());
            info->setMetaData(Qmmp::COMMENT,
                              codec->toUnicode(comment.toCString(utf)).trimmed());
            info->setMetaData(Qmmp::GENRE,
                              codec->toUnicode(genre.toCString(utf)).trimmed());
            info->setMetaData(Qmmp::TITLE,
                              codec->toUnicode(title.toCString(utf)).trimmed());
            info->setMetaData(Qmmp::YEAR,
                              tag->year());
            info->setMetaData(Qmmp::TRACK,
                              tag->track());

            if(tag == fileRef.ID3v2Tag())
            {
                if(!fileRef.ID3v2Tag()->frameListMap()["TPE2"].isEmpty())
                {
                    TagLib::String albumArtist;
                    albumArtist = fileRef.ID3v2Tag()->frameListMap()["TPE2"].front()->toString();
                    info->setMetaData(Qmmp::ALBUMARTIST,
                                      codec->toUnicode(albumArtist.toCString(utf)).trimmed());
                }
                if(!fileRef.ID3v2Tag()->frameListMap()["TCOM"].isEmpty())
                {
                    TagLib::String composer;
                    composer = fileRef.ID3v2Tag()->frameListMap()["TCOM"].front()->toString();
                    info->setMetaData(Qmmp::COMPOSER,
                                      codec->toUnicode(composer.toCString(utf)).trimmed());
                }
                if(!fileRef.ID3v2Tag()->frameListMap()["TPOS"].isEmpty())
                {
                    TagLib::String disc = fileRef.ID3v2Tag()->frameListMap()["TPOS"].front()->toString();
                    info->setMetaData(Qmmp::DISCNUMBER, QString(disc.toCString()).trimmed());
                }
            }
            else if(tag == fileRef.APETag())
            {
                TagLib::APE::Item fld;
                if(!(fld = fileRef.APETag()->itemListMap()["ALBUM ARTIST"]).isEmpty())
                    info->setMetaData(Qmmp::ALBUMARTIST,
                                      QString::fromUtf8(fld.toString().toCString(true)).trimmed());
                if(!(fld = fileRef.APETag()->itemListMap()["COMPOSER"]).isEmpty())
                    info->setMetaData(Qmmp::COMPOSER,
                                      QString::fromUtf8(fld.toString().toCString(true)).trimmed());
            }
        }
    }
    if (fileRef.audioProperties())
        info->setLength(fileRef.audioProperties()->length());
    QList <FileInfo*> list;
    list << info;
    return list;
}

MetaDataModel* DecoderMPG123Factory::createMetaDataModel(const QString &path, QObject *parent)
{
   return new MPEGMetaDataModel(m_using_rusxmms, path, parent);
}

void DecoderMPG123Factory::showSettings(QWidget *parent)
{
    SettingsDialog *s = new SettingsDialog(m_using_rusxmms, parent);
    s -> show();
}

void DecoderMPG123Factory::showAbout(QWidget *parent)
{
    QMessageBox::about (parent, tr("About MPG123 Audio Plugin"),
                        tr("Qmmp MPG123 Audio Plugin")+"\n"+
                        tr("This plugin provides MPEG v1/2 layer1/2/3 support\n"
                           "with use of libmpg123 library")+"\n"+
                        tr("Written by: Ilya Kotov <forkotov02@hotmail.ru>")
                        );
}

QTranslator *DecoderMPG123Factory::createTranslator(QObject *parent)
{
    QTranslator *translator = new QTranslator(parent);
    QString locale = Qmmp::systemLanguageID();
    translator->load(QString(":/mpg123_plugin_") + locale);
    return translator;
}

Q_EXPORT_PLUGIN2(mpg123, DecoderMPG123Factory)
