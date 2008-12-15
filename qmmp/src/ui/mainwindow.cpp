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
#include <QtGui>
#include <QFileDialog>
#include <QDir>
#include <QAction>
#include <QMenu>

#include <math.h>

#include <qmmp/soundcore.h>
#include <qmmp/visual.h>
#include <qmmpui/generalhandler.h>
#include <qmmpui/general.h>
#include <qmmpui/playlistparser.h>
#include <qmmpui/playlistformat.h>
#include <qmmpui/commandlinemanager.h>
#include <qmmpui/filedialog.h>
#include <qmmpui/fileloader.h>
#include <qmmpui/playlistmodel.h>
#include <qmmpui/mediaplayer.h>

#include "textscroller.h"
#include "mainwindow.h"
#include "skin.h"
#include "playlist.h"
#include "configdialog.h"
#include "dock.h"
#include "eqwidget.h"
#include "mainvisual.h"
#include "jumptotrackdialog.h"
#include "aboutdialog.h"
#include "addurldialog.h"
#include "listwidget.h"
#include "visualmenu.h"
#include "builtincommandlineoption.h"

#define KEY_OFFSET 10

MainWindow::MainWindow(const QStringList& args, BuiltinCommandLineOption* option_manager, QWidget *parent)
        : QMainWindow(parent)
{
    m_vis = 0;
    seeking = FALSE;
    m_update = FALSE;
    m_paused = FALSE;
    m_playlistName = tr("Default");
    m_option_manager = option_manager;
    setWindowIcon(QIcon(":/32x32/qmmp.png"));
    setWindowFlags(Qt::FramelessWindowHint);
    setFixedSize (275,116);

    //prepare libqmmp and libqmmpui libraries for playing
    m_player = new MediaPlayer(this);
    m_core = new SoundCore(this);
    m_playListModel = new PlayListModel(this);
    m_player->initialize(m_core, m_playListModel);
    //additional featuries
    new PlaylistParser(this);
    m_generalHandler = new GeneralHandler(this);

    //user interface
    m_skin = new Skin(this);
    Dock *dock = new Dock(this);
    dock->setMainWidget(this);
    display = new MainDisplay(this);
    setCentralWidget(display);
    display->show();
    display->setFocus ();

    m_playlist = new PlayList(this);
    m_playlist->setModel(m_playListModel);
    dock->addWidget(m_playlist);

    m_equalizer = new EqWidget(this);
    dock->addWidget(m_equalizer);

    m_jumpDialog = new JumpToTrackDialog(this);
    m_jumpDialog->setModel(m_playListModel);
    m_jumpDialog->hide();

    m_titlebar = new TitleBar(this);
    m_titlebar->move(0,0);
    m_titlebar->show();
    m_titlebar->setActive(TRUE);

    createActions();
    //prepare visulization
    Visual::initialize(this, m_visMenu, SLOT(updateActions()));
    m_vis = MainVisual::getPointer();
    Visual::add(m_vis);
    //connections
    connect (m_playlist,SIGNAL(next()),SLOT(next()));
    connect (m_playlist,SIGNAL(prev()),SLOT(previous()));
    connect (m_playlist,SIGNAL(play()),SLOT(play()));
    connect (m_playlist,SIGNAL(pause()), m_core ,SLOT(pause()));
    connect (m_playlist,SIGNAL(stop()),SLOT(stop()));
    connect (m_playlist,SIGNAL(eject()),SLOT(addFile()));
    connect (m_playlist,SIGNAL(newPlaylist()),SLOT(newPlaylist()));
    connect (m_playlist,SIGNAL(loadPlaylist()),SLOT(loadPlaylist()));
    connect (m_playlist,SIGNAL(savePlaylist()),SLOT(savePlaylist()));

    connect(display,SIGNAL(shuffleToggled(bool)),m_playListModel,SLOT(prepareForShufflePlaying(bool)));
    connect(display,SIGNAL(repeatableToggled(bool)),m_playListModel,SLOT(prepareForRepeatablePlaying(bool)));

    connect(m_equalizer, SIGNAL(valueChanged()), SLOT(updateEQ()));

    connect(m_jumpDialog,SIGNAL(playRequest()),this,SLOT(play()));

    //connect(m_core, SIGNAL(finished()), SLOT(next()));
    connect(m_core, SIGNAL(stateChanged(Qmmp::State)), SLOT(showState(Qmmp::State)));
    connect(m_core, SIGNAL(elapsedChanged(qint64)),m_playlist, SLOT(setTime(qint64)));
    connect(m_core, SIGNAL(elapsedChanged(qint64)),m_titlebar, SLOT(setTime(qint64)));
    connect(m_core, SIGNAL(metaDataChanged()),SLOT(showMetaData()));
    connect(m_core, SIGNAL(bufferingProgress(int)), TextScroller::getPointer(),
            SLOT(setProgress(int)));

    connect(m_generalHandler, SIGNAL(toggleVisibilityCalled()), SLOT(toggleVisibility()));
    connect(m_generalHandler, SIGNAL(exitCalled()), SLOT(close()));

    readSettings();
    display->setEQ(m_equalizer);
    display->setPL(m_playlist);
    dock->updateDock();
    m_playListModel->readSettings();
    updateEQ();
    char buf[PATH_MAX + 1];
    QString cwd = QString::fromLocal8Bit(getcwd(buf,PATH_MAX));
    processCommandArgs(args,cwd);
    if (m_startHidden && m_generalHandler->visibilityControl())
        toggleVisibility();
}


