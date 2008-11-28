/***************************************************************************
 *   Copyright (C) 2006-2008 by Ilya Kotov                                 *
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

#include <qmmp/output.h>
#include <qmmp/decoder.h>
#include <qmmp/decoderfactory.h>

#include "display.h"
#include "playlistitem.h"
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
class GeneralHandler;

class QMenu;
class QKeyEvent;
class BuiltinCommandLineOption;


class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(const QStringList& args, BuiltinCommandLineOption*, QWidget *parent);

    ~MainWindow();

    PlayList *getPLPointer(){return m_playlist;}

    QMenu* menu();
    void setVolume(int volume, int balance);
    SoundCore* soundCore()const;
    MainDisplay* mainDisplay()const;
    bool processCommandArgs(const QStringList &list,const QString& cwd);

public slots:
    void seek(int);
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
    void showState(Qmmp::State state);
    void showMetaData();
    void clear();
    void startSeek();
    void endSeek();
    void showSettings();
    void updateEQ();
    void forward();
    void backward();
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
    bool m_update;
    bool m_showMessage;
    int m_messageDelay;
    bool m_paused;
    bool m_showToolTip;
    Skin *m_skin;
    QString m_playlistName;
    JumpToTrackDialog* m_jumpDialog;
    bool m_hideOnClose, m_startHidden;
    int m_elapsed;
    VisualMenu *m_visMenu;
    BuiltinCommandLineOption* m_option_manager;
    GeneralHandler *m_generalHandler;
};

#endif
