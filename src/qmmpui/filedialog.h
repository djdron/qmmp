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

#ifndef FILEDIALOG_H
#define FILEDIALOG_H

#include <QString>
#include <QStringList>

#include <QFileDialog>
#include <QMap>

#include "filedialogfactory.h"


/**
   @author Vladimir Kuznetsov <vovanec@gmail.com>
 */

class FileDialog : public QObject
{
    Q_OBJECT
public:
    enum Mode{AddFile = 0, AddDir, AddFiles, AddDirs, AddDirsFiles, SaveFile};

    static QString getExistingDirectory(QWidget *parent = 0,
                                        const QString &caption = QString(),
                                        const QString &dir = QString());

    static QString getOpenFileName(QWidget *parent = 0,
                                   const QString &caption = QString(),
                                   const QString &dir = QString(),
                                   const QString &filter = QString(),
                                   QString *selectedFilter = 0);

    static QStringList getOpenFileNames(QWidget *parent = 0,
                                        const QString &caption = QString(),
                                        const QString &dir = QString(),
                                        const QString &filter = QString(),
                                        QString *selectedFilter = 0);

    static QString getSaveFileName (QWidget *parent = 0,
                                    const QString &caption = QString(),
                                    const QString &dir = QString(),
                                    const QString &filter = QString(),
                                    QString *selectedFilter = 0);

    static void popup(QWidget *parent = 0,
                      Mode = AddFiles,
                      QString *dir = 0,
                      QObject *receiver = 0,
                      const char *member = 0,
                      const QString &caption = QString(),
                      const QString &filters = QString());

    static QList <FileDialogFactory*> registeredFactories();
    static bool isModal();
    static void setEnabled(FileDialogFactory *factory);
    static bool isEnabled(FileDialogFactory *factory);


signals:
    void filesAdded(const QStringList&);
protected:
    FileDialog();
    virtual QString existingDirectory(QWidget *parent,
                                      const QString &caption,
                                      const QString &dir);

    virtual QString openFileName( QWidget *parent,
                                  const QString &caption,
                                  const QString &dir,
                                  const QString &filter,
                                  QString *selectedFilter);

    virtual QStringList openFileNames(QWidget *parent,
                                      const QString &caption,
                                      const QString &dir,
                                      const QString &filter,
                                      QString *selectedFilter);

    virtual QString saveFileName ( QWidget *parent ,
                                   const QString &caption,
                                   const QString &dir,
                                   const QString &filter ,
                                   QString *selectedFilter);

    virtual bool modal()const
    {
        return TRUE;
    };

    virtual ~FileDialog()
    {
        ;
    };

    void init(QObject* receiver, const char* member, QString *dir);

    virtual void raise(const QString &dir = QString(),
                       Mode mode = AddFiles,
                       const QString &caption = QString(),
                       const QStringList &mask = QStringList())
    {
        Q_UNUSED(dir);
        Q_UNUSED(mode);
        Q_UNUSED(caption);
        Q_UNUSED(mask);
    }

    static FileDialog* instance();
    static FileDialog* defaultInstance();
    static bool registerFactory(FileDialogFactory *factory);
    static void registerBuiltinFactories();
    static void registerExternalFactories();

private slots:
    void updateLastDir(const QStringList&);

private:
    static QMap <QString,FileDialogFactory*> factories;
    static FileDialog* _instance;
    static QString m_current_factory;
    bool m_initialized;
    QString *m_lastDir;
};

#endif


