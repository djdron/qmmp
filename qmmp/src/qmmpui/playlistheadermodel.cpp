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
#include "metadatahelper_p.h"
#include "playlistheadermodel.h"

PlayListHeaderModel::PlayListHeaderModel(QObject *parent) :
    QObject(parent)
{
    m_helper = MetaDataHelper::instance();

    ColumnHeader col;
    col.name = tr("Artist - Title");
    col.pattern = "%if(%p,%p - %t,%t)";
    m_columns.append(col);
    m_helper->setTitleFormats(QStringList() << col.pattern);
}

PlayListHeaderModel::~PlayListHeaderModel()
{
    sync();
    m_columns.clear();
}

void PlayListHeaderModel::insert(int index, const QString &name, const QString &pattern)
{
    if(index < 0 || index > m_columns.size())
    {
        qWarning("ColumnManager: index is out of range");
        return;
    }

    ColumnHeader col;
    col.name = name;
    col.pattern = pattern;
    m_columns.insert(index, col);
    sync();
    emit columnAdded(index);
    emit headerChanged();
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

    sync();
    emit columnRemoved(index);
    emit headerChanged();
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
    emit columnMoved(from, to);
    emit headerChanged();
}

void PlayListHeaderModel::execEdit(int index, QWidget *parent)
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
        emit columnChanged(index);
        emit headerChanged();
    }
}

void PlayListHeaderModel::execInsert(int index, QWidget *parent)
{
    if(index < 0 || index > m_columns.size())
    {
        qWarning("ColumnManager: index is out of range");
        return;
    }

    if(!parent)
        parent = qApp->activeWindow();

    ColumnEditor editor(tr("Title"),"%t",parent);
    editor.setWindowTitle(tr("Add Column"));
    if(editor.exec() == QDialog::Accepted)
        insert(index, editor.name(), editor.pattern());
}

int PlayListHeaderModel::count()
{
    return m_columns.count();
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

void PlayListHeaderModel::sync()
{
    /*QSettings s (Qmmp::configFile(), QSettings::IniFormat);
    s.beginGroup("PlayList");
    int old_count = s.value("column_count", 1).toInt();
    s.setValue("column_count", m_columns.count());
    for(int i = 0; i < m_columns.count(); ++i)
    {
        s.beginGroup(QString("column%1").arg(i));
        ColumnHeader col = m_columns.at(i);
        s.setValue("name", col.name);
        s.setValue("pattern", col.pattern);
        s.endGroup();
    }
    s.setValue("column_count", m_columns.count());
    for(int i = m_columns.count(); i < old_count; ++i)
    {
        s.remove(QString("column%1").arg(i));
    }
    s.endGroup();*/
}
