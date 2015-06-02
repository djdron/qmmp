/***************************************************************************
 *   Copyright (C) 2015 by Ilya Kotov                                      *
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

#ifndef PLAYLISTHEADERMODEL_H
#define PLAYLISTHEADERMODEL_H

#include <QObject>
#include <QWidget>
#include <QSettings>
#include <QHash>
#include <QVariant>
#include "metadataformatter.h"

class MetaDataHelper;

/**
 * @author Ilya Kotov <forkotov02@hotmail.ru>
 */
class PlayListHeaderModel : public QObject
{
    Q_OBJECT
public:
    explicit PlayListHeaderModel(QObject *parent = 0);

    ~PlayListHeaderModel();

    void restoreSettings(const QString &groupName);
    void restoreSettings(QSettings *settings);
    void saveSettings(const QString &groupName);
    void saveSettings(QSettings *settings);
    bool isSettingsLoaded() const;

    void insert(int index, const QString &name, const QString &pattern);
    void remove(int index);
    void move(int from, int to);
    void execEdit(int index, QWidget *parent = 0);
    void execInsert(int index, QWidget *parent = 0);

    int count();

    const QString name(int index) const;
    const QString pattern(int index) const;

    void setData(int index, int key, const QVariant &data);
    const QVariant data(int index, int key) const;

signals:
    void columnAdded(int index);
    void columnRemoved(int index);
    void columnChanged(int index);
    void columnMoved(int from, int to);
    void headerChanged();

private:
    void updatePlayLists();

    struct ColumnHeader
    {
        QString name;
        QString pattern;
        QHash<int, QVariant> data;
    };
    QList<ColumnHeader> m_columns;
    bool m_settings_loaded;
    MetaDataHelper *m_helper;
};

#endif // COLUMNMANAGER_H
