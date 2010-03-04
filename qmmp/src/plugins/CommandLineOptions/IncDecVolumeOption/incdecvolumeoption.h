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

#ifndef IncDecVolumeCommandLineOption_H
#define IncDecVolumeCommandLineOption_H

#include <QString>
#include <QObject>
#include <QStringList>
#include <qmmpui/commandlineoption.h>
#include <qmmpui/commandlinemanager.h>


class IncDecVolumeCommandLineOption : public QObject, public CommandLineOption
{
Q_OBJECT
Q_INTERFACES(CommandLineOption)
public:
    virtual bool identify(const QString& opt_str)const;
    virtual const QString name()const;
    virtual const QString helpString()const;
    virtual void executeCommand(const QString& opt_str, const QStringList &args);
    virtual QTranslator *createTranslator(QObject *parent);
};

#endif