MainWindow::~MainWindow()
{
    stop();
}

void MainWindow::play()
{
    m_player->play();
}

void MainWindow::replay()
{
    stop();
    play();
}

void MainWindow::seek(int pos)
{
    if (!seeking)
        m_core->seek(pos);
}


void MainWindow::forward()
{
    seek(m_core->elapsed() + KEY_OFFSET);
}

void MainWindow::backward()
{
    seek(qMax(qint64(0), m_core->elapsed() - KEY_OFFSET));
}

void MainWindow::setVolume(int volume, int balance)
{
    m_core->setVolume(volume-qMax(balance,0)*volume/100,
                      volume+qMin(balance,0)*volume/100);
}

void MainWindow::pause(void)
{
    m_core->pause();
}

void MainWindow::stop()
{
    //display->setTime(0);
    m_core->stop();
}

void MainWindow::next()
{
     m_player->next();
}

void MainWindow::previous()
{
    m_player->previous();
}

void MainWindow::updateEQ()
{
    int b[10];
    for (int i=0; i<10; ++i)
        b[i] = m_equalizer->gain(i);
    m_core->setEQ(b, m_equalizer->preamp());
    m_core->setEQEnabled(m_equalizer->isEQEnabled());
}

void MainWindow::showState(Qmmp::State state)
{
    disconnect(m_playListModel, SIGNAL(firstAdded()), this, SLOT(play()));
    switch ((int) state)
    {
    case Qmmp::Playing:
    {
        m_equalizer->loadPreset(m_playListModel->currentItem()->url().section("/",-1));
        if (m_playlist->listWidget())
            m_playlist->listWidget()->updateList(); //removes progress message from TextScroller
        break;
    }
    case Qmmp::Paused:
    {
        //m_generalHandler->setState(General::Paused);
        break;
    }
    case Qmmp::Stopped:
    {
        //m_generalHandler->setState(General::Stopped);
        m_playlist->setTime(-1);
        m_titlebar->setTime(-1);
        break;
    }
    }
}
void MainWindow::showMetaData()
{
    qDebug("===== metadata ======");
    qDebug("ARTIST = %s", qPrintable(m_core->metaData(Qmmp::TITLE)));
    qDebug("TITLE = %s", qPrintable(m_core->metaData(Qmmp::ARTIST)));
    qDebug("ALBUM = %s", qPrintable(m_core->metaData(Qmmp::ALBUM)));
    qDebug("COMMENT = %s", qPrintable(m_core->metaData(Qmmp::COMMENT)));
    qDebug("GENRE = %s", qPrintable(m_core->metaData(Qmmp::GENRE)));
    qDebug("YEAR = %s", qPrintable(m_core->metaData(Qmmp::YEAR)));
    qDebug("TRACK = %s", qPrintable(m_core->metaData(Qmmp::TRACK)));
    qDebug("== end of metadata ==");

    if (m_playlist->currentItem())
    {
        m_playlist->currentItem()->updateMetaData(m_core->metaData());
        m_playlist->updateList();
    }
}

