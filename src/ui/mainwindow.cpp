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
#include <QtGui>
#include <QFileDialog>
#include <QDir>
#include <QAction>
#include <QMenu>
#include <math.h>
#include <qmmp/soundcore.h>
#include <qmmp/visual.h>
#include <qmmp/metadatamanager.h>
#include <qmmpui/generalhandler.h>
#include <qmmpui/general.h>
#include <qmmpui/playlistparser.h>
#include <qmmpui/playlistformat.h>
#include <qmmpui/commandlinemanager.h>
#include <qmmpui/filedialog.h>
#include <qmmpui/playlistmodel.h>
#include <qmmpui/playlistmanager.h>
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

#define KEY_OFFSET 10000

MainWindow::MainWindow(const QStringList& args, BuiltinCommandLineOption* option_manager, QWidget *parent)
        : QMainWindow(parent)
{
    m_vis = 0;
    m_update = FALSE;
    m_option_manager = option_manager;
    setWindowIcon(QIcon(":/32x32/qmmp.png"));
#if QT_VERSION >= 0x040500
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowCloseButtonHint | Qt::WindowSystemMenuHint);
#else
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowSystemMenuHint);
#endif
    setWindowTitle("Qmmp");

    //prepare libqmmp and libqmmpui libraries for playing
    m_player = new MediaPlayer(this);
    m_core = new SoundCore(this);
    m_pl_manager = new PlayListManager(this);
    m_player->initialize(m_core, m_pl_manager);
    //additional featuries
    new PlaylistParser(this);
    m_generalHandler = new GeneralHandler(this);

    //user interface
    m_skin = new Skin(this);
    resize(275 * m_skin->ratio(),116 * m_skin->ratio());
    Dock *dock = new Dock(this);
    dock->setMainWidget(this);
    m_display = new MainDisplay(this);
    setCentralWidget(m_display);
    m_display->setFocus ();

    m_playlist = new PlayList(m_pl_manager, this);
    dock->addWidget(m_playlist);

    m_equalizer = new EqWidget(this);
    dock->addWidget(m_equalizer);

    m_jumpDialog = new JumpToTrackDialog(m_pl_manager, this);
    m_jumpDialog->hide();

    createActions();
    //prepare visualization
    Visual::initialize(this, m_visMenu, SLOT(updateActions()));
    m_vis = MainVisual::instance();
    Visual::add(m_vis);
    //connections
    connect (m_playlist,SIGNAL(next()),SLOT(next()));
    connect (m_playlist,SIGNAL(prev()),SLOT(previous()));
    connect (m_playlist,SIGNAL(play()),SLOT(play()));
    connect (m_playlist,SIGNAL(pause()), m_core ,SLOT(pause()));
    connect (m_playlist,SIGNAL(stop()),SLOT(stop()));
    connect (m_playlist,SIGNAL(eject()),SLOT(addFile()));
    connect (m_playlist,SIGNAL(loadPlaylist()),SLOT(loadPlaylist()));
    connect (m_playlist,SIGNAL(savePlaylist()),SLOT(savePlaylist()));

    connect(m_display,SIGNAL(shuffleToggled(bool)),m_pl_manager,SLOT(setShuffle(bool)));
    connect(m_display,SIGNAL(repeatableToggled(bool)),m_pl_manager,SLOT(setRepeatableList(bool)));

    connect(m_equalizer, SIGNAL(valueChanged()), SLOT(updateEQ()));
    connect(m_jumpDialog,SIGNAL(playRequest()), SLOT(replay()));

    connect(m_core, SIGNAL(stateChanged(Qmmp::State)), SLOT(showState(Qmmp::State)));
    connect(m_core, SIGNAL(elapsedChanged(qint64)),m_playlist, SLOT(setTime(qint64)));
    connect(m_core, SIGNAL(metaDataChanged()),SLOT(showMetaData()));
    connect(m_core, SIGNAL(bufferingProgress(int)), TextScroller::getPointer(), SLOT(setProgress(int)));
    connect(m_generalHandler, SIGNAL(toggleVisibilityCalled()), SLOT(toggleVisibility()));
    connect(m_generalHandler, SIGNAL(exitCalled()), SLOT(close()));

    readSettings();
    m_display->setEQ(m_equalizer);
    m_display->setPL(m_playlist);
    dock->updateDock();
    m_pl_manager->currentPlayList()->doCurrentVisibleRequest();
    updateEQ();
