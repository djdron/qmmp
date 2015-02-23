/***************************************************************************
 *   Copyright (C) 2015 by Ilya Kotov                                      *
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
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#ifndef LISTWIDGETDRAWER_H
#define LISTWIDGETDRAWER_H

#include <QString>
#include <QColor>
#include <QRect>
#include <QFontMetrics>

class Skin;
class QPainter;

struct ListWidgetRow
{
    ListWidgetRow()
    {
        flags = NO_FLAGS;
    }
    QString title;
    QString length;
    QString extraString;
    int number;
    enum
    {
        NO_FLAGS = 0x00,
        GROUP = 0x01,
        SELECTED = 0x02,
        CURRENT = 0x04,
        ANCHOR = 0x08
    };

    int flags;

    enum
    {
        NUMBER = 0,
        TITLE,
        EXTRA_STRING,
        LENGTH
    };
    int x[LENGTH + 1];
    QRect rect; //geometry
};

/**
   @author Ilya Kotov <forkotov02@hotmail.ru>
*/
class ListWidgetDrawer
{
public:
    ListWidgetDrawer();
    ~ListWidgetDrawer();

    void readSettings();
    void loadColors();
    int rowHeight() const;
    void calculateNumberWidth(int count);
    void prepareRow(ListWidgetRow *row);
    void fillBackground(QPainter *painter, int width, int height);
    void drawBackground(QPainter *painter, ListWidgetRow *row);
    void drawSeparator(QPainter *painter, ListWidgetRow *row, bool rtl);
    void drawTrack(QPainter *painter, ListWidgetRow *row);
    void drawDropLine(QPainter *painter, int row_number, int width);
    void drawVerticalLine(QPainter *painter, int row_count, int width, bool rtl);

private:
    QColor m_normal, m_current, m_normal_bg, m_selected_bg;
    Skin *m_skin;
    QFontMetrics *m_metrics;
    QFontMetrics *m_extra_metrics;
    QFont m_font, m_extra_font;
    bool m_update;
    bool m_show_number;
    bool m_show_anchor;
    bool m_align_numbres;
    int m_number_width;
    int m_row_height;
};

#endif // LISTWIDGETDRAWER_H
