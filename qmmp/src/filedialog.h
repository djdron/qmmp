#ifndef FILEDIALOG_H
#define FILEDIALOG_H

#include <QString>
#include <QStringList>

#include <QFileDialog>
#include <QMap>

#define interface struct

interface FileDialogFactory;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////// FILE DIALOG //////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class FileDialog : public QObject
{
Q_OBJECT
    public:
        enum Mode{AddFiles,AddDirs,SaveFiles};
        static QString getExistingDirectory( QWidget * parent = 0, const QString & caption = QString(), const QString & dir = QString(),bool = FALSE);
        static QString getOpenFileName(QWidget * parent = 0,const QString & caption = QString(),const QString & dir = QString(),const QString & filter = QString(),
                    QString * selectedFilter = 0,bool = FALSE);
        static QStringList getOpenFileNames( QWidget * parent = 0, const QString & caption = QString(), const QString & dir = QString(),
                    const QString & filter = QString(), QString * selectedFilter = 0,bool = FALSE);
        static QString getSaveFileName ( QWidget * parent = 0, const QString & caption = QString(),
                const QString & dir = QString(), const QString & filter = QString(), QString * selectedFilter = 0,bool = FALSE);
        static QStringList registeredFactories();

        static bool isModal();
        static void popup(QObject* ,const QString& = QString(),Mode = AddFiles,const QStringList& nameFilters = QStringList());
    signals:
        void filesAdded(const QStringList&);
    protected:
        FileDialog();
        virtual QString existingDirectory( QWidget* , const QString& , const QString& );
        virtual QString openFileName( QWidget* ,const QString& ,const QString& ,const QString& , QString* );
        virtual QStringList openFileNames( QWidget* , const QString& , const QString&  ,const QString& , QString* );
        virtual QString saveFileName ( QWidget* , const QString& ,const QString& , const QString& , QString* );
        virtual bool modal()const{ return TRUE;}
        virtual ~FileDialog(){;}
		  virtual void init(QObject*);
		  virtual void raise(const QString& = QString(),Mode = AddFiles,const QStringList& = QStringList()){;}

        static bool registerFactory(FileDialogFactory*);
    public:
        static void registerBuiltinFactories();
        static void registerExternalFactories();
    protected:
		  static FileDialog* instance();
		  static FileDialog* defaultInstance();
    private:
        static QMap <QString,FileDialogFactory*> factories;
        static FileDialog* _instance;
        static QString m_current_factory;
        bool m_initialized;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




interface FileDialogFactory
{
    virtual FileDialog* create() = 0;
    virtual QString name() = 0;
    virtual ~FileDialogFactory(){;}
};

Q_DECLARE_INTERFACE(FileDialogFactory, "FileDialogFactory/1.0");


//////////////////////////////////////////// QT FILE DIALOG ////////////////////////////////////////////////////////////////////

class QtFileDialog : public FileDialog
{
    public:
        virtual ~QtFileDialog();
        virtual QString existingDirectory(QWidget * parent , const QString & , const QString & dir);
        virtual QString openFileName(QWidget * parent,const QString & caption,const QString & dir,const QString & filter,
                QString * selectedFilter);
        virtual QStringList openFileNames(QWidget * parent, const QString & caption , const QString & dir ,
                const QString & filter, QString * selectedFilter);
        virtual QString saveFileName ( QWidget * parent, const QString & caption,
                const QString & dir, const QString & filter, QString * selectedFilter);
};


class QtFileDialogFactory : public QObject, public FileDialogFactory
{
    Q_OBJECT
    Q_INTERFACES(FileDialogFactory);
    public:
        virtual FileDialog* create();
        virtual QString name();
        virtual ~QtFileDialogFactory(){;}
        static QString QtFileDialogFactoryName;
};


#endif


