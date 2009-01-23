/**************************************************************************
*   Copyright (C) 2008 by Ilya Kotov                                      *
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
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************/

#include "filedialog.h"

#include <QSettings>
#include <QTranslator>
#include <QApplication>
#include <QPluginLoader>
#include <QMetaObject>
#include <qmmp/qmmp.h>

#include "qtfiledialog.h"


FileDialog* FileDialog::_instance = 0;

QMap<QString,FileDialogFactory*> FileDialog::factories = QMap<QString,FileDialogFactory*>();

FileDialog::FileDialog() : QObject(), m_initialized(FALSE)
{
    m_lastDir = 0;
}

bool FileDialog::isModal()
{
    return instance()->modal();
}

void FileDialog::setEnabled(FileDialogFactory *factory)
{
    if (factories.isEmpty())
    {
        registerBuiltinFactories();
        registerExternalFactories();
    }
    QSettings settings (Qmmp::configFile(), QSettings::IniFormat);
    settings.setValue("FileDialog", factories.key(factory));
}

bool FileDialog::isEnabled(FileDialogFactory *factory)
{
    if (factories.isEmpty())
    {
        registerBuiltinFactories();
        registerExternalFactories();
    }
    QSettings settings (Qmmp::configFile(), QSettings::IniFormat);
    QString f_dialogName = settings.value("FileDialog", "qt_dialog").toString();
    return factories.value(f_dialogName) == factory;
}

void FileDialog::init(QObject* receiver, const char* member, QString *dir)
{
    m_lastDir = dir;
    if (m_initialized)
        disconnect();
    if (receiver &&  member)
    {
        connect(this,SIGNAL(filesAdded(const QStringList&)), receiver, member);
        connect(this,SIGNAL(filesAdded(const QStringList&)), SLOT(updateLastDir(const QStringList&)));
        m_initialized = true;
    }
}

QString FileDialog::getExistingDirectory(QWidget *parent,
        const QString &caption,
        const QString &dir)
{
    return instance()->existingDirectory(parent,caption,dir);
}

QString FileDialog::getOpenFileName(QWidget *parent,
                                    const QString &caption,
                                    const QString &dir,
                                    const QString &filter,
                                    QString *selectedFilter)
{
    return instance()->openFileName(parent,caption,dir,filter,selectedFilter);
}

QStringList FileDialog::getOpenFileNames(QWidget *parent, const QString &caption,
        const QString &dir,const QString &filter,
        QString *selectedFilter)
{
    return  instance()->openFileNames(parent,caption,dir,filter,selectedFilter);
}

QString FileDialog::getSaveFileName (QWidget *parent, const QString &caption,
                                     const QString& dir, const QString &filter,
                                     QString *selectedFilter)
{
    return instance()->saveFileName(parent,caption,dir,filter,selectedFilter);
}

//virtual
QString FileDialog::existingDirectory(QWidget *parent, const QString &caption, const QString &dir)
{
    FileDialog *instance = FileDialog::defaultInstance();
    QString dir_path = instance->existingDirectory(parent, caption, dir);
    delete instance;
    return dir_path;
}

QString FileDialog::openFileName(QWidget *parent, const QString &caption, const QString &dir,
                                 const QString &filter, QString *selectedFilter)
{
    FileDialog *instance = FileDialog::defaultInstance();
    QString file_path = instance->openFileName(parent, caption, dir, filter, selectedFilter);
    delete instance;
    return file_path;
}

QStringList FileDialog::openFileNames(QWidget *parent, const QString &caption, const QString &dir,
                                      const QString &filter, QString *selectedFilter)
{
    FileDialog *instance = FileDialog::defaultInstance();
    QStringList list = instance->openFileNames(parent, caption, dir, filter, selectedFilter);
    delete instance;
    return list;
}

QString FileDialog::saveFileName(QWidget *parent, const QString &caption, const QString &dir,
                                 const QString &filter, QString *selectedFilter)
{
    FileDialog *instance = FileDialog::defaultInstance();
    QString file_path = instance->saveFileName(parent, caption, dir, filter, selectedFilter);
    delete instance;
    return file_path;
}


void FileDialog::registerBuiltinFactories()
{
    FileDialogFactory *fct = new QtFileDialogFactory();
    qApp->installTranslator(fct->createTranslator(qApp));
    registerFactory(fct);
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
        {
            if (!registerFactory(fct))
                qDebug("Warning: Plugin with name %s is already registered...",
                       qPrintable(fileName));
            qApp->installTranslator(fct->createTranslator(qApp));
        }
    }
}

bool FileDialog::registerFactory(FileDialogFactory *factory)
{
    if (!factories.contains(factory->properties().shortName))
    {
        factories.insert(factory->properties().shortName, factory);
        return true;
    }
    return false;
}

QString FileDialog::m_current_factory = QString();

FileDialog* FileDialog::instance()
{
    if (factories.isEmpty())
    {
        registerBuiltinFactories();
        registerExternalFactories();
    }

    QSettings settings (Qmmp::configFile(), QSettings::IniFormat);
    QString f_dialogName = settings.value("FileDialog", "qt_dialog").toString();

    QStringList names = factories.keys();

    if(!names.contains(f_dialogName))
        f_dialogName = "qt_dialog";

    if (m_current_factory != f_dialogName || !_instance)
    {
        if (_instance)
        {
            delete _instance;
            _instance = 0;
        }

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
            _instance = factories["qt_dialog"]->create();
    }
    return _instance;
}

FileDialog* FileDialog::defaultInstance()
{
    if (factories.isEmpty())
    {
        registerBuiltinFactories();
        registerExternalFactories();
    }
    return factories["qt_dialog"]->create();
}

QList <FileDialogFactory*> FileDialog::registeredFactories()
{
    if (factories.isEmpty())
    {
        registerBuiltinFactories();
        registerExternalFactories();
    }
    return factories.values();
}

void FileDialog::popup(QWidget *parent,
                       Mode m,
                       QString *dir,
                       QObject *receiver,
                       const char *member,
                       const QString &caption,
                       const QString &filters)

{
    if (!dir)
        qFatal("FileDialog: empty last dir pointer");
    FileDialog* inst = instance();
    inst->setParent(parent);
    inst->init(receiver, member, dir);
    if (!inst->modal())
        inst->raise(*dir, m, caption, filters.split(";;"));
    else
    {
        QStringList files;
        if (m == AddFiles || m == AddFile || m == AddDirsFiles)
            files = getOpenFileNames(parent, caption, *dir, filters);
        else if (m == AddDirs || m == AddDir)
        {
            QString path = getExistingDirectory(parent, caption, *dir);
            if (!path.isEmpty())
                files << path;
        }
        QMetaObject::invokeMethod(inst, "filesAdded", Q_ARG(QStringList, files));
    }
}


void FileDialog::updateLastDir(const QStringList& list)
{
    if (!list.isEmpty() && m_lastDir)
    {
        QString path = list[0];
        if (path.endsWith('/'))
            path.remove(path.size()-1, 1);
        *m_lastDir = path.left(path.lastIndexOf ('/'));
    }
}
