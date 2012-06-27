/***************************************************************************
 *   Copyright (C) 2006-2012 by Ilya Kotov                                 *
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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPointer>
#include <qmmp/output.h>
#include <qmmp/decoder.h>
#include <qmmp/decoderfactory.h>
#include <qmmpui/playlistitem.h>
#include "display.h"
#include "titlebar.h"

class PlayList;
class PlayListManager;
class EqWidget;
class MainVisual;
class Skin;
class SoundCore;
class VisualMenu;
class UiHelper;
class MediaPlayer;
class QMenu;
class QKeyEvent;

/**
   @author Ilya Kotov <forkotov02@hotmail.ru>
*/
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);

    ~MainWindow();

    PlayList *playlist(){return m_playlist;}

    QMenu* menu();
    void setVolume(int volume, int balance);
    MainDisplay* mainDisplay()const;

public slots:
    void previous();
    void play();
    void pause();
    void playPause();
    void stop();
    void next();
    void replay();
    void jumpToTrack();
    void toggleVisibility();

    void addDir();
    void addFile();
    void addUrl();

    void loadPlaylist();
    void savePlaylist();
    void about();
    void updateSettings();

protected:
    virtual void closeEvent (QCloseEvent *);
    virtual void changeEvent (QEvent *event);
    virtual void keyPressEvent (QKeyEvent* );

private slots:
    void showState(Qmmp::State state);
    void showMetaData();
    void showSettings();
    void forward();
    void backward();

private:
    void readSettings();
    void writeSettings();
    void createActions();
    SoundCore *m_core;
    QMenu *m_mainMenu;
    MainDisplay *m_display;
    PlayList *m_playlist;
    PlayListManager *m_pl_manager;
    QPointer <PlayListModel> m_model;
    int m_preamp;
    EqWidget *m_equalizer;
    MainVisual *m_vis;
    bool m_update;
    Skin *m_skin;
    bool m_hideOnClose, m_startHidden;
    VisualMenu *m_visMenu;
    UiHelper *m_uiHelper;
    MediaPlayer *m_player;
};

#endif