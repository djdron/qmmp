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
#include <QPushButton>
#include <QHBoxLayout>
#include <QSlider>
#include <QLabel>
#include <QTreeView>
#include <QMessageBox>
#include <QSignalMapper>
#include <qmmp/soundcore.h>
#include <qmmp/decoder.h>
#include <qmmp/metadatamanager.h>
#include <qmmpui/general.h>
#include <qmmpui/playlistparser.h>
#include <qmmpui/playlistformat.h>
#include <qmmpui/filedialog.h>
#include <qmmpui/playlistmodel.h>
#include <qmmpui/mediaplayer.h>
#include <qmmpui/uihelper.h>
#include <qmmpui/configdialog.h>
#include "actionmanager.h"
#include "visualmenu.h"
#include "listwidget.h"
#include "positionslider.h"
#include "mainwindow.h"
#include "renamedialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    //qmmp objects
    m_player = MediaPlayer::instance();
    m_core = SoundCore::instance();
    m_pl_manager = PlayListManager::instance();
    m_uiHelper = UiHelper::instance();
    connect(m_uiHelper, SIGNAL(toggleVisibilityCalled()), SLOT(toggleVisibility()));
    m_visMenu = new VisualMenu(this); //visual menu
    ui.actionVisualization->setMenu(m_visMenu);
    new ActionManager(this); //action manager
    //status
    connect(m_core, SIGNAL(elapsedChanged(qint64)), SLOT(updatePosition(qint64)));
    connect(m_core, SIGNAL(stateChanged(Qmmp::State)), SLOT(showState(Qmmp::State)));
    connect(m_core, SIGNAL(bitrateChanged(int)), SLOT(showBitrate(int)));
    //create tabs
    foreach(PlayListModel *model, m_pl_manager->playLists())
    {
        ListWidget *list = new ListWidget(model, this);
        if(m_pl_manager->currentPlayList() != model)
            ui.tabWidget->addTab(list, model->name());
        else
        {
            ui.tabWidget->addTab(list, "[" + model->name() + "]");
            ui.tabWidget->setCurrentWidget(list);
        }
    }
    m_slider = new PositionSlider(this);

    ui.progressToolBar->addWidget(m_slider);
    //prepare visualization
    Visual::initialize(this, m_visMenu, SLOT(updateActions()));
    //playlist manager
    connect(m_slider, SIGNAL(sliderReleased()), SLOT(seek()));
    connect(m_pl_manager, SIGNAL(currentPlayListChanged(PlayListModel*,PlayListModel*)),
            SLOT(updateTabs()));
    connect(m_pl_manager, SIGNAL(selectedPlayListChanged(PlayListModel*,PlayListModel*)),
            SLOT(updateTabs()));
    connect(m_pl_manager, SIGNAL(playListRemoved(int)), SLOT(removeTab(int)));
    connect(m_pl_manager, SIGNAL(playListAdded(int)), SLOT(addTab(int)));
    connect(ui.tabWidget,SIGNAL(currentChanged(int)), m_pl_manager, SLOT(selectPlayList(int)));
    connect(ui.tabWidget, SIGNAL(tabCloseRequested(int)), SLOT(removePlaylistWithIndex(int)));
    //status bar
    m_timeLabel = new QLabel(this);
    m_statusLabel = new QLabel(this);
    ui.statusbar->addPermanentWidget(m_statusLabel, 0);
    ui.statusbar->addPermanentWidget(m_timeLabel, 1);
    createActions();
    show();
}

MainWindow::~MainWindow()
{
}

void MainWindow::addDir()
{
    m_uiHelper->addDirectory(this);
}

void MainWindow::addFiles()
{
    m_uiHelper->addFile(this);
}

void MainWindow::addUrl()
{
    //m_uiHelper->
}

void MainWindow::showPlManager()
{
    //TODO add playlist manager
}

