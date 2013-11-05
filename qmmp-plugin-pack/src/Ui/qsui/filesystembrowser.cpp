/***************************************************************************
 *   Copyright (C) 2013 by Ilya Kotov                                      *
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

#include <QFileSystemModel>
#include <QListView>
#include <QVBoxLayout>
#include <QSettings>
#include <qmmp/metadatamanager.h>
#include <qmmp/qmmp.h>
#include "elidinglabel.h"
#include "filesystembrowser.h"

FileSystemBrowser::FileSystemBrowser(QWidget *parent) :
    QWidget(parent)
{
    m_update = false;

    m_listView = new QListView(this);
    m_listView->setFrameStyle(QFrame::NoFrame);
    m_listView->setDragEnabled(true);
    connect(m_listView, SIGNAL(activated(QModelIndex)), SLOT(onListViewActivated(QModelIndex)));

    m_label = new Utils::ElidingLabel(this);
    m_label->setMargin(0);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(m_listView);
    layout->addWidget(m_label);
    setLayout(layout);

    m_model = new QFileSystemModel(this);
    m_model->setReadOnly(true);
    m_model->setNameFilterDisables(false);
    m_model->setFilter(QDir::AllDirs | QDir::Files | QDir::NoDot);
    m_listView->setModel(m_model);
    readSettings();
}

FileSystemBrowser::~FileSystemBrowser()
{
    QSettings settings (Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("Simple");
    settings.setValue("fsbrowser_current_dir", m_model->rootPath());
    settings.endGroup();
}

void FileSystemBrowser::readSettings()
{
    QSettings settings (Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("Simple");
    if(!m_update)
    {
        m_update = true;
        setCurrentDirectory(settings.value("fsbrowser_current_dir",
                                           QDir::homePath()).toString());
    }
    settings.endGroup();
    m_model->setNameFilters(MetaDataManager::instance()->nameFilters());
}

void FileSystemBrowser::onListViewActivated(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    QString name = m_model->fileName(index);

    if(name == "..")
    {
        setCurrentDirectory(m_model->fileInfo(index).absoluteFilePath());
    }
    else if(m_model->isDir(index))
    {
        QFileInfo info = m_model->fileInfo(index);
        if(info.isExecutable() && info.isReadable())
            setCurrentDirectory(m_model->filePath(index));
    }
}

void FileSystemBrowser::setCurrentDirectory(const QString &path)
{
    if(path.isEmpty())
        return;

    QModelIndex index = m_model->setRootPath(path);
    if(index.isValid())
    {
        m_listView->setRootIndex(index);
        m_label->setText(QDir(QDir::cleanPath(path)).dirName());
    }
    else
        m_label->clear();
}
