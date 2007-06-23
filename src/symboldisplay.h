/***************************************************************************
 *   Copyright (C) 2006 by Ilya Kotov                                      *
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
#ifndef SYMBOLDISPLAY_H
#define SYMBOLDISPLAY_H

#include <QPixmap>

#include "pixmapwidget.h"

/**
   @author Vladimir Kuznetsov <vovanec@gmail.com>
 */

class Skin;

class SymbolDisplay : public PixmapWidget
{
    Q_OBJECT
public:
    SymbolDisplay(QWidget *parent = 0, int digits = 3);

    ~SymbolDisplay();
    void display(const QString&);
    void display(int);
    void setAlignment(Qt::Alignment a)
    {
        m_alignment = a;
    }
    Qt::Alignment alignment()const
    {
        return m_alignment;
    }

private slots:
    void draw();

private:
    Skin* m_skin;
    QPixmap m_pixmap;
    int m_digits;
    QString m_text;
    Qt::Alignment m_alignment;
    int m_max;

};

#endif
