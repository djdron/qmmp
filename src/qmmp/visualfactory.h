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

#ifndef VISUALFACTORY_H
#define VISUALFACTORY_H

class QObject;
class QWidget;
class QTranslator;
class QDialog;

class Visual;

class VisualProperties
{
public:
    VisualProperties()
    {
        hasAbout = FALSE;
        hasSettings = FALSE;
    }
    QString name;
    QString shortName;
    bool hasAbout;
    bool hasSettings;
};

class VisualFactory
{
public:
    virtual ~VisualFactory() {}
    virtual const VisualProperties properties() const = 0;
    virtual Visual *create(QWidget *parent) = 0;
    virtual QDialog *createConfigDialog(QWidget *parent) = 0;
    virtual void showAbout(QWidget *parent) = 0;
    virtual QTranslator *createTranslator(QObject *parent) = 0;
};

Q_DECLARE_INTERFACE(VisualFactory, "VisualFactory/1.0");

#endif
