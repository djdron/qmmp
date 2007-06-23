/***************************************************************************
 *   Copyright (C) 2006 by Ilya Kotov                                       *
 *   forkotov02@hotmail.ru                                                     *
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
#ifndef FILETAG_H
#define FILETAG_H


#include <QString>

class FileTag
{
public:
    FileTag() {};

    virtual ~FileTag() {};
    virtual QString title () = 0;
    virtual QString artist () = 0;
    virtual QString album () = 0;
    virtual QString comment () = 0 ;
    virtual QString genre () = 0;
    virtual uint year () = 0;
    virtual uint track () = 0;
    virtual uint length () = 0;
    virtual bool isEmpty () = 0;

};

#endif
