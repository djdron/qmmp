#include <QFileSystemModel>
#include <qmmp/metadatamanager.h>
#include "ui_filesystembrowser.h"
#include "filesystembrowser.h"

FileSystemBrowser::FileSystemBrowser(QWidget *parent) :
    QWidget(parent)
{
    m_ui = new Ui::FileSystemBrowser;
    m_ui->setupUi(this);
    m_model = new QFileSystemModel(this);
    m_model->setReadOnly(true);
    m_model->setNameFilterDisables(false);
    m_model->setFilter(QDir::AllDirs | QDir::Files | QDir::NoDot);
    m_ui->listView->setFrameStyle(QFrame::NoFrame);
    m_ui->listView->setDragEnabled(true);
    m_ui->listView->setModel(m_model);
    readSettings();
}

FileSystemBrowser::~FileSystemBrowser()
{
    delete m_ui;
}

void FileSystemBrowser::readSettings()
{
    setCurrentDirectory(QDir::currentPath());
    m_model->setNameFilters(MetaDataManager::instance()->nameFilters());
}

void FileSystemBrowser::on_listView_activated(const QModelIndex &index)
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
        m_ui->listView->setRootIndex(index);
        m_ui->directoryLabel->setText(QDir(QDir::cleanPath(path)).dirName());
    }
    else
        m_ui->directoryLabel->clear();
}
