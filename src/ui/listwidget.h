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
#ifndef LISTWIDGET_H
#define LISTWIDGET_H

#include <QWidget>
#include <QDir>
#include <QContextMenuEvent>

/**
   @author Ilya Kotov <forkotov02@hotmail.ru>
*/
class QFont;
class QFontMetrics;
class QMenu;
class QAction;

class PlayList;
class PlayListModel;
class Skin;
class PlayListItem;
class MediaPlayer;

class ListWidget : public QWidget
{
    Q_OBJECT
public:
    ListWidget(QWidget *parent = 0);

    ~ListWidget();

    void readSettings();
    /*!
     * Returns count of currently visible rows.
     */
    int visibleRows()const
    {
        return m_rows;
    }

    /*!
     * Returns number of first visible row.
     */
    int firstVisibleRow()const
    {
        return m_first;
    }

    int getAnchorRow()const
    {
        return m_anchor_row;
    }

    void setAnchorRow(int r)
    {
        m_anchor_row = r;
    }

    QMenu *menu()
    {
        return m_menu;
    }

    PlayListModel *model()
    {
        Q_ASSERT(!m_model);
        return m_model;
    }


public slots:
    void updateList();
    void scroll(int); //0-99
    void recenterCurrent();
    void setModel(PlayListModel *selected, PlayListModel *previous = 0);


signals:
    void selectionChanged();
    void positionChanged(int, int); //current position, maximum value

protected:
    void paintEvent(QPaintEvent *);
    void mouseDoubleClickEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void resizeEvent(QResizeEvent *);
    void wheelEvent(QWheelEvent *);
    int rowAt(int)const;
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    void contextMenuEvent (QContextMenuEvent * event);
    bool event (QEvent *e);

private slots:
    void updateSkin();

private:
    void loadColors();
    void processFileInfo(const QFileInfo&);
    bool m_update;
    bool m_scroll;
    int m_pressed_row;
    QMenu *m_menu;
    PlayListModel *m_model;
    /*!
     * Returns string with queue number or(and) repeate flag for the item number \b i.
     */
    const QString getExtraString(int i);
    int m_rows, m_first;
    QList <QString> m_titles;
    QList <QString> m_times;
    PlayList *m_pl;
    QFont m_font;
    QFontMetrics *m_metrics;
    Skin *m_skin;
    QColor m_normal, m_current, m_normal_bg, m_selected_bg;
    int m_anchor_row;

    enum ScrollDirection
    {
        NONE = 0,TOP,DOWN
    };

    /*!
     * Scroll direction that is preforming in current moment.
     */
    ScrollDirection m_scroll_direction;
    int m_prev_y;
    bool m_select_on_release;
    bool m_show_protocol;
    bool m_show_number;
    MediaPlayer *m_player;
};

#endif
