/***************************************************************************
 *   Copyright (C) 2006-2013 by Ilya Kotov                                 *
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
#ifndef LISTWIDGET_H
#define LISTWIDGET_H

#include <QWidget>
#include <QDir>
#include <QContextMenuEvent>

class QFont;
class QFontMetrics;
class QMenu;
class QAction;
class QTimer;
class QScrollBar;
class PlayListModel;
class PlayListItem;
class QmmpUiSettings;
namespace PlayListPopup{
class PopupWidget;
}

/**
   @author Ilya Kotov <forkotov02@hotmail.ru>
*/
struct ListWidgetRow
{
    QString title;
    QString length;
    QString extraString;
    int number;
    bool separator;
    bool selected;
};


/**
   @author Ilya Kotov <forkotov02@hotmail.ru>
*/
class ListWidget : public QWidget
{
    Q_OBJECT
public:
    ListWidget(PlayListModel *model, QWidget *parent = 0);

    ~ListWidget();

    void readSettings();
    /*!
     * Returns count of currently visible rows.
     */
    int visibleRows() const;
    /*!
     * Returns index of the first visible item.
     */
    int firstVisibleIndex() const;

    int anchorIndex() const;
    void setAnchorIndex(int index);
    QMenu *menu();
    PlayListModel *model();
    void setMenu(QMenu *menu);

public slots:
    void updateList();
    void scroll(int); //0-99
    void recenterCurrent();

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
    int indexAt(int y) const;
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *);
    void dragMoveEvent(QDragMoveEvent *event);
    void contextMenuEvent (QContextMenuEvent * event);
    bool event (QEvent *e);

private slots:
    void autoscroll();

private:
    void loadSystemColors();
    bool m_update;
    bool m_scroll;
    int m_pressed_index;
    QMenu *m_menu;
    PlayListModel *m_model;
    /*!
     * Returns string with queue number or(and) repeate flag for the item number \b i.
     */
    const QString getExtraString(int i);
    int m_row_count, m_first;
    QFont m_font, m_extra_font;
    QFontMetrics *m_metrics, *m_bold_metrics, *m_extra_metrics;
    QColor m_normal, m_alternate, m_current, m_highlighted, m_normal_bg, m_selected_bg;
    int m_anchor_index;

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
    QmmpUiSettings *m_ui_settings;
    PlayListPopup::PopupWidget *m_popupWidget;
    QTimer *m_timer;
    QScrollBar *m_scrollBar;
    bool m_show_anchor;
    int m_number_width;
    int m_drop_index;
    QList<ListWidgetRow *> m_rows;
    bool m_align_numbres;
    bool m_use_system_colors;
};

#endif