void MainWindow::closeEvent (QCloseEvent *)
{
    writeSettings();
    m_playlist->close();
    m_equalizer->close();
    QApplication::quit ();
}

void MainWindow::addDir()
{
    FileDialog::popup(this, FileDialog::AddDirs, &m_lastDir,
                      m_playListModel, SLOT(addFileList(const QStringList&)),
                      tr("Choose a directory"));
}

void MainWindow::addFile()
{
    QStringList filters;
    filters << tr("All Supported Bitstreams")+" (" + Decoder::nameFilters().join (" ") +")";
    filters << Decoder::filters();
    FileDialog::popup(this, FileDialog::AddDirsFiles, &m_lastDir,
                      m_playListModel, SLOT(addFileList(const QStringList&)),
                      tr("Select one or more files to open"), filters.join(";;"));
}

void MainWindow::clear()
{
    m_playListModel->clear();
}

void MainWindow::startSeek()
{
    seeking = TRUE;
}

void MainWindow::endSeek()
{
    seeking = FALSE;
}

void MainWindow::changeEvent (QEvent * event)
{
    if (event->type() == QEvent::ActivationChange)
    {
        m_titlebar->setActive(isActiveWindow());
    }
}

void MainWindow::readSettings()
{
    QSettings settings(QDir::homePath()+"/.qmmp/qmmprc", QSettings::IniFormat);
    if (!m_update)
    {
        settings.beginGroup("MainWindow");
        //geometry
        move(settings.value("pos", QPoint(100, 100)).toPoint());
        //last directory
        m_lastDir = settings.value("last_dir","/").toString();
        m_startHidden = settings.value("start_hidden", FALSE).toBool();
        settings.endGroup();
        show();

        //visibility
        m_playlist->setVisible(settings.value("Playlist/visible",TRUE).toBool());
        m_equalizer->setVisible(settings.value("Equalizer/visible",TRUE).toBool());
        bool val = settings.value("Playlist/repeatable",FALSE).toBool();

        // Repeat/Shuffle
        m_playListModel->prepareForRepeatablePlaying(val);
        display->setIsRepeatable(val);
        val = settings.value("Playlist/shuffle",FALSE).toBool();
        display->setIsShuffle(val);
        m_playListModel->prepareForShufflePlaying(val);

        // Playlist name
        m_playlistName = settings.value("Playlist/playlist_name","Default").toString();

        m_update = TRUE;
    }
    m_hideOnClose = settings.value("MainWindow/hide_on_close", FALSE).toBool();
}

void MainWindow::writeSettings()
{
    QSettings settings(QDir::homePath()+"/.qmmp/qmmprc", QSettings::IniFormat);
    settings.beginGroup("MainWindow");
    //geometry
    settings.setValue("pos", this->pos());
    //last directory
    settings.setValue("last_dir",m_lastDir);
    settings.endGroup();

    // Repeat/Shuffle
    settings.beginGroup("Playlist");
    settings.setValue("repeatable",display->isRepeatable());
    settings.setValue("shuffle",display->isShuffle());

    // Playlist name
    settings.setValue("playlist_name",m_playlistName);
    settings.endGroup();
}

