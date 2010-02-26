/***************************************************************************
 *   Copyright (C) 2009-2010 by Ilya Kotov                                 *
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

#ifndef PLAYLISTSELECTOR_H
#define PLAYLISTSELECTOR_H

#include <QWidget>
#include <QColor>
#include <QPixmap>

class QFontMetrics;
class QFont;
class QMouseEvent;
class QMenu;
class PlayListManager;
class Skin;

/**
   @author Ilya Kotov <forkotov02@hotmail.ru>
*/
class PlayListSelector : public QWidget
{
Q_OBJECT
public:
    PlayListSelector(PlayListManager *manager, QWidget *parent = 0);
    ~PlayListSelector();
    void readSettings();

private slots:
    void updateTabs();
    void updateSkin();
    void renamePlaylist();

private:
    void paintEvent(QPaintEvent *);
    void mousePressEvent (QMouseEvent *e);
    void mouseDoubleClickEvent (QMouseEvent *e);
    void resizeEvent (QResizeEvent *);
    void updateOffsets();
    void loadColors();
    void drawButtons();
    void updateScrollers();
    int firstVisible();
    int lastVisible();
    PlayListManager *m_pl_manager;
    QFontMetrics *m_metrics;
    QFont m_font;
    QMenu *m_menu;
    bool m_update;
    bool m_scrollable;
    QList <QRect> m_rects;
    Skin *m_skin;
    QColor m_normal, m_current, m_normal_bg, m_selected_bg;
    QPixmap m_pixmap;
    bool m_showButtons;
    int m_offset, m_offset_max;
};

#endif // PLAYLISTSELECTOR_H
