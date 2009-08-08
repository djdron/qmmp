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
#include <taglib/tag.h>
#include <taglib/fileref.h>
#include <taglib/flacfile.h>
#include <taglib/xiphcomment.h>
#include <taglib/tmap.h>

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
        loadFLACInfo();
        loadTags();
        blockSaveButton(!QFileInfo(m_path).isWritable());
    }
    else
        blockSaveButton();
}

DetailsDialog::~DetailsDialog()
{}

void DetailsDialog::loadFLACInfo()
{
    TagLib::FLAC::File f (m_path.toLocal8Bit());
    QMap <QString, QString> ap;
    QString text = QString("%1").arg(f.audioProperties()->length()/60);
    text +=":"+QString("%1").arg(f.audioProperties()->length()%60,2,10,QChar('0'));
    ap.insert(tr("Length"), text);
    ap.insert(tr("Sample rate"), QString("%1 " + tr("Hz")).arg(f.audioProperties()->sampleRate()));
    ap.insert(tr("Channels"), QString("%1").arg(f.audioProperties()->channels()));
    ap.insert(tr("Bitrate"), QString("%1 " + tr("kbps")).arg(f.audioProperties()->bitrate()));
    ap.insert(tr("Sample width"), QString("%1 "+tr("bits")).arg(f.audioProperties()->sampleWidth()));
    ap.insert(tr("File size"), QString("%1 "+tr("KB")).arg(f.length()/1024));
    setAudioProperties(ap);
}

void DetailsDialog::loadTags()
{
    TagLib::FileRef f (m_path.toLocal8Bit());
    setMetaData(Qmmp::TITLE, TStringToQString_qt4(f.tag()->title()));
    setMetaData(Qmmp::ARTIST, TStringToQString_qt4(f.tag()->artist()));
    setMetaData(Qmmp::ALBUM, TStringToQString_qt4(f.tag()->album()));
    setMetaData(Qmmp::COMMENT, TStringToQString_qt4(f.tag()->comment()));
    setMetaData(Qmmp::GENRE, TStringToQString_qt4(f.tag()->genre()));
    setMetaData(Qmmp::YEAR, f.tag()->year());
    setMetaData(Qmmp::TRACK, f.tag()->track());
    setMetaData(Qmmp::URL, m_path);
    TagLib::FLAC::File *file = dynamic_cast< TagLib::FLAC::File *>(f.file());
    TagLib::StringList fld;
    if(file->xiphComment() && !(fld = file->xiphComment()->fieldListMap()["COMPOSER"]).isEmpty())
        setMetaData(Qmmp::COMPOSER, TStringToQString_qt4(fld.toString()));
    if(file->xiphComment() && !(fld = file->xiphComment()->fieldListMap()["DISCNUMBER"]).isEmpty())
        setMetaData(Qmmp::DISCNUMBER, TStringToQString_qt4(fld.toString()));
}

void DetailsDialog::writeTags()
{
    TagLib::FileRef f (m_path.toLocal8Bit());
    f.tag()->setTitle(QStringToTString_qt4(strMetaData(Qmmp::TITLE)));
    f.tag()->setArtist(QStringToTString_qt4(strMetaData(Qmmp::ARTIST)));
    f.tag()->setAlbum(QStringToTString_qt4(strMetaData(Qmmp::ALBUM)));
    f.tag()->setComment(QStringToTString_qt4(strMetaData(Qmmp::COMMENT)));
    f.tag()->setGenre(QStringToTString_qt4(strMetaData(Qmmp::GENRE)));
    f.tag()->setYear(intMetaData(Qmmp::YEAR));
    f.tag()->setTrack(intMetaData(Qmmp::TRACK));
    TagLib::FLAC::File *file = dynamic_cast< TagLib::FLAC::File *>(f.file());
    strMetaData(Qmmp::COMPOSER).isEmpty() ?
    file->xiphComment()->removeField("COMPOSER"):
    file->xiphComment()->addField("COMPOSER", QStringToTString_qt4(strMetaData(Qmmp::COMPOSER)), TRUE);
    intMetaData(Qmmp::DISCNUMBER) == 0 ?
    file->xiphComment()->removeField("DISCNUMBER"):
    file->xiphComment()->addField("DISCNUMBER",
                                  QStringToTString_qt4(strMetaData(Qmmp::DISCNUMBER)), TRUE);
    f.save();
}
