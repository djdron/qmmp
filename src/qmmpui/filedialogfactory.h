/***************************************************************************
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
#ifndef FILEDIALOGFACTORY_H
#define FILEDIALOGFACTORY_H

class QObject;
class QTranslator;
class FileDialog;

class FileDialogProperties
{
public:
    FileDialogProperties()
    {
        hasAbout = FALSE;
    }
    bool hasAbout;
    QString name;
    QString shortName;
};


/**
   @author Vladimir Kuznetsov <vovanec@gmail.com>
 */

class FileDialogFactory
{
public:
    virtual ~FileDialogFactory() {};
    virtual FileDialog* create() = 0;
    virtual const FileDialogProperties properties() const = 0;
    virtual void showAbout(QWidget *parent) = 0;
    virtual QTranslator *createTranslator(QObject *parent) = 0;
};

Q_DECLARE_INTERFACE(FileDialogFactory, "FileDialogFactory/1.0");

#endif
