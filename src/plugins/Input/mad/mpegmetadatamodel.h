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

#ifndef MPEGMETADATAMODEL_H
#define MPEGMETADATAMODEL_H

#include <qmmp/metadatamodel.h>
#include <taglib/mpegfile.h>

class QTextCodec;

class MPEGMetaDataModel : public MetaDataModel
{
Q_OBJECT
public:
    MPEGMetaDataModel(const QString &path, QObject *parent);
    ~MPEGMetaDataModel();
    QHash<QString, QString> audioProperties();
    QList<TagModel* > tags();

private:
    QString m_path;
    QList<TagModel* > m_tags;
};

class MpegFileTagModel : public TagModel
{
public:
    MpegFileTagModel(const QString &path, TagLib::MPEG::File::TagTypes tagType);
    ~MpegFileTagModel();
    const QString name();
    QList<Qmmp::MetaData> keys();
    const QString value(Qmmp::MetaData key);
    void setValue(Qmmp::MetaData key, const QString &value);
    bool exists();
    void create();
    void remove();
    void save();

private:
    QTextCodec *m_codec;
    TagLib::MPEG::File *m_file;
    TagLib::Tag *m_tag;
    TagLib::MPEG::File::TagTypes m_tagType;
};

#endif // MPEGMETADATAMODEL_H
