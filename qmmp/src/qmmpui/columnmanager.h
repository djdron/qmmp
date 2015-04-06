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

#ifndef COLUMNMANAGER_H
#define COLUMNMANAGER_H

#include <QObject>
#include <QWidget>
#include "metadataformatter.h"

/**
 * @author Ilya Kotov <forkotov02@hotmail.ru>
 */
class ColumnManager : public QObject
{
    Q_OBJECT
public:
    explicit ColumnManager(QObject *parent = 0);

    ~ColumnManager();

    void insert(int index, const QString &name, const QString &pattern);
    void remove(int index);
    void resize(int index, int size);
    void setAutoResize(int index, bool autoResize);
    void move(int from, int to);
    void execEditor(int index, QWidget *parent = 0);

    int count();
    const MetaDataFormatter* titleFormatter(int index) const;
    int size(int index) const;
    const QString name(int index) const;
    const QString pattern(int index) const;
    QList<int> autoResizeColumns() const;

signals:
    void inserted(int index);
    void removed(int index);
    void changed(int index);
    void resized(int index);
    void moved(int from, int to);

private:
    void sync();
    struct Column
    {
        QString name;
        QString pattern;
        int size;
        bool autoResize;
        MetaDataFormatter *titleFormatter;
    };
    QList<Column> m_columns;
};

#endif // COLUMNMANAGER_H