#ifndef Q_OS_WIN32
    QString cwd = QDir::currentPath();
    processCommandArgs(args,cwd);
#endif
    if (m_startHidden && m_generalHandler->visibilityControl())
        toggleVisibility();
}


MainWindow::~MainWindow()
{}

void MainWindow::play()
{
    m_player->play();
}

void MainWindow::replay()
{
    m_pl_manager->activatePlayList(m_pl_manager->selectedPlayList());
    play();
}

void MainWindow::seek(qint64 pos)
{
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
    m_player->stop();
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
    double b[10];
    for (int i=0; i<10; ++i)
        b[i] = m_equalizer->gain(i);
    m_core->setEQ(b, m_equalizer->preamp());
    m_core->setEQEnabled(m_equalizer->isEQEnabled());
}

void MainWindow::showState(Qmmp::State state)
{
    if(m_model)
    {
        disconnect(m_model, SIGNAL(firstAdded()), this, 0);
        m_model = 0;
    }
    switch ((int) state)
    {
    case Qmmp::Playing:
        if (m_pl_manager->currentPlayList()->currentItem())
            m_equalizer->loadPreset(m_pl_manager->currentPlayList()->currentItem()->url().section("/",-1));
        if (m_playlist->listWidget())
            m_playlist->listWidget()->updateList(); //removes progress message from TextScroller
        break;
    case Qmmp::Paused:
        break;
    case Qmmp::Stopped:
        m_playlist->setTime(-1);
        if (m_playlist->currentItem())
            setWindowTitle(m_playlist->currentItem()->text());
        else
            setWindowTitle("Qmmp");
        break;
    }
}

void MainWindow::showMetaData()
{
    qDebug("===== metadata ======");
    qDebug("ARTIST = %s", qPrintable(m_core->metaData(Qmmp::ARTIST)));
    qDebug("TITLE = %s", qPrintable(m_core->metaData(Qmmp::TITLE)));
    qDebug("ALBUM = %s", qPrintable(m_core->metaData(Qmmp::ALBUM)));
    qDebug("COMMENT = %s", qPrintable(m_core->metaData(Qmmp::COMMENT)));
    qDebug("GENRE = %s", qPrintable(m_core->metaData(Qmmp::GENRE)));
    qDebug("YEAR = %s", qPrintable(m_core->metaData(Qmmp::YEAR)));
    qDebug("TRACK = %s", qPrintable(m_core->metaData(Qmmp::TRACK)));
    qDebug("== end of metadata ==");

    if (m_playlist->currentItem() &&
        m_playlist->currentItem()->url() == m_core->metaData().value(Qmmp::URL))
    {
        m_playlist->currentItem()->updateMetaData(m_core->metaData());
        m_playlist->updateList();
        TextScroller::getPointer()->setText(m_playlist->currentItem()->text());
        setWindowTitle(m_playlist->currentItem()->text());
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
                      m_pl_manager->selectedPlayList(), SLOT(addFileList(const QStringList&)),
                      tr("Choose a directory"));
}

void MainWindow::addFile()
{
    QStringList filters;
    filters << tr("All Supported Bitstreams")+" (" +
            MetaDataManager::instance()->nameFilters().join (" ") +")";
    filters << MetaDataManager::instance()->filters();
    FileDialog::popup(this, FileDialog::AddDirsFiles, &m_lastDir,
                      m_pl_manager->selectedPlayList(), SLOT(addFileList(const QStringList&)),
                      tr("Select one or more files to open"), filters.join(";;"));
}