void MainWindow::updatePosition(qint64 pos)
{
    m_slider->setMaximum(m_core->totalTime()/1000);
    if(!m_slider->isSliderDown())
        m_slider->setValue(pos/1000);
    m_timeLabel->setText(QString("%1:%2/%3:%4").arg(pos/1000/60, 2, 10, QChar('0'))
                     .arg(pos/1000%60, 2, 10, QChar('0'))
                     .arg(m_core->totalTime()/1000/60, 2, 10, QChar('0'))
                     .arg(m_core->totalTime()/1000%60, 2, 10, QChar('0')));
}

void MainWindow::seek()
{
    m_core->seek(m_slider->value()*1000);
}

void MainWindow::showState(Qmmp::State state)
{
    switch((int) state)
    {
    case Qmmp::Playing:
        showBitrate(m_core->bitrate());
        break;
    case Qmmp::Paused:
        m_statusLabel->setText(tr("Paused"));
        break;
    case Qmmp::Stopped:
        m_statusLabel->setText(tr("Stopped"));
        m_slider->setValue(0);
        break;
    }
}

void MainWindow::updateTabs()
{
    for(int i = 0; i < m_pl_manager->count(); ++i)
    {
        PlayListModel *model = m_pl_manager->playListAt(i);
        if(model == m_pl_manager->currentPlayList())
            ui.tabWidget->setTabText(i, "[" + model->name() + "]");
        else
            ui.tabWidget->setTabText(i, model->name());
        if(model == m_pl_manager->selectedPlayList())
            ui.tabWidget->setCurrentIndex(i);
    }
}

void MainWindow::addPlaylist()
{
    m_pl_manager->createPlayList(tr("Playlist"));
}

void MainWindow::removePlaylist()
{
    m_pl_manager->removePlayList(m_pl_manager->selectedPlayList());
}

void MainWindow::removePlaylistWithIndex(int index)
{
    m_pl_manager->removePlayList(m_pl_manager->playListAt(index));
}

void MainWindow::addTab(int index)
{
    ListWidget *list = new ListWidget(m_pl_manager->playListAt(index), this);
    ui.tabWidget->insertTab(index, list, m_pl_manager->playListAt(index)->name());
    updateTabs();
}

void MainWindow::removeTab(int index)
{
    ui.tabWidget->widget(index)->deleteLater();
    ui.tabWidget->removeTab(index);
    updateTabs();
}

void MainWindow::renameTab()
{
    RenameDialog *dialog = new RenameDialog(this);
    if(dialog->exec()==QDialog::Accepted)
    {
        if(!dialog->ui.nameLineEdit->text().isEmpty())
        {
            m_pl_manager->playListAt(ui.tabWidget->currentIndex())->setName(dialog->ui.nameLineEdit->text());
            updateTabs();
        }
    }
    dialog->deleteLater();
}

void MainWindow::about()
{
    m_uiHelper->about(this);
}

void MainWindow::toggleVisibility()
{
    if (isHidden())
    {
        show();

    }
    else
        hide();
}

void MainWindow::showSettings()
{
    ConfigDialog *confDialog = new ConfigDialog(this);
    confDialog->exec();
    confDialog->deleteLater();
}

void MainWindow::showBitrate(int)
{
    m_statusLabel->setText(tr("<b>Playing</b> [%1 kbps/%2 bit/%3/%4 Hz]").arg(m_core->bitrate())
                              .arg(m_core->sampleSize())
                              .arg(m_core->channels() > 1 ? tr("Stereo"):tr("Mono"))
                              .arg(m_core->frequency()));
}

void MainWindow::closeEvent(QCloseEvent *)
{
    m_uiHelper->exit();
}

