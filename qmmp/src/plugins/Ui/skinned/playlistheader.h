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

#ifndef PLAYLISTHEADER_H
#define PLAYLISTHEADER_H

#include <QWidget>

class QFontMetrics;
class QFont;
class QMouseEvent;
class QContextMenuEvent;
class QMenu;
class Skin;
class ColumnManager;

/**
    @author Ilya Kotov <forkotov02@hotmail.ru>
*/
class PlayListHeader : public QWidget
{
    Q_OBJECT
public:
    explicit PlayListHeader(QWidget *parent = 0);
    ~PlayListHeader();

    void readSettings();
    void setNumberWidth(int width);
    void updateColumns();

private slots:
    void updateSkin();
    void addColumn();
    void editColumn();
    void removeColumn();

private:
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *e);
    void resizeEvent(QResizeEvent *);
    void contextMenuEvent(QContextMenuEvent *e);
    void paintEvent(QPaintEvent *);
    void loadColors();
    int findColumn(QPoint pos);

    Skin *m_skin;
    QFontMetrics *m_metrics;
    QMenu *m_menu;
    QFont m_font;
    QColor m_normal, m_normal_bg, m_current;
    QList <QRect> m_rects;
    QStringList m_names;
    QPoint m_pressed_pos;
    QPoint m_mouse_pos;
    ColumnManager *m_manager;
    bool m_show_number;
    bool m_align_numbres;
    int m_number_width;
    int m_pressed_column;
    int m_old_size;
    int m_press_offset;

    enum
    {
        NO_TASK = -1,
        RESIZE,
        MOVE
    } m_task;


};

#endif // PLAYLISTHEADER_H
