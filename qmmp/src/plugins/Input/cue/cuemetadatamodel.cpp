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

#include <qmmp/metadatamanager.h>
#include "cueparser.h"
#include "cuemetadatamodel.h"

CUEMetaDataModel::CUEMetaDataModel(const QString &url, QObject *parent) : MetaDataModel(parent)
{
    QString p = QUrl(url).path();
    p.replace(QString(QUrl::toPercentEncoding("#")), "#");
    p.replace(QString(QUrl::toPercentEncoding("?")), "?");
    p.replace(QString(QUrl::toPercentEncoding("%")), "%");
    m_parser = new CUEParser(p);
    if (m_parser->count() == 0)
    {
        qWarning("CUEMetaDataModel: invalid cue file");
        return;
    }
    int track = url.section("#", -1).toInt();
    m_path = m_parser->filePath(track);
}

CUEMetaDataModel::~CUEMetaDataModel()
{
    delete m_parser;
}

QPixmap CUEMetaDataModel::cover()
{
    return MetaDataManager::instance()->getCover(m_path);
}

QString CUEMetaDataModel::coverPath()
{
    return MetaDataManager::instance()->getCoverPath(m_path);
}
