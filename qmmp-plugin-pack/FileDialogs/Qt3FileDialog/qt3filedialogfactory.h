#ifndef QT3FILEDIALOGFACTORY_H
#define  QT3FILEDIALOGFACTORY_H


#include <filedialog.h>

class Qt3SupportFileDialogFactory : public QObject, public FileDialogFactory
{
    Q_OBJECT
    Q_INTERFACES(FileDialogFactory);
    public:
        virtual FileDialog* create();
        virtual QString name();
        virtual ~Qt3SupportFileDialogFactory(){;}
        static QString Qt3SupportFileDialogFactoryName;
};

#endif

