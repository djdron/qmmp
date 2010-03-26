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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qmmp/qmmp.h>
#include <ui_mainwindow.h>

class QSlider;
class QLabel;

class PlayListModel;
class MediaPlayer;
class SoundCore;
class PlayListManager;
class GeneralHandler;

class MainWindow : public QMainWindow
{
Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);

    ~MainWindow();

private slots:
    void addDir();
    void addFiles();
    void updatePosition(qint64 pos);
    void seek();
    void showState(Qmmp::State);
    void showBitrate(int);
    void updateTabs();
    void addPlaylist();
    void removePlaylist();
    void addTab(int);
    void removeTab(int);
    void about();
    void toggleVisibility();

private:
    QString m_lastDir;
    PlayListManager *m_pl_manager;
    Ui::MainWindow ui;
    MediaPlayer *m_player;
    QSlider *m_slider;
    QLabel *m_label;
    SoundCore *m_core;
    GeneralHandler *m_generalHandler;

};

#endif
