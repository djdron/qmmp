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
#include <QtGui>
#include <QFileDialog>
#include <QDir>
#include <QAction>
#include <QMenu>

#include <math.h>

#include <soundcore.h>

#include "textscroller.h"
#include "mainwindow.h"
#include "constants.h"
#include "fileloader.h"
#include "skin.h"
#include "playlist.h"
#include "playlistmodel.h"
#include "configdialog.h"
#include "dock.h"
#include "eqwidget.h"
#include "mainvisual.h"
#include  "playlistformat.h"
#include "tcpserver.h"
#include "jumptotrackdialog.h"
#include "aboutdialog.h"
#include <addurldialog.h>
#include "filedialog.h"

MainWindow::MainWindow(const QStringList& args, QWidget *parent)
        : QMainWindow(parent)
{
    m_vis = 0;
    seeking = FALSE;
    m_update = FALSE;
    m_paused = FALSE;

    setWindowIcon( QIcon(":/qmmp.xpm") );

    m_skin = new Skin(this);
    Dock *dock = new Dock(this);
    dock->setMainWidget(this);

    setWindowFlags(Qt::FramelessWindowHint);
    setFixedSize (275,116);

    display = new MainDisplay(this);
    setCentralWidget(display);
    display->show();
    display->setFocus ();

    m_playlistName = tr("Default");

    m_playlist = new PlayList(this);

    connect (m_playlist,SIGNAL(next()),SLOT(next()));
    connect (m_playlist,SIGNAL(prev()),SLOT(previous()));
    connect (m_playlist,SIGNAL(play()),SLOT(play()));
    connect (m_playlist,SIGNAL(pause()),SLOT(pause()));
    connect (m_playlist,SIGNAL(stop()),SLOT(stop()));
    connect (m_playlist,SIGNAL(eject()),SLOT(addFile()));

    connect (m_playlist,SIGNAL(newPlaylist()),SLOT(newPlaylist()));
    connect (m_playlist,SIGNAL(loadPlaylist()),SLOT(loadPlaylist()));
    connect (m_playlist,SIGNAL(savePlaylist()),SLOT(savePlaylist()));

    m_playListModel = new PlayListModel(this);

    connect(display,SIGNAL(shuffleToggled(bool)),m_playListModel,SLOT(prepareForShufflePlaying(bool)));
    connect(display,SIGNAL(repeatableToggled(bool)),m_playListModel,SLOT(prepareForRepeatablePlaying(bool)));

    dock->addWidget(m_playlist);

    m_equalizer = new EqWidget(this);
    dock->addWidget(m_equalizer);
    connect(m_equalizer, SIGNAL(valueChanged()), SLOT(updateEQ()));

    m_playlist->setModel(m_playListModel);

    m_jumpDialog = new JumpToTrackDialog(this);
    m_jumpDialog->setModel(m_playListModel);
    connect(m_jumpDialog,SIGNAL(playRequest()),this,SLOT(play()));
    m_jumpDialog->hide();

    createActions();

    m_titlebar = new TitleBar(this);
    m_titlebar->move(0,0);
    m_titlebar->show();
    m_titlebar->setActive(TRUE);

    m_tray = new QSystemTrayIcon( this );
    m_tray->setIcon ( QIcon(":/stop.png") );
    m_tray->setContextMenu( m_mainMenu );
    connect(m_tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayActivated(QSystemTrayIcon::ActivationReason)));

    readSettings();
    dock->updateDock();

    display->setEQ(m_equalizer);
    display->setPL(m_playlist);

    m_vis = MainVisual::getPointer();

    m_core = new SoundCore(this);
    m_core -> addVisualization(m_vis);

    connect(m_core, SIGNAL(outputStateChanged(const OutputState&)),
            SLOT(showOutputState(const OutputState&)));
    connect(m_core, SIGNAL(decoderStateChanged(const DecoderState&)),
            SLOT(showDecoderState(const DecoderState&)));

    connect ( m_skin, SIGNAL ( skinChanged() ), this, SLOT ( updateSkin() ) );
    updateEQ();
    updateSkin();

    // Starting the TcpServer

    new TcpServer(this);

    FileDialog::registerBuiltinFactories();
    FileDialog::registerExternalFactories();

    m_playListModel->readSettings();
    char buf[PATH_MAX + 1];
    QString cwd = QString::fromLocal8Bit(getcwd(buf,PATH_MAX));
    processCommandArgs(args,cwd);
}


