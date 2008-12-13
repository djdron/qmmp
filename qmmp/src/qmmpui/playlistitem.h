/***************************************************************************
 *   Copyright (C) 2008 by Ilya Kotov                                      *
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
#ifndef PLAYLISTITEM_H
#define PLAYLISTITEM_H

#include <qmmp/qmmp.h>
#include <qmmpui/abstractplaylistitem.h>

class FileInfo;
class QSettings;
/**
	@author Ilya Kotov <forkotov02@hotmail.ru>
*/
class PlayListItem : public AbstractPlaylistItem
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
    PlayListItem();
    //PlayListItem(const QString& path);
    PlayListItem(FileInfo *info, QSettings *settings = 0);

    ~PlayListItem();

    //playlist support
    void setSelected(bool yes);
    bool isSelected() const;
    void setCurrent(bool yes);
    bool isCurrent() const;
    FLAGS flag()const;
    void setFlag(FLAGS);
    const QString text() const;
    void setText(const QString &title);
    //modify functions
    void updateMetaData(const QMap <Qmmp::MetaData, QString> &metaData);
    void updateTags();

private:
    void readMetadata();
    QString printTag(QString str, QString regExp, QString tagStr);
    QString m_title;
    FileInfo *m_info;
    bool m_selected;
    bool m_current;
    bool m_use_meta;
    bool m_convertUnderscore, m_convertTwenty, m_fullStreamPath;
    QString m_format;
    FLAGS m_flag;
};

#endif
