/***************************************************************************
 *   Copyright (C) 2008-2010 by Ilya Kotov                                 *
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

#include <QFileDialog>
#include "qtfiledialog.h"


FileDialog* QtFileDialogFactory::create()
{
    qDebug("QtFileDialogFactory::create()");
    return new QtFileDialog();
}

const FileDialogProperties QtFileDialogFactory::properties() const
{
    FileDialogProperties properties;
    properties.name = tr("Qt File Dialog");
    properties.shortName = "qt_dialog";
    properties.hasAbout = false;
    return properties;
}

void QtFileDialogFactory::showAbout(QWidget*){};

QTranslator *QtFileDialogFactory::createTranslator(QObject *parent)
{
    Q_UNUSED(parent)
    return 0;
}

QtFileDialog::~QtFileDialog()
{
    qDebug("QtFileDialog::~QtFileDialog()");
}

QString QtFileDialog::existingDirectory(QWidget *parent,
                                        const QString &caption,
                                        const QString &dir)
{
    return QFileDialog::getExistingDirectory(parent,caption,dir, QFileDialog::ShowDirsOnly);
}

QString QtFileDialog::openFileName(QWidget *parent,
                                   const QString &caption,
                                   const QString &dir,const QString &filter,
                                   QString *selectedFilter)
{
    Q_UNUSED(selectedFilter);
    return QFileDialog::getOpenFileName(parent,caption,dir,filter);
}

QStringList QtFileDialog::openFileNames(QWidget *parent,
                                        const QString &caption,
                                        const QString &dir,
                                        const QString &filter,
                                        QString *selectedFilter)
{
    return QFileDialog::getOpenFileNames(parent,caption,dir,filter,selectedFilter);
}

QString QtFileDialog::saveFileName (QWidget *parent,
                                    const QString &caption,
                                    const QString &dir,
                                    const QString &filter,
                                    QString *selectedFilter)
{
    return QFileDialog::getSaveFileName(parent,caption,dir,filter,selectedFilter);
}



