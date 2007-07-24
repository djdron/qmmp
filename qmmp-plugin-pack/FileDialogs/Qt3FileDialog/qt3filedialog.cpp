#include "qt3filedialog.h"
#include <Q3FileDialog>

Qt3SupportFileDialog::~Qt3SupportFileDialog()
{
    qWarning("Qt3SupportFileDialog::~Qt3SupportFileDialog()");
}

QString Qt3SupportFileDialog::existingDirectory(QWidget * parent , const QString & caption, const QString & dir)
{
    return Q3FileDialog::getExistingDirectory ( dir,
            parent,
            0,
            caption,
            true,
            true );
}

QString Qt3SupportFileDialog::openFileName(QWidget * parent,const QString & caption,const QString & /*dir*/,const QString & filter,
                QString * selectedFilter)
{
          return Q3FileDialog::getOpenFileName ( QString(),filter,parent,0,caption,selectedFilter);
}

QStringList Qt3SupportFileDialog::openFileNames(QWidget * parent, const QString & caption , const QString & dir ,
                const QString & filter, QString * selectedFilter)
{
     return Q3FileDialog::getOpenFileNames(filter,dir,parent,0,caption,selectedFilter);
}

QString Qt3SupportFileDialog::saveFileName ( QWidget * parent, const QString & caption,
        const QString &, const QString & filter, QString * selectedFilter)
{
        return Q3FileDialog::getSaveFileName(QString(),filter,parent,0,caption,selectedFilter);
}
