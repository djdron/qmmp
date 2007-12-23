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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>

#include "output.h"
#include "decoder.h"
#include "display.h"
#include "mediafile.h"
#include "decoderfactory.h"
#include "titlebar.h"

/**
   @author Ilya Kotov <forkotov02@hotmail.ru>
*/
class PlayList;
class PlayListModel;
class ConfigDialog;
class EqWidget;
class MainVisual;
class Skin;
class SoundCore;
class JumpToTrackDialog;
class VisualMenu;

class QMenu;
class QKeyEvent;
class CommandLineOptionManager;


class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(const QStringList& args,CommandLineOptionManager*, QWidget *parent);

    ~MainWindow();

    PlayList *getPLPointer(){return m_playlist;}

    void seek(int);
    QMenu* menu();
    void setVolume(int volume, int balance);
    SoundCore* soundCore()const;
    MainDisplay* mainDisplay()const;
    bool processCommandArgs(const QStringList &list,const QString& cwd);
public slots:
    void previous();
    void play();
    void pause();
    void playPause();
    void stop();
    void next();
    void replay();
    void jumpToFile();
    void toggleVisibility();

    void addDir();
    void addFile();
    void addUrl();

    void newPlaylist();
    void loadPlaylist();
    void savePlaylist();

    void setFileList(const QStringList&);

protected:
    virtual  void closeEvent ( QCloseEvent *);
    virtual  void changeEvent ( QEvent * event );
    virtual void keyPressEvent ( QKeyEvent* );

private slots:
    void showOutputState(const OutputState&);
    void showDecoderState(const DecoderState&);
    void changeTitle(const QString&);
    void clear();
    void startSeek();
    void endSeek();
    void showSettings();
    void updateEQ();
    void updateSkin();
    void forward();
    void backward();
    void trayActivated(QSystemTrayIcon::ActivationReason);
    void about();
    void handleCloseRequest();

private:
    void readSettings();
    void writeSettings();
    void createActions();
    bool seeking;
    SoundCore *m_core;
    QMenu *m_mainMenu;
    MainDisplay *display;
    PlayList *m_playlist;
    PlayListModel *m_playListModel;
    TitleBar *m_titlebar;
    ConfigDialog *m_confDialog;
    int m_preamp;
    EqWidget *m_equalizer;
    MainVisual *m_vis;
    QString m_lastDir;
    QSystemTrayIcon *m_tray;
    bool m_update;
    bool m_showMessage;
    int m_messageDelay;
    bool m_paused;
    bool m_showToolTip;
    Skin *m_skin;
    QString m_playlistName;
    JumpToTrackDialog* m_jumpDialog;
    bool m_hide_on_titlebar_close;
    int m_elapsed;
    VisualMenu *m_visMenu;
    CommandLineOptionManager* m_option_manager;
};

#endif
