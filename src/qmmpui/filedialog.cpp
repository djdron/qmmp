/**************************************************************************
*   Copyright (C) 2008-2012 by Ilya Kotov                                 *
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
*   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
***************************************************************************/

#include <QFile>
#include <QSettings>
#include <QTranslator>
#include <QApplication>
#include <QPluginLoader>
#include <QMetaObject>
#include <QLibrary>
#include <qmmp/qmmp.h>
#include "filedialog.h"
#include "qtfiledialog_p.h"


//static functions
FileDialog* FileDialog::m_instance = 0;
QList<FileDialogFactory*> *FileDialog::m_factories = 0;
QHash <FileDialogFactory*, QString> *FileDialog::m_files = 0;
FileDialogFactory *FileDialog::m_currentFactory = 0;

void FileDialog::checkFactories()
{
    if(m_factories)
        return;

    m_factories = new QList<FileDialogFactory *>;
    m_files = new QHash <FileDialogFactory*, QString>;
    m_factories->append(new QtFileDialogFactory);
    QDir pluginsDir (Qmmp::pluginsPath());
    pluginsDir.cd("FileDialogs");
    foreach (QString fileName, pluginsDir.entryList(QDir::Files))
    {
        QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = loader.instance();
        if (loader.isLoaded())
            qDebug("FileDialog: loaded plugin %s", qPrintable(fileName));
        else
            qWarning("FileDialog: %s",qPrintable(loader.errorString()));

        FileDialogFactory *factory = 0;
        if (plugin)
            factory = qobject_cast<FileDialogFactory *>(plugin);

        if (factory)
        {
            m_factories->append(factory);
            m_files->insert(factory, fileName);
            qApp->installTranslator(factory->createTranslator(qApp));
        }
    }
#ifndef Q_OS_WIN32
#if (QT_VERSION >= 0x040500 && QT_VERSION < 0x040600)
    //load native kde dialog
    QStringList paths;
    paths << "/usr/lib/kde4/kio_file.so";
    paths << "/usr/lib64/kde4/kio_file.so";
    paths << "/usr/local/kde4/lib/kde4/kio_file.so";
    foreach(QString path, paths)
    {
        if(QFile::exists(path))
        {
            QLibrary *l = new QLibrary(path, qApp);
            l->load();
            break;
        }
    }
#endif
#endif
}

QList <FileDialogFactory*> *FileDialog::factories()
{
    checkFactories();
    return m_factories;
}

void FileDialog::setEnabled(FileDialogFactory *factory)
{
    checkFactories();
    QSettings settings (Qmmp::configFile(), QSettings::IniFormat);
    settings.setValue("FileDialog", factory->properties().shortName);
}

bool FileDialog::isEnabled(FileDialogFactory *factory)
{
    checkFactories();
    QSettings settings (Qmmp::configFile(), QSettings::IniFormat);
    QString name = settings.value("FileDialog", "qt_dialog").toString();
    return factory->properties().shortName == name;
}

QString FileDialog::file(FileDialogFactory *factory)
{
    checkFactories();
    return m_files->value(factory);
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
    if (!m_currentFactory->properties().modal)
        inst->raise(*dir, m, caption, filters.split(";;"));
    else
    {
        QStringList files;
        if (m == AddFiles || m == AddFile || m == AddDirsFiles)
        {
            QString selectedFilter;
            files = getOpenFileNames(parent, caption, *dir, filters, &selectedFilter);
        }
        else if (m == AddDirs || m == AddDir)
        {
            QString path = getExistingDirectory(parent, caption, *dir);
            if (!path.isEmpty())
                files << path;
        }
        QMetaObject::invokeMethod(inst, "filesAdded", Q_ARG(QStringList, files));
    }
}

FileDialog* FileDialog::instance()
{
    checkFactories();
    FileDialogFactory *selected = 0;

    QSettings settings (Qmmp::configFile(), QSettings::IniFormat);
    QString name = settings.value("FileDialog", "qt_dialog").toString();
    foreach(FileDialogFactory *factory, *m_factories)
    {
        if(factory->properties().shortName == name)
        {
            selected = factory;
            break;
        }
    }

    if(!selected)
        selected = m_factories->at(0);

    if(selected == m_currentFactory && m_instance)
        return m_instance;

    if(m_instance)
    {
        delete m_instance;
        m_instance = 0;
    }

    m_currentFactory = selected;
    m_instance = m_currentFactory->create();
    return m_instance;
}

FileDialog* FileDialog::createDefault()
{
    return m_factories->at(0)->create();
}

//base implementation
FileDialog::FileDialog() : QObject(), m_initialized(false)
{
    m_lastDir = 0;
}

FileDialog::~FileDialog() {}

void FileDialog::raise(const QString &dir, Mode mode, const QString &caption, const QStringList &mask)
{
    Q_UNUSED(dir);
    Q_UNUSED(mode);
    Q_UNUSED(caption);
    Q_UNUSED(mask);
}

QString FileDialog::existingDirectory(QWidget *parent, const QString &caption, const QString &dir)
{
    FileDialog *instance = FileDialog::createDefault();
    QString dir_path = instance->existingDirectory(parent, caption, dir);
    delete instance;
    return dir_path;
}

QString FileDialog::openFileName(QWidget *parent, const QString &caption, const QString &dir,
                                 const QString &filter, QString *selectedFilter)
{
    FileDialog *instance = FileDialog::createDefault();
    QString file_path = instance->openFileName(parent, caption, dir, filter, selectedFilter);
    delete instance;
    return file_path;
}

QStringList FileDialog::openFileNames(QWidget *parent, const QString &caption, const QString &dir,
                                      const QString &filter, QString *selectedFilter)
{
    FileDialog *instance = FileDialog::createDefault();
    QStringList list = instance->openFileNames(parent, caption, dir, filter, selectedFilter);
    delete instance;
    return list;
}

QString FileDialog::saveFileName(QWidget *parent, const QString &caption, const QString &dir,
                                 const QString &filter, QString *selectedFilter)
{
    FileDialog *instance = FileDialog::createDefault();
    QString file_path = instance->saveFileName(parent, caption, dir, filter, selectedFilter);
    delete instance;
    return file_path;
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