MainWindow::~MainWindow()
{
    stop();
}

void MainWindow::play()
{
    m_playListModel->doCurrentVisibleRequest();

    if (m_core->isPaused())
    {
        pause();
        return;
    }
    stop();
    if (m_playListModel->count() == 0)
        return;

    m_equalizer->loadPreset(m_playListModel->currentItem()->fileName());
    QString s = m_playListModel->currentItem()->path();
    if (s.isEmpty())
        return;
    if (m_core->play(s))
    {
        display->setTime(0);
        display->setMaxTime(m_core->length());
    }
    else
    {
        //find out the reason why the playback failed
        switch ((int) m_core->error())
        {
        case SoundCore::OutputError:
        {
            stop();
            return; //unrecovable error in output, so abort playing
        }
        case SoundCore::DecoderError:
        {
            //error in decoder, so we should try to play next song
            if (!m_playListModel->isEmptyQueue())
            {
                m_playListModel->setCurrentToQueued();
            }
            else if (!m_playListModel->next())
            {
                stop();
                display->hideTimeDisplay();
                return;
            }
            m_playlist->update();
            play();
            break;
        }
        }
    }
}

void MainWindow::replay()
{
    stop();
    play();
}

void MainWindow::seek(int pos)
{
    m_core->seek(pos);
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
    display->setTime(0);
    m_core->stop();
}
void MainWindow::next()
{
    if (!m_playListModel->isEmptyQueue())
    {
        m_playListModel->setCurrentToQueued();
    }
    else if (!m_playListModel->next())
    {
        stop();
        display->hideTimeDisplay();
        return;
    }
    m_playlist->update();
    if (m_core->isInitialized())
    {
        stop();
        play();
    }
    else
        display->hideTimeDisplay();
}
void MainWindow::previous()
{
    if (!m_playListModel->previous())
    {
        display->hideTimeDisplay();
        return;
    }

    m_playlist->update();
    if (m_core->isInitialized())
    {
        stop();
        play();
    }
    else
        display->hideTimeDisplay();
}

void MainWindow::updateEQ()
{
    int b[10];
    for (int i=0; i<10; ++i)
        b[i] = m_equalizer->gain(i);
    m_core->setEQ(b, m_equalizer->preamp());
    m_core->setEQEnabled(m_equalizer->isEQEnabled());
}

void MainWindow::updatePreset()
{
    //if(m_playListModel->currentItem())
    //  m_equalizer->setPresetName(m_playListModel->currentItem()->fileName());
}

void MainWindow::showOutputState(const OutputState &st)

{
    if (seeking)
        return;

    display->setInfo(st);
    m_playlist->setInfo(st, m_core->length(), m_playListModel->totalLength());
    switch ((int) st.type())
    {
    case OutputState::Playing:
    {
        m_tray->setIcon ( QIcon(":/play.png") );
        if (m_showMessage && m_playListModel->currentItem())
            m_tray->showMessage ( tr("Now Playing"),
                                  m_playListModel->currentItem()->title(),
                                  QSystemTrayIcon::Information, m_messageDelay );
        if (m_showToolTip && m_playListModel->currentItem())
            m_tray->setToolTip (m_playListModel->currentItem()->title());
        break;
    }
    case OutputState::Paused:
    {
        m_tray->setIcon ( QIcon(":/pause.png") );
        break;
    }
    case OutputState::Stopped:
    {
        m_tray->setIcon ( QIcon(":/stop.png") );
        break;
    }
    }

}
void MainWindow::showDecoderState(const DecoderState &st)
{
    switch ((int) st.type())
    {
    case DecoderState::Finished:
    {
        next();
        break;
    }
    }
}

void MainWindow::closeEvent ( QCloseEvent *)
{
    writeSettings();
    m_playlist->close();
    m_equalizer->close();
    QApplication::quit ();
}

