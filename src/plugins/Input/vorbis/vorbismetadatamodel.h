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

#include <taglib/vorbisfile.h>
#include <taglib/xiphcomment.h>
#include <qmmp/metadatamodel.h>

class VorbisMetaDataModel : public MetaDataModel
{
Q_OBJECT
public:
    VorbisMetaDataModel(const QString &path, QObject *parent);
    ~VorbisMetaDataModel();
    QHash<QString, QString> audioProperties();
    QList<TagModel* > tags();

private:
    QString m_path;
    QList<TagModel* > m_tags;
};

class VorbisCommentModel : public TagModel
{
public:
    VorbisCommentModel(const QString &path);
    ~VorbisCommentModel();
    const QString name();
    //QList<Qmmp::MetaData> keys();
    const QString value(Qmmp::MetaData key);
    void setValue(Qmmp::MetaData key, const QString &value);
    bool exists();
    void create();
    void remove();
    void save();

private:
    TagLib::Ogg::Vorbis::File *m_file;
    TagLib::Ogg::XiphComment *m_tag;
};

#endif // MPEGMETADATAMODEL_H