void MainWindow::showSettings()
{
    m_confDialog = new ConfigDialog(this);
    if (m_confDialog->exec() == QDialog::Accepted)
    {
        readSettings();
        m_playlist->readSettings();
        TextScroller::getPointer()->readSettings();
        //m_core->updateConfig();
        m_visMenu->updateActions();
    }
    delete m_confDialog;
}

void MainWindow::toggleVisibility()
{
    if (isHidden())
    {
        show();
        m_playlist->setVisible(display->isPlaylistVisible());
        m_equalizer->setVisible(display->isEqualizerVisible());
        if (isMinimized())
        {
            if (isMaximized())
                showMaximized();
            else
                showNormal();
        }
        raise();
        activateWindow();
    }
    else
    {
        hide();
        if (m_playlist->isVisible())
            m_playlist->hide();
        if (m_equalizer->isVisible())
            m_equalizer->hide();
    }
}

void MainWindow::createActions()
{
    m_mainMenu = new QMenu(this);
    m_mainMenu->addAction(tr("&Play"),this, SLOT(play()), tr("X"));
    m_mainMenu->addAction(tr("&Pause"),m_core, SLOT(pause()), tr("C"));
    m_mainMenu->addAction(tr("&Stop"),this, SLOT(stop()), tr("V"));
    m_mainMenu->addAction(tr("&Previous"),this, SLOT(previous()), tr("Z"));
    m_mainMenu->addAction(tr("&Next"),this, SLOT(next()), tr("B"));
    m_mainMenu->addAction(tr("&Play/Pause"),this, SLOT(playPause()), tr("Space"));
    m_mainMenu->addSeparator();
    m_mainMenu->addAction(tr("&Jump To File"),this, SLOT(jumpToFile()), tr("J"));
    m_mainMenu->addSeparator();
    m_visMenu = new VisualMenu(this);
    m_mainMenu->addMenu(m_visMenu);

    m_mainMenu->addSeparator();
    m_mainMenu->addAction(tr("&Settings"), this, SLOT(showSettings()), tr("Ctrl+P"));
    m_mainMenu->addSeparator();
    m_mainMenu->addAction(tr("&About"), this, SLOT(about()));
    m_mainMenu->addAction(tr("&About Qt"), qApp, SLOT(aboutQt()));
    Dock::getPointer()->addActions(m_mainMenu->actions());
    m_mainMenu->addSeparator();
    m_mainMenu->addAction(tr("&Exit"),this, SLOT(close ()), tr("Ctrl+Q"));

    QAction* forward = new QAction(this);
    forward->setShortcut(QKeySequence(Qt::Key_Right));
    connect(forward,SIGNAL(triggered(bool)),this,SLOT(forward()));
    QAction* backward = new QAction(this);
    backward->setShortcut(QKeySequence(Qt::Key_Left));
    connect(backward,SIGNAL(triggered(bool)),this,SLOT(backward()));

    Dock::getPointer()->addActions( QList<QAction*>() << forward << backward );
    Dock::getPointer()->addActions(m_mainMenu->actions());
}


void MainWindow::about()
{
    AboutDialog dlg(this);
    dlg.exec();
}

QMenu* MainWindow::menu()
{
    return m_mainMenu;
}

void MainWindow::newPlaylist()
{
    m_playListModel->clear();
    m_playlistName = tr("Default");
}

void MainWindow::loadPlaylist()
{
    QStringList l;
    QList<PlaylistFormat*> p_list = PlaylistParser::instance()->formats();
    if (!p_list.isEmpty())
    {
        foreach(PlaylistFormat* fmt,p_list)
        l << fmt->getExtensions();

        QString mask = tr("Playlist Files")+" (" + l.join(" *.").prepend("*.") + ")";
        //TODO use nonmodal dialog and multiplier playlists
        QString f_name = FileDialog::getOpenFileName(this,tr("Open Playlist"),m_lastDir,mask);
        if (!f_name.isEmpty())
        {
            m_playListModel->clear();
            m_playListModel->loadPlaylist(f_name);
            m_playlistName = QFileInfo(f_name).baseName();
            m_lastDir = QFileInfo(f_name).absoluteDir().path();
        }
    }
    else
    {
        qWarning("Error: There is no registered playlist parsers");
    }
}

