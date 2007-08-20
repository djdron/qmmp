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
#ifndef MEDIAFILE_H
#define MEDIAFILE_H

#include <QString>

class FileTag;
/**
   @author Ilya Kotov <forkotov02@hotmail.ru>
*/


class MediaFile
{
public:
    MediaFile()
    {};
    MediaFile(QString);

    ~MediaFile();
    //MediaFile &operator=(const MediaFile &other);

    const QString path()const;
    const QString title()const;
    const QString fileName()const;
    uint year()const;
    int length()const;
    void setSelected(bool);
    bool isSelected()const;
    bool isCurrent();
    void setCurrent(bool);
    void updateTags(const FileTag*);


private:
    void readMetadata();
    QString m_path;
    QString m_title;
    uint m_year;
    FileTag *m_tag;
    bool m_selected;
    bool m_current;
    bool m_use_meta;
    QString m_format;

};

#endif
