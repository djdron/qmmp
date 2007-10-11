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
    /*!
     * Current state of media file.
     * FREE - instance is free and may be deleted
     * EDITING - instance is currently busy in some kind of operation(tags editing etc.)
     * and can't be deleted at the moment. Set flag SCHEDULED_FOR_DELETION for it 
     * instead of delete operator call.
     */
    enum FLAGS{FREE = 0,EDITING,SCHEDULED_FOR_DELETION};

    MediaFile() : m_flag(FREE)
    {};
    MediaFile(const QString&);

    ~MediaFile();

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
    void updateTags();
    void changeTitle(const QString&);
    FLAGS flag()const;
    void setFlag(FLAGS);

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
    FLAGS m_flag;

};

#endif