void MainWindow::changeEvent (QEvent * event)
{
    if (event->type() == QEvent::ActivationChange)
    {
        m_display->setActive(isActiveWindow());
    }
}

void MainWindow::readSettings()
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
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
        qApp->processEvents();
        //visibility
        m_playlist->setVisible(settings.value("Playlist/visible",TRUE).toBool());
        m_equalizer->setVisible(settings.value("Equalizer/visible",TRUE).toBool());
        bool val = settings.value("Playlist/repeatable",FALSE).toBool();

        // Repeat/Shuffle
        m_pl_manager->setRepeatableList(val);
        m_display->setIsRepeatable(val);
        val = settings.value("Playlist/shuffle",FALSE).toBool();
        m_display->setIsShuffle(val);
        m_pl_manager->setShuffle(val);

        m_update = TRUE;
    }
    if(!settings.value("General/metacity_compat", FALSE).toBool())
    {
        setWindowOpacity(settings.value("MainWindow/opacity", 1.0).toDouble());
        m_equalizer->setWindowOpacity(settings.value("Equalizer/opacity", 1.0).toDouble());
        m_playlist->setWindowOpacity(settings.value("PlayList/opacity", 1.0).toDouble());
    }
    m_hideOnClose = settings.value("MainWindow/hide_on_close", FALSE).toBool();
}

void MainWindow::writeSettings()
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("MainWindow");
    //geometry
    settings.setValue("pos", this->pos());
    //last directory
    settings.setValue("last_dir",m_lastDir);
    settings.endGroup();

    // Repeat/Shuffle
    settings.beginGroup("Playlist");
    settings.setValue("repeatable",m_display->isRepeatable());
    settings.setValue("shuffle",m_display->isShuffle());

    settings.endGroup();
}

void MainWindow::showSettings()
{
    m_confDialog = new ConfigDialog(this);
    m_confDialog->exec();
    readSettings();
    m_playlist->readSettings();
    TextScroller::getPointer()->readSettings();
    m_visMenu->updateActions();
    m_skin->reloadSkin();
    Dock::instance()->updateDock();
    delete m_confDialog;
}

