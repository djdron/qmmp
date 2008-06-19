#include "filedialog.h"

#include <QSettings>


#include <QApplication>
#include <QPluginLoader>


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//#include "playlistmodel.h"

FileDialog* FileDialog::_instance = 0;

QMap<QString,FileDialogFactory*> FileDialog::factories = QMap<QString,FileDialogFactory*>();

FileDialog::FileDialog() : QObject(), m_initialized(false)
{
}


bool FileDialog::isModal()
{
    return instance()->modal();
}


void FileDialog::init(QObject* receiver, const char* member)
{
    if (!m_initialized && !instance()->modal())
    {
        if (receiver &&  member)
        {
            connect(this,SIGNAL(filesAdded(const QStringList&)), receiver, member);
            m_initialized = true;
        }
    }
}



QString FileDialog::getExistingDirectory( QWidget * parent, const QString & caption, const QString & dir,bool def)
{
    QString s;
    if (def)
        s = defaultInstance()->existingDirectory(parent,caption,dir);
    else
        s = instance()->existingDirectory(parent,caption,dir);
    delete _instance;
    _instance = 0;
    return s;
}

QString FileDialog::getOpenFileName(QWidget * parent,
                                    const QString & caption,
                                    const QString & dir,
                                    const QString & filter,
                                    QString * selectedFilter,bool def)
{
    QString s;
    if (def)
        s = defaultInstance()->openFileName(parent,caption,dir,filter,selectedFilter);
    else
        s = instance()->openFileName(parent,caption,dir,filter,selectedFilter);
    delete _instance;
    _instance = 0;
    return s;
}

QStringList FileDialog::getOpenFileNames(QWidget * parent, const QString & caption , const QString & dir ,
        const QString & filter, QString * selectedFilter,bool def)
{
    QStringList sl;
    if (def)
        sl = defaultInstance()->openFileNames(parent,caption ,dir ,filter, selectedFilter);
    else
        sl =  instance()->openFileNames(parent,caption,dir,filter,selectedFilter);
    delete _instance;
    _instance = 0;
    return sl;
}

QString FileDialog::getSaveFileName ( QWidget * parent, const QString & caption,
                                      const QString & dir, const QString & filter, QString * selectedFilter,bool def)
{
    QString s;
    if (def)
        s = defaultInstance()->saveFileName(parent,caption,dir,filter,selectedFilter);
    else
        s =  instance()->saveFileName(parent,caption,dir,filter,selectedFilter);
    delete _instance;
    _instance = 0;
    return s;
}


QString FileDialog::existingDirectory(QWidget *, const QString &, const QString &)
{
    return QString();
}

QString FileDialog::openFileName(QWidget *, const QString &, const QString &, const QString &, QString *)
{
    return QString();
}

QStringList FileDialog::openFileNames(QWidget *, const QString &, const QString &, const QString &, QString *)
{
    return QStringList();
}

QString FileDialog::saveFileName(QWidget *, const QString &, const QString &, const QString &, QString *)
{
    return QString();
}


void FileDialog::registerBuiltinFactories()
{
    registerFactory(new QtFileDialogFactory());
    //registerFactory(new QmmpFileDialogFactory());
}

void FileDialog::registerExternalFactories()
{
    QDir pluginsDir (qApp->applicationDirPath());
    pluginsDir.cdUp();
    pluginsDir.cd("./"LIB_DIR"/qmmp/FileDialogs");

    foreach (QString fileName, pluginsDir.entryList(QDir::Files))
    {
        QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = loader.instance();
        if (loader.isLoaded())
            qDebug("FileDialog: plugin loaded - %s", qPrintable(fileName));
        else
            qDebug("FileDialog: %s",qPrintable(loader.errorString()));

        FileDialogFactory *fct = 0;
        if (plugin)
            fct = qobject_cast<FileDialogFactory *>(plugin);

        if (fct)
            if (!registerFactory(fct))
                qDebug("Warning: Plugin with name %s is already registered...",
                       qPrintable(fct->name()));
    }
}

bool FileDialog::registerFactory(FileDialogFactory *f)
{
    QString name = f->name();
    if (!factories.contains(name))
    {
        factories.insert(name,f);
        return true;
    }
    return false;
}

QString FileDialog::m_current_factory = QString();

FileDialog* FileDialog::instance()
{
    if (_instance && _instance->modal())
    {
        delete _instance;
        _instance = 0;
    }

    if (factories.isEmpty())
    {
        registerBuiltinFactories();
        registerExternalFactories();
    }

    QSettings settings ( QDir::homePath() +"/.qmmp/qmmprc", QSettings::IniFormat );
    QString f_dialogName =
        settings.value("FileDialog",QtFileDialogFactory::QtFileDialogFactoryName).toString();

    QStringList names = factories.keys();

    if (m_current_factory != f_dialogName || !_instance)
    {
        if (_instance)
            delete _instance;

        foreach(QString name,names)
        {
            if (name == f_dialogName)
            {
                _instance = factories[name]->create();
                m_current_factory = f_dialogName;
                break;
            }
        }

        if (!_instance)
            _instance = factories[QtFileDialogFactory::QtFileDialogFactoryName]->create();
    }
    //else if(!_instance->modal())
    //return _instance;
    //      _instance->raise();
    return _instance;

}

FileDialog* FileDialog::defaultInstance()
{
    if (_instance)
    {
        delete _instance;
        _instance = 0;
    }

    if (factories.isEmpty())
    {
        registerBuiltinFactories();
        registerExternalFactories();
    }

    _instance = factories[QtFileDialogFactory::QtFileDialogFactoryName]->create();

    return _instance;
}

QStringList FileDialog::registeredFactories()
{
    if (factories.isEmpty())
    {
        registerBuiltinFactories();
        registerExternalFactories();
    }
    return factories.keys();
}


void FileDialog::popup(const QString& d,Mode m,const QStringList& f, QObject* receiver, const char* member)
{
    FileDialog* inst = instance();
    inst->init(receiver, member);
    inst->raise(d,m,f);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



QtFileDialog::~QtFileDialog()
{
    qWarning("QtFileDialog::~QtFileDialog()");
}

QString QtFileDialog::existingDirectory(QWidget * parent, const QString & caption, const QString & dir)
{
    return QFileDialog::getExistingDirectory(parent,caption,dir,QFileDialog::DontResolveSymlinks | QFileDialog::ShowDirsOnly);
}

QString QtFileDialog::openFileName(QWidget * parent,const QString & caption,const QString & dir,const QString & filter,
                                   QString * selectedFilter)
{
    return QFileDialog::getOpenFileName(parent,caption,dir,filter);
}

QStringList QtFileDialog::openFileNames(QWidget * parent, const QString & caption , const QString & dir ,
                                        const QString & filter, QString * selectedFilter)
{
    return QFileDialog::getOpenFileNames(parent,caption,dir,filter,selectedFilter);
}

QString QtFileDialog::saveFileName ( QWidget * parent, const QString & caption,
                                     const QString & dir, const QString & filter, QString * selectedFilter)
{
    return QFileDialog::getSaveFileName(parent,caption,dir,filter,selectedFilter);
}






FileDialog* QtFileDialogFactory::create()
{
    return new QtFileDialog();
}

QString QtFileDialogFactory::name()
{
    return QtFileDialogFactoryName;
}

QString QtFileDialogFactory::QtFileDialogFactoryName = "Qt File Dialog";


