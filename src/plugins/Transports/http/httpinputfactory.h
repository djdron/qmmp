/***************************************************************************
 *   Copyright (C) 2009-2010 by Ilya Kotov                                 *
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

#ifndef HTTPINPUTFACTORY_H
#define HTTPINPUTFACTORY_H

#include <QObject>
#include <QStringList>
#include <qmmp/inputsourcefactory.h>

class QTranslator;

/*!
 * @author Ilya Kotov <forkotov02@hotmail.ru>
 */
class HTTPInputFactory : public QObject, InputSourceFactory
{
Q_OBJECT
Q_INTERFACES(InputSourceFactory);
public:
    const InputSourceProperties properties() const;
    InputSource *create(const QString &url, QObject *parent = 0);
    void showSettings(QWidget *parent);
    void showAbout(QWidget *parent);
    QTranslator *createTranslator(QObject *parent);
};

#endif // HTTPINPUTFACTORY_H
