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
#include <QPixmap>

class QFontMetrics;
class QFont;
class QMouseEvent;
class QContextMenuEvent;
class QMenu;
class QAction;
class QStyleOptionHeader;
class PlayListHeaderModel;

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
    int requiredHeight() const;
    QList<int> sizes() const;
    int trackStateColumn() const;

signals:
    void resizeColumnRequest();

public slots:
    void updateColumns();
    void showSortIndicator(int column, bool reverted);
    void hideSortIndicator();

private slots:
    void addColumn();
    void editColumn();
    void removeColumn();
    void setAutoResize(bool yes);
    void showTrackState(bool yes);
    void restoreSize();
    void onColumnAdded(int index);

private:
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *e);
    void resizeEvent(QResizeEvent *e);
    void contextMenuEvent(QContextMenuEvent *e);
    void paintEvent(QPaintEvent *);
    void timerEvent(QTimerEvent *e);
    int findColumn(QPoint pos);
    void initStyleOption(QStyleOptionHeader *opt);
    int size(int index) const;
    void setSize(int index, int size);
    const QString name(int index) const;
    bool adjustColumns();
    void writeSettings();
    void showEvent(QShowEvent *);
    void hideEvent(QHideEvent *);

    enum DataKey
    {
        NAME = 0,
        SIZE,
        RECT, //geometry
        AUTO_RESIZE,
        TRACK_STATE
    };

    QFontMetrics *m_metrics;
    QMenu *m_menu;
    QPoint m_pressed_pos;
    QPoint m_mouse_pos;
    PlayListHeaderModel *m_model;
    QAction *m_trackStateAction;
    QAction *m_autoResizeAction;
    bool m_reverted;
    int m_number_width;
    int m_pressed_column;
    int m_old_size;
    int m_press_offset;
    int m_pl_padding;
    int m_sorting_column;
    bool m_block_resize;
    QSize m_size_hint;
    QList<int> m_old_sizes;

    enum
    {
        NO_TASK = -1,
        RESIZE,
        MOVE,
        SORT
    } m_task;


};

#endif // PLAYLISTHEADER_H
