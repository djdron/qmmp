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
#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QWidget>

class KeyboardManager;

/**
   @author Ilya Kotov <forkotov02@hotmail.ru>
*/

class QMenu;

class Skin;
class ListWidget;
class PlayListItem;
class Button;
class PlayListModel;
class PlayListTitleBar;
class PlayListSlider;
class MainWindow;
class SymbolDisplay;
class OutputState;
class PixmapWidget;
class PlaylistControl;

class PlayList : public QWidget
{
        Q_OBJECT
    public:
        PlayList ( QWidget *parent = 0 );

        ~PlayList();
        void load ( PlayListItem * );
        void setModel ( PlayListModel * );
        void readSettings();
        void setInfo ( const OutputState &,int,int );
        PlayListItem *currentItem();
        ListWidget* listWidget() const{return m_listWidget;}

    signals:
        void play();
        void next();
        void prev();
        void pause();
        void stop();
        void eject();
        void loadPlaylist();
        void savePlaylist();
        void newPlaylist();
        void closed();

    private slots:
        void showAddMenu();
        void showSubMenu();
        void showSelectMenu();
        void showSortMenu();
        void showPlaylistMenu();


    private:
        QString formatTime ( int sec );
        void drawPixmap ( int, int );
        void writeSettings();
        void createMenus();
        void createActions();
        QMenu *m_addMenu;
        QMenu *m_subMenu;
        QMenu *m_selectMenu;
        QMenu *m_sortMenu;
        QMenu *m_playlistMenu;
        Button *m_buttonAdd;
        Button *m_buttonSub;
        Button *m_selectButton;
        Button *m_sortButton;
        Button* m_playlistButton;

        PlaylistControl* m_pl_control;
        SymbolDisplay* m_length_totalLength;
        SymbolDisplay* m_current_time;

        Skin *m_skin;
        ListWidget *m_listWidget;
        PlayListModel *m_playListModel;
        PlayListTitleBar *m_titleBar;
        PlayListSlider *m_plslider;
        QList <QAction *> m_actions;
        QPoint m_pos;
        bool m_resize;
        bool m_update;
        int m_anchor_row;
        KeyboardManager* m_keyboardManager;

    protected:
        virtual void paintEvent ( QPaintEvent * );
        virtual void resizeEvent ( QResizeEvent * );
        virtual void mouseMoveEvent ( QMouseEvent * );
        virtual void mousePressEvent ( QMouseEvent * );
        virtual void mouseReleaseEvent ( QMouseEvent * );
        virtual void changeEvent ( QEvent* );
        virtual void closeEvent ( QCloseEvent* );
        virtual void keyPressEvent ( QKeyEvent* );
};

#endif
