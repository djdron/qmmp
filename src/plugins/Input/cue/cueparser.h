/***************************************************************************
 *   Copyright (C) 2008-2011 by Ilya Kotov                                 *
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
#ifndef CUEPARSER_H
#define CUEPARSER_H

#include <QList>
#include <QMap>
#include <QString>
#include <QStringList>
#include <QUrl>
#include <qmmp/qmmp.h>
#include <qmmp/fileinfo.h>


/**
    @author Ilya Kotov <forkotov02@hotmail.ru>
*/
class CUEParser
{
public:
    CUEParser(const QString &url);

    ~CUEParser();

    QList<FileInfo*> createPlayList();
    const QString filePath(int track) const;
    qint64 offset(int track) const;
    qint64 length(int track) const;
    int count() const;
    FileInfo *info(int track);
    const QString trackURL(int track);

private:
    QList <FileInfo> m_infoList;
    QList <qint64> m_offsets;
    QStringList m_files;
    bool m_dirty;
    QStringList splitLine(const QString &line);
    qint64 getLength(const QString &str);
    QString getDirtyPath(const QString &cue, const QString &path);
};

#endif
