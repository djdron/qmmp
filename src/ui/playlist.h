/***************************************************************************
 *   Copyright (C) 2006-2009 by Ilya Kotov                                 *
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

class QMenu;
class QActionGroup;
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
class KeyboardManager;
class PlayListManager;


/**
   @author Ilya Kotov <forkotov02@hotmail.ru>
*/
class PlayList : public QWidget
{
        Q_OBJECT
    public:
        PlayList (PlayListManager *manager, QWidget *parent = 0);

        ~PlayList();
        void load (PlayListItem *);
        void readSettings();
        PlayListItem *currentItem();
        ListWidget* listWidget() const
        {
            return m_listWidget;
        }
        void setMinimalMode(bool b = TRUE);

    signals:
        void play();
        void next();
        void prev();
        void pause();
        void stop();
        void eject();
        void loadPlaylist();
        void savePlaylist();
        void closed();

    public slots:
        void setTime(qint64 time);
        void updateList();

    private slots:
        void showAddMenu();
        void showSubMenu();
        void showSelectMenu();
        void showSortMenu();
        void showPlaylistMenu();
        void updateSkin();
        void select(QAction *a); //selects playlist with action
        void addModel(int i);
        void removeModel(int i);
        void deletePlaylist();

    private:
        void updatePositions();
        QString formatTime (int sec);
        void drawPixmap (int, int);
        void writeSettings();
        void createMenus();
        void createActions();
        QMenu *m_addMenu;
        QMenu *m_subMenu;
        QMenu *m_selectMenu;
        QMenu *m_sortMenu;
        QMenu *m_playlistMenu;
        QWidget *m_resizeWidget;
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
        PlayListTitleBar *m_titleBar;
        PlayListSlider *m_plslider;
        QList <QAction *> m_actions;
        QPoint m_pos;
        bool m_resize;
        bool m_update;
        int m_ratio;
        int m_height;
        bool m_shaded;
        PlayListManager *m_pl_manager;
        KeyboardManager* m_keyboardManager;
        QActionGroup *m_pl_actions;

    protected:
        virtual void paintEvent (QPaintEvent *);
        virtual void resizeEvent (QResizeEvent *);
        virtual void mouseMoveEvent (QMouseEvent *);
        virtual void mousePressEvent (QMouseEvent *);
        virtual void mouseReleaseEvent (QMouseEvent *);
        virtual void changeEvent (QEvent*);
        virtual void closeEvent (QCloseEvent*);
        virtual void keyPressEvent (QKeyEvent*);
};

#endif
