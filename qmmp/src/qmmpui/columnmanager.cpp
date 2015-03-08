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

#include <QSettings>
#include <qmmp/qmmp.h>
#include "columnmanager.h"

ColumnManager::ColumnManager(QObject *parent) :
    QObject(parent)
{
    QSettings s (Qmmp::configFile(), QSettings::IniFormat);
    s.beginGroup("PlayList");
    int c = qMax(1, s.value("column_count", 1).toInt());
    for(int i = 0; i < c; ++i)
    {
        s.beginGroup(QString("column%1").arg(i));
        Column col;
        col.name = s.value("name", tr("Album - Title")).toString();
        col.pattern = s.value("pattern", "%p%if(%p&%t, - ,)%t").toString();
        col.size = s.value("size", 150).toInt();
        col.titleFormatter = new MetaDataFormatter(col.pattern);
        m_columns.append(col);
        s.endGroup();
    }
    s.endGroup();
}

ColumnManager::~ColumnManager()
{
    sync();
    foreach (Column col, m_columns)
    {
        delete col.titleFormatter;
        col.titleFormatter = 0;
    }
    m_columns.clear();
}

void ColumnManager::insert(int index, const QString &name, const QString &pattern)
{
    if(index < 0 || index >= m_columns.size())
        qWarning("ColumnManager: index is out of range");

    Column col;
    col.name = name;
    col.pattern = pattern;
    col.titleFormatter = new MetaDataFormatter(pattern);
    m_columns.insert(index, col);
    sync();
    emit inserted(index);
}

void ColumnManager::remove(int index)
{
    if(index < 0 || index >= m_columns.size())
        qWarning("ColumnManager: index is out of range");

    if(m_columns.count() == 1)
        return;

    delete m_columns.takeAt(index).titleFormatter;
    sync();
    emit removed(index);
}

void ColumnManager::resize(int index, int size)
{
    if(index < 0 || index >= m_columns.size())
        qWarning("ColumnManager: index is out of range");

   m_columns[index].size = qMax(size, 30);
   //sync();
   emit resized(index);
}

void ColumnManager::move(int from, int to)
{
    if(from < 0 || from >= m_columns.size())
        qWarning("ColumnManager: index is out of range");

    if(to < 0 || to >= m_columns.size())
        qWarning("ColumnManager: index is out of range");
    m_columns.move(from, to);
    emit moved(from, to);
}

void ColumnManager::execEditor(int index, QWidget *parent)
{

}

int ColumnManager::count()
{
    return m_columns.count();
}

const MetaDataFormatter *ColumnManager::titleFormatter(int index) const
{
    if(index < 0 || index >= m_columns.size())
    {
        qWarning("ColumnManager: index is out of range");
        return 0;
    }
    return m_columns[index].titleFormatter;
}

int ColumnManager::size(int index) const
{
    if(index < 0 || index >= m_columns.size())
    {
        qWarning("ColumnManager: index is out of range");
        return 0;
    }
    return m_columns[index].size;
}

const QString ColumnManager::name(int index) const
{
    if(index < 0 || index >= m_columns.size())
    {
        qWarning("ColumnManager: index is out of range");
        return QString();
    }
    return m_columns[index].name;
}
const QString ColumnManager::pattern(int index) const
{
    if(index < 0 || index >= m_columns.size())
    {
        qWarning("ColumnManager: index is out of range");
        return QString();
    }
    return m_columns[index].pattern;
}

void ColumnManager::sync()
{
    QSettings s (Qmmp::configFile(), QSettings::IniFormat);
    s.beginGroup("PlayList");
    int old_count = s.value("column_count", 1).toInt();
    s.setValue("column_count", m_columns.count());
    for(int i = 0; i < m_columns.count(); ++i)
    {
        s.beginGroup(QString("column%1").arg(i));
        Column col = m_columns.at(i);
        s.setValue("name", col.name);
        s.setValue("pattern", col.pattern);
        s.setValue("size", col.size);
        s.endGroup();
    }
    s.setValue("column_count", m_columns.count());
    for(int i = m_columns.count(); i < old_count; ++i)
    {
        s.remove(QString("column%1").arg(i));
    }
    s.endGroup();
}
