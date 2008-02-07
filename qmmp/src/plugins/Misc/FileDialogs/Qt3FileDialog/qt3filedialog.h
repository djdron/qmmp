#ifndef QT3FILEDIALOG_H
#define QT3FILEDIALOG_H

#include <filedialog.h>


class Qt3SupportFileDialog : public FileDialog
{
    public:
        virtual ~Qt3SupportFileDialog();
        virtual QString existingDirectory(QWidget * parent , const QString & , const QString & dir);
        virtual QString openFileName(QWidget * parent,const QString & caption,const QString & dir,const QString & filter,
                QString * selectedFilter);
        virtual QStringList openFileNames(QWidget * parent, const QString & caption , const QString & dir ,
                const QString & filter, QString * selectedFilter);
        virtual QString saveFileName ( QWidget * parent, const QString & caption,
                const QString & dir, const QString & filter, QString * selectedFilter);

};

#endif


