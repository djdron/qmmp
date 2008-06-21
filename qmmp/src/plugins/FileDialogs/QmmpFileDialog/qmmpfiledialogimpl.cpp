#include "qmmpfiledialogimpl.h"

#include <QDirModel>
#include <QApplication>
#include <QFileInfo>
#include <QStyle>

QmmpFileDialogImpl::QmmpFileDialogImpl( QWidget * parent, Qt::WindowFlags f) : QDialog(parent,f)
{
    setupUi(this);
    setAttribute(Qt::WA_QuitOnClose, FALSE);
    m_model = new QDirModel(this);
    m_model->setSorting(QDir::Type /*| QDir::Name*/);
    fileListView->setModel(m_model);
    //fileListView->setViewMode(QListView::IconMode);
    listToolButton->setChecked(true);
    upToolButton->setIcon(qApp->style()->standardIcon(QStyle::SP_ArrowUp));
    listToolButton->setIcon(qApp->style()->standardIcon(QStyle::SP_FileDialogListView));
    iconToolButton->setIcon(qApp->style()->standardIcon(QStyle::SP_FileDialogDetailedView));
}

QmmpFileDialogImpl::~QmmpFileDialogImpl()
{
}

void QmmpFileDialogImpl::on_lookInComboBox_activated(const QString&)
{
    qWarning("TODO: %s  %d",__FILE__,__LINE__);
}

void QmmpFileDialogImpl::on_upToolButton_clicked()
{
    fileListView->setRootIndex(m_model->parent(fileListView->rootIndex()));
    lookInComboBox->setEditText(m_model->filePath(fileListView->rootIndex()));
}

void QmmpFileDialogImpl::on_fileListView_doubleClicked(const QModelIndex& ind)
{
    if (ind.isValid())
    {
        QFileInfo info = m_model->fileInfo(ind);
        if (info.isDir())
        {
            fileListView->setRootIndex(ind);
            lookInComboBox->setEditText(m_model->filePath(ind));
        }
        else
        {
            QStringList l;
            l << m_model->filePath(ind);
            emit filesAdded(l);
        }
    }

}

void QmmpFileDialogImpl::on_fileNameLineEdit_returnPressed()
{
    on_addPushButton_clicked();
}

void QmmpFileDialogImpl::on_addPushButton_clicked()
{
    QModelIndexList ml = fileListView->selectionModel()->selectedIndexes();
    QStringList l;
    foreach(QModelIndex i,ml)
    l << m_model->filePath(i);
    qWarning("!!!!!!!!!");
    emit filesAdded(l);
}

void QmmpFileDialogImpl::setModeAndMask(const QString& d,FileDialog::Mode m, const QStringList& mask)
{
    if (m == FileDialog::AddFiles)
    {
        setWindowTitle("Add Files");
        m_model->setFilter(QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot);
        m_model->setNameFilters(mask);
    }
    else if (m == FileDialog::AddDirs)
    {
        setWindowTitle("Add Dirs");
        m_model->setFilter(QDir::AllDirs | QDir::NoDotAndDotDot);
    }
    else
    {
        setWindowTitle("Save File");
        m_model->setFilter(QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot);
        qWarning("To be implemented...");
    }

    m_model->setSorting(QDir::Type);
    fileListView->setRootIndex(m_model->index(d));
    m_model->sort(0);
    lookInComboBox->setEditText(QDir::cleanPath(d));
}

void QmmpFileDialogImpl::on_listToolButton_toggled(bool yes)
{
    if (yes)
    {
        iconToolButton->setChecked(false);
        fileListView->setViewMode(QListView::ListMode);
        fileListView->setGridSize(QSize(-1, -1));
    }
}

void QmmpFileDialogImpl::on_iconToolButton_toggled(bool yes)
{
    if (yes)
    {
        listToolButton->setChecked(false);
        fileListView->setViewMode(QListView::IconMode);
        fileListView->setGridSize(QSize(82, 82));
    }
}

