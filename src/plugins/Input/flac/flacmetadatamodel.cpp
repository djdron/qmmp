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

#include <QPixmap>
#include <taglib/tag.h>
#include <taglib/fileref.h>
#include <taglib/tmap.h>
#include <qmmp/metadatamanager.h>
#include "flacmetadatamodel.h"

#define QStringToTString_qt4(s) TagLib::String(s.toUtf8().constData(), TagLib::String::UTF8)
#define TStringToQString_qt4(s) QString::fromUtf8(s.toCString(TRUE)).trimmed()

FLACMetaDataModel::FLACMetaDataModel(const QString &path, QObject *parent) : MetaDataModel(parent)
{
    if(path.startsWith("flac://"))
    {
        QString p = QUrl(path).path();
        p.replace(QString(QUrl::toPercentEncoding("#")), "#");
        p.replace(QString(QUrl::toPercentEncoding("?")), "?");
        p.replace(QString(QUrl::toPercentEncoding("%")), "%");
        m_path = p;
    }
    else
    {
        m_path = path;
        m_tags << new VorbisCommentModel(path);
    }
}

FLACMetaDataModel::~FLACMetaDataModel()
{
    while(!m_tags.isEmpty())
        delete m_tags.takeFirst();
}

QHash<QString, QString> FLACMetaDataModel::audioProperties()
{
    QHash<QString, QString> ap;
    TagLib::FLAC::File f (m_path.toLocal8Bit());
    if(!f.audioProperties())
        return ap;
    QString text = QString("%1").arg(f.audioProperties()->length()/60);
    text +=":"+QString("%1").arg(f.audioProperties()->length()%60,2,10,QChar('0'));
    ap.insert(tr("Length"), text);
    ap.insert(tr("Sample rate"), QString("%1 " + tr("Hz")).arg(f.audioProperties()->sampleRate()));
    ap.insert(tr("Channels"), QString("%1").arg(f.audioProperties()->channels()));
    ap.insert(tr("Bitrate"), QString("%1 " + tr("kbps")).arg(f.audioProperties()->bitrate()));
    ap.insert(tr("File size"), QString("%1 "+tr("KB")).arg(f.length()/1024));
    return ap;
}

QList<TagModel* > FLACMetaDataModel::tags()
{
    return m_tags;
}

QPixmap FLACMetaDataModel::cover()
{
    QString cPath = coverPath();
    return cPath.isEmpty() ? QPixmap() : QPixmap(cPath);
}

QString FLACMetaDataModel::coverPath()
{
    return MetaDataManager::instance()->getCoverPath(m_path);
}

VorbisCommentModel::VorbisCommentModel(const QString &path) : TagModel(TagModel::Save)
{
    m_file = new TagLib::FLAC::File (path.toLocal8Bit().constData());
    m_tag = m_file->xiphComment();
}

VorbisCommentModel::~VorbisCommentModel()
{
    delete m_file;
}

const QString VorbisCommentModel::name()
{
    return "Vorbis Comment";
}

const QString VorbisCommentModel::value(Qmmp::MetaData key)
{
    if(!m_tag)
        return QString();
    switch((int) key)
    {
    case Qmmp::TITLE:
        return TStringToQString_qt4(m_tag->title());
    case Qmmp::ARTIST:
        return TStringToQString_qt4(m_tag->artist());
    case Qmmp::ALBUM:
        return TStringToQString_qt4(m_tag->album());
    case Qmmp::COMMENT:
        return TStringToQString_qt4(m_tag->comment());
    case Qmmp::GENRE:
        return TStringToQString_qt4(m_tag->genre());
    case Qmmp::COMPOSER:
        if(m_tag->fieldListMap()["COMPOSER"].isEmpty())
            return QString();
        else
            return TStringToQString_qt4(m_tag->fieldListMap()["COMPOSER"].front());
    case Qmmp::YEAR:
        return QString::number(m_tag->year());
    case Qmmp::TRACK:
        return QString::number(m_tag->track());
    case  Qmmp::DISCNUMBER:
        if(m_tag->fieldListMap()["DISCNUMBER"].isEmpty())
            return QString();
        else
            return TStringToQString_qt4(m_tag->fieldListMap()["DISCNUMBER"].front());
    }
    return QString();
}

void VorbisCommentModel::setValue(Qmmp::MetaData key, const QString &value)
{
    if(!m_tag)
        return;

    TagLib::String str = QStringToTString_qt4(value);

    switch((int) key)
    {
    case Qmmp::TITLE:
        m_tag->setTitle(str);
        return;
    case Qmmp::ARTIST:
        m_tag->setArtist(str);
        return;
    case Qmmp::ALBUM:
        m_tag->setAlbum(str);
        return;
    case Qmmp::COMMENT:
        m_tag->setComment(str);
        return;
    case Qmmp::GENRE:
        m_tag->setGenre(str);
        return;
    case Qmmp::COMPOSER:
        value.isEmpty() ?
        m_tag->removeField("COMPOSER"):
        m_tag->addField("COMPOSER", str, TRUE);
        return;
    case Qmmp::TRACK:
        m_tag->setTrack(value.toInt());
        return;
    case Qmmp::YEAR:
        m_tag->setYear(value.toInt());
        return;
    case Qmmp::DISCNUMBER:
        value == "0" ?
        m_tag->removeField("DISCNUMBER"):
        m_tag->addField("DISCNUMBER", str, TRUE);
    }
}

void VorbisCommentModel::save()
{
    m_file->save();
}
