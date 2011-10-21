#ifndef QMMPFILEDIALOG_H
#define QMMPFILEDIALOG_H

#include <filedialog.h>

class QmmpFileDialogImpl;

class QmmpFileDialog : public FileDialog
{
Q_OBJECT
    public:
        QmmpFileDialog();
        virtual ~QmmpFileDialog();
        virtual bool modal()const;
        virtual void raise(const QString&,Mode = AddFiles,const QStringList& = QStringList());
    public slots:
        void handleSelected();

    private:
        QmmpFileDialogImpl * m_dialog;
};




class QmmpFileDialogFactory : public QObject, public FileDialogFactory
{
    Q_OBJECT
    Q_INTERFACES(FileDialogFactory);
    public:
        virtual FileDialog* create();
        virtual QString name();
        virtual ~QmmpFileDialogFactory(){;}
        static QString QmmpFileDialogFactoryName;
};


#endif