void MainWindow::savePlaylist()
{
    QStringList l;
    QList<PlaylistFormat*> p_list = PlaylistParser::instance()->formats();
    if (!p_list.isEmpty())
    {
        foreach(PlaylistFormat* fmt,p_list)
        l << fmt->getExtensions();

        QString mask = tr("Playlist Files")+" (" + l.join(" *.").prepend("*.") + ")";
        QString f_name = FileDialog::getSaveFileName(this, tr("Save Playlist"),m_lastDir + "/" +
                         m_playlistName + "." + l[0],mask);

        if (!f_name.isEmpty())
        {
            m_playListModel->savePlaylist(f_name);
            m_playlistName = QFileInfo(f_name).baseName();
            m_lastDir = QFileInfo(f_name).absoluteDir().path();
        }
    }
    else
        qWarning("Error: There is no registered playlist parsers");
}

void MainWindow::setFileList(const QStringList & l)
{
    connect(m_playListModel, SIGNAL(firstAdded()), SLOT(play()));
    if (!m_playListModel->setFileList(l))
    {
        disconnect(m_playListModel, SIGNAL(firstAdded()), this, SLOT(play()));
        addFile();
    }
}

void MainWindow::playPause()
{
    if (m_core->state() == Qmmp::Playing)
        m_core->pause();
    else
        play();
}

bool MainWindow::processCommandArgs(const QStringList &slist,const QString& cwd)
{
    if (slist.count() > 0)
    {
        QString str = slist[0];

        if (str.startsWith("--enqueue")) //check for "--enqueue" modifier
        {
            if (slist.count() < 2)
                return FALSE;
            QStringList full_path_list;
            for (int i = 1; i < slist.count(); ++i)
            {
                if ((slist.at(i).startsWith("/")) || (slist.at(i).contains("://")))   //is it absolute path or url?
                    full_path_list << slist.at(i);
                else
                    full_path_list << cwd + "/" + slist.at(i);
            }
            m_playListModel->addFileList(full_path_list); //TODO url support
        }
        else if (str.startsWith("--")) // is it a command?
        {
            if (CommandLineManager::hasOption(str))
                m_generalHandler->executeCommand(str);
            else if (m_option_manager->identify(str))
                m_option_manager->executeCommand(str, this);
            else
                return FALSE;
        }
        else// maybe it is a list of files or dirs
        {
            QStringList full_path_list;
            foreach(QString s,slist)
            {
                if ((s.startsWith("/")) || (s.contains("://")))   //is it absolute path or url?
                    full_path_list << s;
                else
                    full_path_list << cwd + "/" + s;
            }
            setFileList(full_path_list); //TODO url support
        }
    }
    return TRUE;
}

void MainWindow::jumpToFile()
{
    if (m_jumpDialog->isHidden())
    {
        m_jumpDialog->show();
        m_jumpDialog->refresh();
    }
}

void MainWindow::handleCloseRequest()
{
    if (m_hideOnClose && m_generalHandler->visibilityControl())
        toggleVisibility();
    else
        QApplication::closeAllWindows();
}

void MainWindow::addUrl( )
{
    AddUrlDialog::popup(this,m_playListModel);
}

SoundCore * MainWindow::soundCore() const
{
    return m_core;
}

MainDisplay * MainWindow::mainDisplay() const
{
    return display;
}

void MainWindow::keyPressEvent(QKeyEvent *ke)
{
    QKeyEvent event = QKeyEvent(ke->type(), ke->key(),
                                ke->modifiers(), ke->text(),ke->isAutoRepeat(), ke->count());
    QApplication::sendEvent(m_playlist,&event);
}