void MainWindow::createActions()
{
    //main toolbar
    ui.buttonsToolBar->addAction(SET_ACTION(ActionManager::PREVIOUS, m_player, SLOT(previous())));
    ui.buttonsToolBar->addAction(SET_ACTION(ActionManager::PLAY, m_player, SLOT(play())));
    ui.buttonsToolBar->addAction(SET_ACTION(ActionManager::PAUSE, m_core, SLOT(pause())));
    ui.buttonsToolBar->addAction(SET_ACTION(ActionManager::STOP, m_player, SLOT(stop())));
    ui.buttonsToolBar->addAction(SET_ACTION(ActionManager::NEXT, m_player, SLOT(next())));
    ui.buttonsToolBar->addAction(SET_ACTION(ActionManager::EJECT,this, SLOT(addFiles())));
    //file menu
    ui.menuFile->addAction(SET_ACTION(ActionManager::PL_ADD_FILE, this, SLOT(addFiles())));
    ui.menuFile->addAction(SET_ACTION(ActionManager::PL_ADD_DIRECTORY, this, SLOT(addDir())));
    ui.menuFile->addAction(SET_ACTION(ActionManager::PL_ADD_URL, this, SLOT(addUrl())));
    ui.menuFile->addSeparator();
    ui.menuFile->addAction(SET_ACTION(ActionManager::PL_NEW, this, SLOT(addPlaylist())));
    ui.menuFile->addAction(SET_ACTION(ActionManager::PL_CLOSE, this, SLOT(removePlaylist())));
    //ui.menuFile->addAction(SET_ACTION(ActionManager::PL_R, this, SLOT(removePlaylist()))); //TODO rename
    ui.menuFile->addAction(SET_ACTION(ActionManager::PL_SHOW_MANAGER, this, SLOT(showPlManager())));
    ui.menuFile->addAction(SET_ACTION(ActionManager::PL_SELECT_NEXT, m_pl_manager,
                                      SLOT(selectNextPlayList())));
    ui.menuFile->addAction(SET_ACTION(ActionManager::PL_SELECT_PREVIOUS, m_pl_manager,
                                      SLOT(selectPreviousPlayList())));
    ui.menuFile->addSeparator();
    ui.menuFile->addAction(SET_ACTION(ActionManager::QUIT, m_uiHelper, SLOT(exit())));
    //edit menu
    ui.menuEdit->addAction(SET_ACTION(ActionManager::PL_SELECT_ALL, m_pl_manager, SLOT(selectAll())));
    ui.menuEdit->addAction(SET_ACTION(ActionManager::PL_REMOVE_SELECTED, m_pl_manager,
                                      SLOT(removeSelected())));
    ui.menuEdit->addAction(SET_ACTION(ActionManager::PL_REMOVE_UNSELECTED, m_pl_manager,
                                      SLOT(removeUnselected())));
    ui.menuEdit->addAction(SET_ACTION(ActionManager::PL_REMOVE_ALL, m_pl_manager, SLOT(clear())));
    ui.menuEdit->addSeparator();

    QMenu* sort_mode_menu = new QMenu (tr("Sort List"), this);
    sort_mode_menu->setIcon(QIcon::fromTheme("view-sort-ascending"));
    QSignalMapper* signalMapper = new QSignalMapper (this);
    QAction* titleAct = sort_mode_menu->addAction (tr ("By Title"));
    connect (titleAct, SIGNAL (triggered (bool)), signalMapper, SLOT (map()));
    signalMapper->setMapping (titleAct, PlayListModel::TITLE);

    QAction* albumAct = sort_mode_menu->addAction (tr ("By Album"));
    connect (albumAct, SIGNAL (triggered (bool)), signalMapper, SLOT (map()));
    signalMapper->setMapping (albumAct, PlayListModel::ALBUM);

    QAction* artistAct = sort_mode_menu->addAction (tr ("By Artist"));
    connect (artistAct, SIGNAL (triggered (bool)), signalMapper, SLOT (map()));
    signalMapper->setMapping (artistAct, PlayListModel::ARTIST);

    QAction* nameAct = sort_mode_menu->addAction (tr ("By Filename"));
    connect (nameAct, SIGNAL (triggered (bool)), signalMapper, SLOT (map()));
    signalMapper->setMapping (nameAct, PlayListModel::FILENAME);

    QAction* pathnameAct = sort_mode_menu->addAction (tr ("By Path + Filename"));
    connect (pathnameAct, SIGNAL (triggered (bool)), signalMapper, SLOT (map()));
    signalMapper->setMapping (pathnameAct, PlayListModel::PATH_AND_FILENAME);

    QAction* dateAct = sort_mode_menu->addAction (tr ("By Date"));
    connect (dateAct, SIGNAL (triggered (bool)), signalMapper, SLOT (map()));
    signalMapper->setMapping (dateAct, PlayListModel::DATE);

    QAction* trackAct = sort_mode_menu->addAction (tr("By Track Number"));
    connect (trackAct, SIGNAL (triggered (bool)), signalMapper, SLOT (map()));
    signalMapper->setMapping (trackAct, PlayListModel::TRACK);

    connect (signalMapper, SIGNAL (mapped (int)), m_pl_manager, SLOT (sort (int)));

    ui.menuEdit->addMenu (sort_mode_menu);

    sort_mode_menu = new QMenu (tr("Sort Selection"), this);
    sort_mode_menu->setIcon(QIcon::fromTheme("view-sort-ascending"));
    signalMapper = new QSignalMapper (this);
    titleAct = sort_mode_menu->addAction (tr ("By Title"));
    connect (titleAct, SIGNAL (triggered (bool)), signalMapper, SLOT (map()));
    signalMapper->setMapping (titleAct, PlayListModel::TITLE);

    albumAct = sort_mode_menu->addAction (tr ("By Album"));
    connect (albumAct, SIGNAL (triggered (bool)), signalMapper, SLOT (map()));
    signalMapper->setMapping (albumAct, PlayListModel::ALBUM);

    artistAct = sort_mode_menu->addAction (tr ("By Artist"));
    connect (artistAct, SIGNAL (triggered (bool)), signalMapper, SLOT (map()));
    signalMapper->setMapping (artistAct, PlayListModel::ARTIST);

    nameAct = sort_mode_menu->addAction (tr ("By Filename"));
    connect (nameAct, SIGNAL (triggered (bool)), signalMapper, SLOT (map()));
    signalMapper->setMapping (nameAct, PlayListModel::FILENAME);

    pathnameAct = sort_mode_menu->addAction (tr ("By Path + Filename"));
    connect (pathnameAct, SIGNAL (triggered (bool)), signalMapper, SLOT (map()));
    signalMapper->setMapping (pathnameAct, PlayListModel::PATH_AND_FILENAME);

    dateAct = sort_mode_menu->addAction (tr ("By Date"));
    connect (dateAct, SIGNAL (triggered (bool)), signalMapper, SLOT (map()));
    signalMapper->setMapping (dateAct, PlayListModel::DATE);

    trackAct = sort_mode_menu->addAction (tr ("By Track Number"));
    connect (trackAct, SIGNAL (triggered (bool)), signalMapper, SLOT (map()));
    signalMapper->setMapping (trackAct, PlayListModel::TRACK);

    connect (signalMapper, SIGNAL (mapped (int)), m_pl_manager, SLOT (sortSelection (int)));
    ui.menuEdit->addMenu (sort_mode_menu);
    ui.menuEdit->addSeparator();
    ui.menuEdit->addAction (QIcon::fromTheme("media-playlist-shuffle"), tr("Randomize List"),
                               m_pl_manager, SLOT(randomizeList()));
    ui.menuEdit->addAction (QIcon::fromTheme("view-sort-descending"), tr("Reverse List"),
                               m_pl_manager, SLOT(reverseList()));
    ui.menuEdit->addSeparator();
    ui.menuEdit->addAction(SET_ACTION(ActionManager::SETTINGS, this, SLOT(showSettings())));
    //tools
    ui.menuTools->addMenu(m_uiHelper->createMenu(UiHelper::TOOLS_MENU, tr("Actions"), this));
    //help menu
    ui.menuHelp->addAction(SET_ACTION(ActionManager::ABOUT, this, SLOT(about())));
    ui.menuHelp->addAction(SET_ACTION(ActionManager::ABOUT_QT, qApp, SLOT(aboutQt())));
}

