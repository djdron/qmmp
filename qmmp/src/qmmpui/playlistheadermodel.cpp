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
#include <QApplication>
#include <qmmp/qmmp.h>
#include "columneditor_p.h"
#include "playlistheadermodel.h"

PlayListHeaderModel::PlayListHeaderModel(QObject *parent) :
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
        col.autoResize = s.value("autoresize", false).toBool();
        m_columns.append(col);
        s.endGroup();
    }
    s.endGroup();
}

PlayListHeaderModel::~PlayListHeaderModel()
{
    sync();
    foreach (Column col, m_columns)
    {
        delete col.titleFormatter;
        col.titleFormatter = 0;
    }
    m_columns.clear();
}

void PlayListHeaderModel::insert(int index, const QString &name, const QString &pattern)
{
    if(index < 0 || index > m_columns.size())
    {
        qWarning("ColumnManager: index is out of range");
        return;
    }

    Column col;
    col.name = name;
    col.pattern = pattern;
    col.titleFormatter = new MetaDataFormatter(pattern);
    col.size = 50;
    col.autoResize = false;
    m_columns.insert(index, col);
    sync();
    emit inserted(index);
}

void PlayListHeaderModel::remove(int index)
{
    if(index < 0 || index >= m_columns.size())
    {
        qWarning("ColumnManager: index is out of range");
        return;
    }

    if(m_columns.count() == 1)
        return;

    delete m_columns.takeAt(index).titleFormatter;
    sync();
    emit removed(index);
}

void PlayListHeaderModel::resize(int index, int size)
{
    if(index < 0 || index >= m_columns.size())
    {
        qWarning("ColumnManager: index is out of range");
        return;
    }

   m_columns[index].size = qMax(size, 30);
   //sync();
   emit resized(index);
}

void PlayListHeaderModel::setAutoResize(int index, bool autoResize)
{
    if(index < 0 || index >= m_columns.size())
    {
        qWarning("ColumnManager: index is out of range");
        return;
    }

    if(m_columns[index].autoResize != autoResize)
    {
        m_columns[index].autoResize = autoResize;
        emit changed(index);
    }
}

void PlayListHeaderModel::move(int from, int to)
{
    if(from < 0 || from >= m_columns.size())
    {
        qWarning("ColumnManager: index is out of range");
        return;
    }

    if(to < 0 || to >= m_columns.size())
    {
        qWarning("ColumnManager: index is out of range");
        return;
    }

    m_columns.move(from, to);
    emit moved(from, to);
}

void PlayListHeaderModel::execEditor(int index, QWidget *parent)
{
    if(index < 0 || index >= m_columns.size())
    {
        qWarning("ColumnManager: index is out of range");
        return;
    }

    if(!parent)
        parent = qApp->activeWindow();

    ColumnEditor editor(m_columns[index].name, m_columns[index].pattern, parent);
    if(editor.exec() == QDialog::Accepted)
    {
        m_columns[index].name = editor.name();
        m_columns[index].pattern = editor.pattern();
        m_columns[index].titleFormatter->setPattern(editor.pattern());
        emit changed(index);
    }
}

int PlayListHeaderModel::count()
{
    return m_columns.count();
}

const MetaDataFormatter *PlayListHeaderModel::titleFormatter(int index) const
{
    if(index < 0 || index >= m_columns.size())
    {
        qWarning("ColumnManager: index is out of range");
        return 0;
    }
    return m_columns[index].titleFormatter;
}

int PlayListHeaderModel::size(int index) const
{
    if(index < 0 || index >= m_columns.size())
    {
        qWarning("ColumnManager: index is out of range");
        return 0;
    }
    return m_columns[index].size;
}

const QString PlayListHeaderModel::name(int index) const
{
    if(index < 0 || index >= m_columns.size())
    {
        qWarning("ColumnManager: index is out of range");
        return QString();
    }
    return m_columns[index].name;
}
const QString PlayListHeaderModel::pattern(int index) const
{
    if(index < 0 || index >= m_columns.size())
    {
        qWarning("ColumnManager: index is out of range");
        return QString();
    }
    return m_columns[index].pattern;
}

QList<int> PlayListHeaderModel::autoResizeColumns() const
{
    QList<int> columns;
    for(int i = 0; i < m_columns.count(); ++i)
    {
        if(m_columns[i].autoResize)
            columns.append(i);
    }
    return columns;
}

void PlayListHeaderModel::sync()
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
        s.setValue("autoresize", col.autoResize);
        s.endGroup();
    }
    s.setValue("column_count", m_columns.count());
    for(int i = m_columns.count(); i < old_count; ++i)
    {
        s.remove(QString("column%1").arg(i));
    }
    s.endGroup();
}
