#include <QFileSystemModel>
#include <qmmp/metadatamanager.h>
#include "filesystembrowser.h"

FileSystemBrowser::FileSystemBrowser(QWidget *parent) :
    QListView(parent)
{
    m_model = new QFileSystemModel(this);
    m_model->setReadOnly(true);
    //m_model->setNameFilterDisables(false);
    m_model->setRootPath(QDir::currentPath());
    m_model->setFilter(QDir::AllDirs | QDir::Files | QDir::NoDot);
    setModel(m_model);
    setRootIndex(m_model->index(QDir::currentPath()));

    connect(this, SIGNAL(doubleClicked(QModelIndex)), SLOT(onDoubleClicked(QModelIndex)));
    /*setColumnHidden(1, true);
    setColumnHidden(2, true);
    setColumnHidden(3, true);
    setHeaderHidden(true);*/
    readSettings();
}

void FileSystemBrowser::readSettings()
{
    m_model->setNameFilters(MetaDataManager::instance()->nameFilters());
}

void FileSystemBrowser::onDoubleClicked(const QModelIndex &ind)
{
    if (ind.isValid())
    {
        QFileInfo info = m_model->fileInfo(ind);
        if (info.isDir())
        {
            qDebug("root=%s", qPrintable(QFileInfo(m_model->filePath(ind)).absolutePath()));
            setRootIndex(m_model->parent(ind));
            //lookInComboBox->setEditText(m_model->filePath(ind));
            //fileListView->selectionModel()->clear ();
            setRootIndex(ind);
            //treeView->selectionModel()->clear ();
            m_model->setRootPath(QFileInfo(m_model->filePath(ind)).absolutePath());
        }
        else
        {
            /*QStringList l;
            l << m_model->filePath(ind);
            addToHistory(l[0]);
            addFiles(l);*/
        }
    }
}