void MainWindow::toggleVisibility()
{
    if (isHidden())
    {
        show();
        raise();
        activateWindow();
        m_playlist->setVisible(m_display->isPlaylistVisible());
        m_equalizer->setVisible(m_display->isEqualizerVisible());
        qApp->processEvents();
        setFocus ();
        if (isMinimized())
        {
            if (isMaximized())
                showMaximized();
            else
                showNormal();
        }
    }
    else
    {
        if (m_playlist->isVisible())
            m_playlist->hide();
        if (m_equalizer->isVisible())
            m_equalizer->hide();
        hide();
    }
    qApp->processEvents();
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
    QAction *repeateAllAction = m_mainMenu->addAction(tr("&Repeat Playlist"));
    QAction *repeateTrackAction = m_mainMenu->addAction(tr("&Repeat Track"));
    QAction *shuffleAction = m_mainMenu->addAction(tr("&Shuffle"));
    repeateAllAction->setCheckable (TRUE);
    repeateTrackAction->setCheckable (TRUE);
    shuffleAction->setCheckable (TRUE);
    repeateAllAction->setShortcut(tr("R")) ;
    repeateTrackAction->setShortcut(tr("Ctrl+R")) ;
    shuffleAction->setShortcut(tr("S")) ;
    connect(repeateAllAction, SIGNAL(triggered (bool)), m_pl_manager, SLOT(setRepeatableList(bool)));
    connect(repeateTrackAction, SIGNAL(triggered (bool)), m_player, SLOT(setRepeatable(bool)));
    connect(shuffleAction, SIGNAL(triggered (bool)), m_pl_manager, SLOT(setShuffle(bool)));
    connect(m_pl_manager, SIGNAL(repeatableListChanged(bool)), repeateAllAction, SLOT(setChecked(bool)));
    connect(m_player, SIGNAL (repeatableChanged(bool)), repeateTrackAction, SLOT(setChecked(bool)));
    connect(m_pl_manager, SIGNAL(shuffleChanged(bool)), shuffleAction, SLOT(setChecked(bool)));
    m_mainMenu->addSeparator();
    m_mainMenu->addAction(tr("&Jump To File"),this, SLOT(jumpToFile()), tr("J"));
    m_mainMenu->addSeparator();
    m_visMenu = new VisualMenu(this);
    m_mainMenu->addMenu(m_visMenu);
    m_mainMenu->addMenu(m_generalHandler->createMenu(GeneralHandler::TOOLS_MENU, tr("Tools"), this));
    m_mainMenu->addSeparator();
    m_mainMenu->addAction(tr("&Settings"), this, SLOT(showSettings()), tr("Ctrl+P"));
    m_mainMenu->addSeparator();
    m_mainMenu->addAction(tr("&About"), this, SLOT(about()));
    m_mainMenu->addAction(tr("&About Qt"), qApp, SLOT(aboutQt()));
    Dock::instance()->addActions(m_mainMenu->actions());
    m_mainMenu->addSeparator();
    m_mainMenu->addAction(tr("&Exit"),this, SLOT(close ()), tr("Ctrl+Q"));

    QAction* forward = new QAction(this);
    forward->setShortcut(QKeySequence(Qt::Key_Right));
    connect(forward,SIGNAL(triggered(bool)),this,SLOT(forward()));
    QAction* backward = new QAction(this);
    backward->setShortcut(QKeySequence(Qt::Key_Left));
    connect(backward,SIGNAL(triggered(bool)),this,SLOT(backward()));

    Dock::instance()->addActions( QList<QAction*>() << forward << backward );
    Dock::instance()->addActions(m_mainMenu->actions());
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
            m_pl_manager->selectedPlayList()->clear();
            m_pl_manager->selectedPlayList()->loadPlaylist(f_name);
            m_pl_manager->selectedPlayList()->setName(QFileInfo(f_name).baseName());
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
                         m_pl_manager->selectedPlayList()->name() + "." + l[0],mask);

        if (!f_name.isEmpty())
        {
            m_pl_manager->selectedPlayList()->savePlaylist(f_name);
            m_lastDir = QFileInfo(f_name).absoluteDir().path();
        }
    }
    else
        qWarning("Error: There is no registered playlist parsers");
}

void MainWindow::setFileList(const QStringList & l)
{
    if (m_core->state() == Qmmp::Playing || m_core->state() == Qmmp::Paused)
        stop();
    qApp->processEvents(); //receive stop signal
    connect(m_pl_manager->selectedPlayList(), SIGNAL(firstAdded()), this, SLOT(play()));
    if (m_pl_manager->selectedPlayList()->setFileList(l))
    {
        m_pl_manager->activatePlayList(m_pl_manager->selectedPlayList());
        m_model = m_pl_manager->selectedPlayList();
    }
    else
    {
        disconnect(m_pl_manager->selectedPlayList(), SIGNAL(firstAdded()), this, SLOT(play()));
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

        if (str.startsWith("--enqueue") || str.startsWith("-e")) //check for "--enqueue" modifier
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
            m_pl_manager->currentPlayList()->addFileList(full_path_list); //TODO url support
        }
        else if (str.startsWith("-")) // is it a command?
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

void MainWindow::addUrl()
{
    AddUrlDialog::popup(this, m_pl_manager->selectedPlayList());
}

SoundCore * MainWindow::soundCore() const
{
    return m_core;
}

MainDisplay * MainWindow::mainDisplay() const
{
    return m_display;
}

void MainWindow::keyPressEvent(QKeyEvent *ke)
{
    QKeyEvent event = QKeyEvent(ke->type(), ke->key(),
                                ke->modifiers(), ke->text(),ke->isAutoRepeat(), ke->count());
    QApplication::sendEvent(m_playlist,&event);
}
