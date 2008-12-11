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


#ifndef QMMPFILEDIALOG_H
#define QMMPFILEDIALOG_H

#include <qmmpui/filedialog.h>

class QmmpFileDialogImpl;

class QmmpFileDialog : public FileDialog
{
    Q_OBJECT
public:
    QmmpFileDialog();
    virtual ~QmmpFileDialog();
    bool modal()const;

    void raise(const QString &dir = QString(),
               Mode mode = AddFiles,
               const QString &caption = QString(),
               const QStringList &mask = QStringList());

    QString openFileName(QWidget *parent = 0,
                         const QString &caption  = QString(),
                         const QString &dir  = QString(),
                         const QString &filter  = QString(),
                         QString *selectedFilter = 0);

    QStringList openFileNames(QWidget *parent = 0,
                              const QString &caption  = QString(),
                              const QString &dir  = QString(),
                              const QString &filter  = QString(),
                              QString *selectedFilter = 0);

    QString saveFileName (QWidget *parent = 0,
                          const QString &caption  = QString(),
                          const QString &dir  = QString(),
                          const QString &filter   = QString(),
                          QString *selectedFilter = 0);

public slots:
    void handleSelected();

private:
    QmmpFileDialogImpl *m_dialog;
};




class QmmpFileDialogFactory : public QObject, public FileDialogFactory
{
    Q_OBJECT
    Q_INTERFACES(FileDialogFactory);
public:
    virtual FileDialog* create();
    virtual const FileDialogProperties properties() const;
    virtual void showAbout(QWidget*);
    virtual QTranslator *createTranslator(QObject *parent);
    virtual ~QmmpFileDialogFactory()
    {
        ;
    }
};


#endif