void MainWindow::addDir()
{
    /*
    QString s = QFileDialog::getExistingDirectory(
                    this,
                    tr("Choose a directory"),
                    m_lastDir,
                    QFileDialog::DontResolveSymlinks | QFileDialog::ShowDirsOnly);

    if (s.isEmpty())
        return;
    m_playListModel->addDirectory(s);
    m_lastDir = s+"../";
*/
    if(FileDialog::isModal())
    {
        qWarning("void MainWindow::addDir()");
        QString s = FileDialog::getExistingDirectory(this,tr("Choose a directory"),m_lastDir);

        if (s.isEmpty())
            return;
        m_playListModel->addDirectory(s);
        m_lastDir = s+"../";
    }
    else
        FileDialog::popup(m_playListModel,m_lastDir,FileDialog::AddDirs,Decoder::nameFilters());
}

void MainWindow::addFile()
{
    /*
    QStringList files = QFileDialog::getOpenFileNames(
                            this,
                            tr("Select one or more files to open"),
                            m_lastDir,
                            Decoder::filter());
    if (files.isEmpty ())
        return;

    //  foreach(QString s, files)
    //  m_playListModel->load(new MediaFile(s));

    m_playListModel->addFiles(files);
    m_lastDir = files.at(0);
*/


    if(FileDialog::isModal())
    {
        QStringList files = FileDialog::getOpenFileNames(
                                this,
                                tr("Select one or more files to open"),
                                m_lastDir,
                                Decoder::filter());
        if (files.isEmpty ())
            return;
        /*
          foreach(QString s, files)
          m_playListModel->load(new MediaFile(s));
        */
        m_playListModel->addFiles(files);
        m_lastDir = files.at(0);
    }
    else
        FileDialog::popup(m_playListModel,m_lastDir,FileDialog::AddFiles,Decoder::nameFilters());

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

void MainWindow::changeEvent ( QEvent * event )
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
    //tray
    settings.beginGroup("Tray");
    m_tray->setVisible(settings.value("enabled",TRUE).toBool());
    m_showMessage = settings.value("show_message",TRUE).toBool();
    m_messageDelay = settings.value("message_delay",2000).toInt();
    m_showToolTip = settings.value("show_tooltip", FALSE).toBool();
    m_hide_on_titlebar_close = settings.value("hide_on_close",FALSE).toBool();
    if (!m_showToolTip)
        m_tray->setToolTip(QString());
    settings.endGroup();

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
        m_core->updateConfig();
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

void MainWindow::trayActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Trigger)
        toggleVisibility();
}

