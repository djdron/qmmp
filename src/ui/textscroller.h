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
#ifndef TEXTSCROLLER_H
#define TEXTSCROLLER_H

#include <pixmapwidget.h>

/**
	@author Ilya Kotov <forkotov02@hotmail.ru>
*/

class QTimer;

class Skin;

class TextScroller : public PixmapWidget
{
Q_OBJECT
public:
    TextScroller(QWidget *parent = 0);

    ~TextScroller();

    static TextScroller *getPointer();
    void setText(const QString&);
    void readSettings();

private slots:
    void addOffset();
    void updateSkin();

protected:
    void hideEvent ( QHideEvent *);
    void showEvent ( QShowEvent *);

private:
    bool m_update;
    static TextScroller *pointer;
    QPixmap m_pixmap;
    int x;
    QFont m_font;
    QFontMetrics *m_metrics;
    QString m_text;
    Skin *m_skin;
    QColor m_color;
    QTimer *m_timer;
};

#endif
