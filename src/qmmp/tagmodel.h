/***************************************************************************
 *   Copyright (C) 2009 by Ilya Kotov                                      *
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

#ifndef TAGMODEL_H
#define TAGMODEL_H

#include <QString>
#include <QList>
#include "qmmp.h"

class TagModel
{
public:
    enum Caps
    {
        NoOptions = 0x0,
        CreateRemove = 0x1,
        Save = 0x2,
    };
    TagModel(int f = TagModel::CreateRemove | TagModel::Save);

    virtual const QString name() = 0;
    virtual QList<Qmmp::MetaData> keys();
    virtual const QString value(Qmmp::MetaData key) = 0;
    virtual void setValue(Qmmp::MetaData key, const QString &value) = 0;
    virtual void setValue(Qmmp::MetaData key, int value);
    virtual bool exists();
    virtual void create();
    virtual void remove();
    virtual void save();
    int caps();

private:
    int m_f;
};

#endif // TAGMODEL_H