void MainWindow::createActions()
{
    m_mainMenu = new QMenu(this);
    m_mainMenu->addAction(tr("&Play"),this, SLOT(play()), tr("X"));
    m_mainMenu->addAction(tr("&Pause"),this, SLOT(pause()), tr("C"));
    m_mainMenu->addAction(tr("&Stop"),this, SLOT(stop()), tr("V"));
    m_mainMenu->addAction(tr("&Previous"),this, SLOT(previous()), tr("Z"));
    m_mainMenu->addAction(tr("&Next"),this, SLOT(next()), tr("B"));
    m_mainMenu->addAction(tr("&Queue"),m_playListModel, SLOT(addToQueue()), tr("Q"));
    m_mainMenu->addSeparator();
    m_mainMenu->addAction(tr("&Jump To File"),this, SLOT(jumpToFile()), tr("J"));
    m_mainMenu->addSeparator();
    m_mainMenu->addAction(tr("&Settings"),this, SLOT(showSettings()), tr("Ctrl+P"));
    m_mainMenu->addSeparator();
    m_mainMenu->addAction(tr("&About"),this, SLOT(about()));
    Dock::getPointer()->addActions(m_mainMenu->actions());
    m_mainMenu->addSeparator();
    m_mainMenu->addAction(tr("&Exit"),this, SLOT(close ()), tr("Ctrl+Q"));
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

void MainWindow::updateSkin()
{
    clearMask();
    m_equalizer->clearMask();
    /*qt bug workarround */
    setMask(QRegion(0,0,275,116));
    m_equalizer->setMask(QRegion(0,0,275,116));
    update();
    m_equalizer->update();

    QRegion region = m_skin->getMWRegion();
    if (!region.isEmpty())
        setMask(region);

    region = m_skin->getPLRegion();
    if (!region.isEmpty())
        m_equalizer->setMask(region);
}

void MainWindow::newPlaylist()
{
    m_playListModel->clear();
    m_playlistName = tr("Default");
}

void MainWindow::loadPlaylist()
{
    QStringList l;
    QList<PlaylistFormat*> p_list = m_playListModel->registeredPlaylistFormats();
    if (!p_list.isEmpty())
    {
        foreach(PlaylistFormat* fmt,p_list)
        l << fmt->getExtensions();

        QString mask = tr("Playlist Files")+" (" + l.join(" *.").prepend("*.") + ")";
        if(FileDialog::isModal())
        {
            //qWarning("Modal");
            QString f_name = FileDialog::getOpenFileName(this,tr("Open Playlist"),m_lastDir,mask);
            if (!f_name.isEmpty())
            {
                m_playListModel->loadPlaylist(f_name);
                m_playlistName = QFileInfo(f_name).baseName();
            }
            m_lastDir = QFileInfo(f_name).absoluteDir().path();
        }
        else //FileDialog::popup(m_playListModel,m_lastDir,FileDialog::AddFiles,Decoder::nameFilters());
            // TODO: implement playlist loading with nonmodal dialogs
            // For now we'll use default dialog
        {
            //qWarning("Modal");
            QString f_name = FileDialog::getOpenFileName(this,tr("Open Playlist"),m_lastDir,mask,0,true);
            if (!f_name.isEmpty())
            {
                m_playListModel->loadPlaylist(f_name);
                m_playlistName = QFileInfo(f_name).baseName();
            }
            m_lastDir = QFileInfo(f_name).absoluteDir().path();
        }
    }
    else
    {
        //qWarning("Non Modal");
        qWarning("Error: There is no registered playlist parsers");
    }
}

void MainWindow::savePlaylist()
{
    QStringList l;
    QList<PlaylistFormat*> p_list = m_playListModel->registeredPlaylistFormats();
    if (!p_list.isEmpty())
    {
        foreach(PlaylistFormat* fmt,p_list)
        l << fmt->getExtensions();

        QString mask = tr("Playlist Files")+" (" + l.join(" *.").prepend("*.") + ")";
        if(FileDialog::isModal())
        {
            QString f_name = FileDialog::getSaveFileName(this, tr("Save Playlist"),m_lastDir + "/" +
                             m_playlistName + "." + l[0],mask);

            if (!f_name.isEmpty())
            {
                m_playListModel->savePlaylist(f_name);
                m_playlistName = QFileInfo(f_name).baseName();
            }
            m_lastDir = QFileInfo(f_name).absoluteDir().path();
        }
        else // TODO: implement saving playlists with nonmodal dialogs
            // For now we'll use default dialog
        {
            QString f_name = FileDialog::getSaveFileName(this, tr("Save Playlist"),m_lastDir + "/" +
                    m_playlistName + "." + l[0],mask,0,true);

            if (!f_name.isEmpty())
            {
                m_playListModel->savePlaylist(f_name);
                m_playlistName = QFileInfo(f_name).baseName();
            }
            m_lastDir = QFileInfo(f_name).absoluteDir().path();
        }
    }
    else
        qWarning("Error: There is no registered playlist parsers");
}

void MainWindow::setFileList(const QStringList & l)
{
    if (!m_playListModel->setFileList(l))
        addFile();
}

void MainWindow::playPause()
{
    if (m_core->isInitialized())
        pause();
    else
        play();
}

bool MainWindow::processCommandArgs(const QStringList &slist,const QString& cwd)
{
    if (slist.count() > 0)
    {
        QString str = slist[0];
        if (str.startsWith("--")) // is it a command?
        {
            if (str == "--play")
                play();
            else if (str == "--stop")
            {
                stop();
                display->hideTimeDisplay();
            }
            else if (str == "--pause")
                pause();
            else if (str == "--next")
                next();
            else if (str == "--previous")
                previous();
            else if (str == "--play-pause")
                playPause();
            else
                return false;
        }
        else// maybe it is a list of files or dirs
        {
            QStringList full_path_list;
            foreach(QString s,slist)
            {
                if (s.left(1) == "/")   //is it absolute path?
                    full_path_list << s;
                else
                    full_path_list << cwd + "/" + s;
                //qWarning("Current working dir: %s",qPrintable(workingDir));
                //qWarning("Full path to play: %s",qPrintable(workingDir + "/" + s));
            }
            setFileList(full_path_list);
        }
    }
    return true;
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
    if (m_hide_on_titlebar_close && m_tray->isVisible())
        toggleVisibility();
    else
        QApplication::closeAllWindows();
}



void MainWindow::addUrl( )
{
    AddUrlDialog::popup(this,m_playListModel);
}


