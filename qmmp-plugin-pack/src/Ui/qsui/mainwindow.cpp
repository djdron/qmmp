/***************************************************************************
 *   Copyright (C) 2009-2013 by Ilya Kotov                                 *
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
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/
#include <QPushButton>
#include <QHBoxLayout>
#include <QSlider>
#include <QLabel>
#include <QTreeView>
#include <QMessageBox>
#include <QSignalMapper>
#include <QMenu>
#include <QSettings>
#include <QInputDialog>
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
#include "qsuisettings.h"
#include "aboutqsuidialog.h"
#include "keyboardmanager.h"
#include "equalizer.h"

#define KEY_OFFSET 10000

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    m_ui.setupUi(this);
    m_balance = 0;
    m_update = false;
    //qmmp objects
    m_player = MediaPlayer::instance();
    m_core = SoundCore::instance();
    m_pl_manager = PlayListManager::instance();
    m_uiHelper = UiHelper::instance();
    connect(m_uiHelper, SIGNAL(toggleVisibilityCalled()), SLOT(toggleVisibility()));
    m_visMenu = new VisualMenu(this); //visual menu
    m_ui.actionVisualization->setMenu(m_visMenu);
    m_pl_menu = new QMenu(this); //playlist menu
    new ActionManager(this); //action manager
    //status
    connect(m_core, SIGNAL(elapsedChanged(qint64)), SLOT(updatePosition(qint64)));
    connect(m_core, SIGNAL(stateChanged(Qmmp::State)), SLOT(showState(Qmmp::State)));
    connect(m_core, SIGNAL(bitrateChanged(int)), SLOT(showBitrate(int)));
    connect(m_core, SIGNAL(bufferingProgress(int)), SLOT(showBuffering(int)));
    connect(m_core, SIGNAL(metaDataChanged()), SLOT(showMetaData()));
    //keyboard manager
    m_key_manager = new KeyboardManager(this);
    //create tabs
    foreach(PlayListModel *model, m_pl_manager->playLists())
    {
        ListWidget *list = new ListWidget(model, this);
        list->setMenu(m_pl_menu);
        if(m_pl_manager->currentPlayList() != model)
            m_ui.tabWidget->addTab(list, model->name());
        else
        {
            m_ui.tabWidget->addTab(list, "[" + model->name() + "]");
            m_ui.tabWidget->setCurrentWidget(list);
        }
        if(model == m_pl_manager->selectedPlayList())
        {
            m_ui.tabWidget->setCurrentWidget(list);
            m_key_manager->setListWidget(list);
        }
    }
    m_slider = new PositionSlider(this);
    m_slider->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_ui.progressToolBar->addWidget(m_slider);
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
    connect(m_ui.tabWidget,SIGNAL(currentChanged(int)), m_pl_manager, SLOT(selectPlayList(int)));
    connect(m_ui.tabWidget, SIGNAL(tabCloseRequested(int)), SLOT(removePlaylistWithIndex(int)));
    connect(m_ui.tabWidget, SIGNAL(tabMoved(int,int)), m_pl_manager, SLOT(move(int,int)));

    m_ui.tabWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_ui.tabWidget, SIGNAL(customContextMenuRequested(QPoint)), SLOT(showTabMenu(QPoint)));
    m_tab_menu = new QMenu(m_ui.tabWidget);
    //status bar
    m_timeLabel = new QLabel(this);
    m_statusLabel = new QLabel(this);
    m_ui.statusbar->addPermanentWidget(m_statusLabel, 0);
    m_ui.statusbar->addPermanentWidget(m_timeLabel, 1);
    //volume
    m_volumeSlider = new QSlider(Qt::Horizontal, this);
    m_volumeSlider->setFixedWidth(100);
    m_volumeSlider->setRange(0,100);
    m_ui.progressToolBar->addSeparator();
    m_ui.progressToolBar->addWidget(m_volumeSlider);
    QIcon volumeIcon = QIcon::fromTheme("audio-volume-high", QIcon(":/qsui/audio-volume-high.png"));
    m_volumeAction = m_ui.progressToolBar->addAction(volumeIcon, tr("Volume"));
    connect(m_volumeSlider, SIGNAL(valueChanged(int)), SLOT(setVolume(int)));
    connect(m_core, SIGNAL(volumeChanged(int,int)), SLOT(updateVolume()));

    updateVolume();
    createActions();
    readSettings();

    //visualization
    Visual::add(m_ui.visualWidget);

    m_ui.splitter->setStretchFactor(0,1);
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
    m_uiHelper->addUrl(this);
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
        m_ui.visualWidget->setCover(MetaDataManager::instance()->getCover(m_core->url()));
        break;
    case Qmmp::Paused:
        m_statusLabel->setText("<b>" + tr("Paused") + "</b>");
        break;
    case Qmmp::Stopped:
        m_statusLabel->setText("<b>" + tr("Stopped") + "</b>");
        m_timeLabel->clear();
        m_slider->setValue(0);
        m_ui.visualWidget->clearCover();
        setWindowTitle("Qmmp");
        break;
    }
}

void MainWindow::updateTabs()
{
    for(int i = 0; i < m_pl_manager->count(); ++i)
    {
        PlayListModel *model = m_pl_manager->playListAt(i);
        if(model == m_pl_manager->currentPlayList())
            m_ui.tabWidget->setTabText(i, "[" + model->name() + "]");
        else
            m_ui.tabWidget->setTabText(i, model->name());
        if(model == m_pl_manager->selectedPlayList())
        {
            m_ui.tabWidget->setCurrentIndex(i);
            m_key_manager->setListWidget(qobject_cast<ListWidget *>(m_ui.tabWidget->widget(i)));
        }
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
    list->setMenu(m_pl_menu);
    m_ui.tabWidget->insertTab(index, list, m_pl_manager->playListAt(index)->name());
    updateTabs();
}

void MainWindow::removeTab(int index)
{
    m_ui.tabWidget->widget(index)->deleteLater();
    m_ui.tabWidget->removeTab(index);
    updateTabs();
}

void MainWindow::renameTab()
{
    bool ok = false;
    QString name = QInputDialog::getText (this,
                                          tr("Rename Playlist"), tr("Playlist name:"),
                                          QLineEdit::Normal,
                                          m_pl_manager->selectedPlayList()->name(), &ok);
    if(ok)
    {
        m_pl_manager->selectedPlayList()->setName(name);
        updateTabs();
    }
}

void MainWindow::aboutUi()
{
    AboutQSUIDialog dialog(this);
    dialog.exec();
}

void MainWindow::about()
{
    m_uiHelper->about(this);
}

void MainWindow::toggleVisibility()
{
    if(isHidden())
    {
        show();
        activateWindow();
    }
    else
        hide();
}

void MainWindow::showSettings()
{
    ConfigDialog *confDialog = new ConfigDialog(this);
    QSUISettings *simpleSettings = new QSUISettings(this);
    confDialog->addPage(tr("Appearance"), simpleSettings, QIcon(":/qsui/qsui_settings.png"));
    confDialog->exec();
    simpleSettings->writeSettings();
    confDialog->deleteLater();
    readSettings();
    ActionManager::instance()->saveActions();
    m_ui.visualWidget->readSettings();
}

void MainWindow::setVolume(int volume)
{
    m_core->setVolume(volume-qMax(m_balance,0)*volume/100,
                      volume+qMin(m_balance,0)*volume/100);
}

void MainWindow::updateVolume()
{
    int maxVol = qMax(m_core->leftVolume(), m_core->rightVolume());
    m_volumeSlider->setValue(maxVol);

    QString iconName = "audio-volume-high";
    if(maxVol == 0)
        iconName = "audio-volume-muted";
    else if(maxVol < 30)
        iconName = "audio-volume-low";
    else if(maxVol >= 30 && maxVol < 60)
        iconName = "audio-volume-medium";

    m_volumeAction->setIcon(QIcon::fromTheme(iconName, QIcon(QString(":/qsui/") + iconName + ".png")));

    if (maxVol)
        m_balance = (m_core->leftVolume() - m_core->rightVolume()) * 100 / maxVol;
}

void MainWindow::jumpTo()
{
    m_uiHelper->jumpToTrack(this);
}

void MainWindow::playPause()
{
    if (m_core->state() == Qmmp::Playing)
        m_core->pause();
    else
        m_player->play();
}

void MainWindow::showBitrate(int)
{
    m_statusLabel->setText(tr("<b>%1</b> [%2 bit/%3/%4 Hz/%5 kbps]")
                           .arg(tr("Playing"))
                           .arg(m_core->sampleSize())
                           .arg(m_core->channels() > 1 ? tr("Stereo"):tr("Mono"))
                           .arg(m_core->frequency())
                           .arg(m_core->bitrate()));
}

void MainWindow::closeEvent(QCloseEvent *)
{
    writeSettings();
    if(!m_hideOnClose || !m_uiHelper->visibilityControl())
        m_uiHelper->exit();

}

void MainWindow::createActions()
{
    //preprare cheackable actions
    connect(ACTION(ActionManager::REPEAT_ALL), SIGNAL(triggered(bool)),
            m_pl_manager, SLOT(setRepeatableList(bool)));
    connect(ACTION(ActionManager::REPEAT_TRACK), SIGNAL(triggered(bool)),
            m_player, SLOT(setRepeatable(bool)));
    connect(ACTION(ActionManager::SHUFFLE), SIGNAL(triggered(bool)),
            m_pl_manager, SLOT(setShuffle(bool)));
    connect(ACTION(ActionManager::NO_PL_ADVANCE), SIGNAL(triggered(bool)),
            m_player, SLOT(setNoPlaylistAdvance(bool)));

    ACTION(ActionManager::REPEAT_ALL)->setChecked(m_pl_manager->isRepeatableList());
    ACTION(ActionManager::SHUFFLE)->setChecked(m_pl_manager->isShuffle());

    connect(m_pl_manager, SIGNAL(repeatableListChanged(bool)),
            ACTION(ActionManager::REPEAT_ALL), SLOT(setChecked(bool)));
    connect(m_player, SIGNAL (repeatableChanged(bool)),
            ACTION(ActionManager::REPEAT_TRACK), SLOT(setChecked(bool)));
    connect(m_player, SIGNAL (noPlaylistAdvanceChanged(bool)),
            ACTION(ActionManager::NO_PL_ADVANCE), SLOT(setChecked(bool)));
    connect(m_pl_manager, SIGNAL(shuffleChanged(bool)),
            ACTION(ActionManager::SHUFFLE), SLOT(setChecked(bool)));
    //main toolbar
    m_ui.buttonsToolBar->addAction(SET_ACTION(ActionManager::PREVIOUS, m_player, SLOT(previous())));
    m_ui.buttonsToolBar->addAction(SET_ACTION(ActionManager::PLAY, m_player, SLOT(play())));
    m_ui.buttonsToolBar->addAction(SET_ACTION(ActionManager::PAUSE, m_core, SLOT(pause())));
    m_ui.buttonsToolBar->addAction(SET_ACTION(ActionManager::STOP, m_player, SLOT(stop())));
    m_ui.buttonsToolBar->addAction(SET_ACTION(ActionManager::NEXT, m_player, SLOT(next())));
    m_ui.buttonsToolBar->addAction(SET_ACTION(ActionManager::EJECT,this, SLOT(addFiles())));

    //file menu
    m_ui.menuFile->addAction(SET_ACTION(ActionManager::PL_ADD_FILE, this, SLOT(addFiles())));
    m_ui.menuFile->addAction(SET_ACTION(ActionManager::PL_ADD_DIRECTORY, this, SLOT(addDir())));
    m_ui.menuFile->addAction(SET_ACTION(ActionManager::PL_ADD_URL, this, SLOT(addUrl())));
    m_ui.menuFile->addSeparator();
    m_ui.menuFile->addAction(SET_ACTION(ActionManager::PL_NEW, this, SLOT(addPlaylist())));
    m_ui.menuFile->addAction(SET_ACTION(ActionManager::PL_CLOSE, this, SLOT(removePlaylist())));
    m_ui.menuFile->addAction(SET_ACTION(ActionManager::PL_RENAME, this, SLOT(renameTab())));
    m_ui.menuFile->addAction(SET_ACTION(ActionManager::PL_SELECT_NEXT, m_pl_manager,
                                        SLOT(selectNextPlayList())));
    m_ui.menuFile->addAction(SET_ACTION(ActionManager::PL_SELECT_PREVIOUS, m_pl_manager,
                                        SLOT(selectPreviousPlayList())));
    m_ui.menuFile->addSeparator();
    m_ui.menuFile->addAction(SET_ACTION(ActionManager::PL_LOAD, this, SLOT(loadPlayList())));
    m_ui.menuFile->addAction(SET_ACTION(ActionManager::PL_SAVE, this, SLOT(savePlayList())));
    m_ui.menuFile->addSeparator();
    m_ui.menuFile->addAction(SET_ACTION(ActionManager::QUIT, m_uiHelper, SLOT(exit())));
    //edit menu
    m_ui.menuEdit->addAction(SET_ACTION(ActionManager::PL_SELECT_ALL, m_pl_manager, SLOT(selectAll())));
    m_ui.menuEdit->addSeparator();
    m_ui.menuEdit->addAction(SET_ACTION(ActionManager::PL_REMOVE_SELECTED, m_pl_manager,
                                        SLOT(removeSelected())));
    m_ui.menuEdit->addAction(SET_ACTION(ActionManager::PL_REMOVE_UNSELECTED, m_pl_manager,
                                        SLOT(removeUnselected())));
    m_ui.menuEdit->addAction(SET_ACTION(ActionManager::PL_REMOVE_ALL, m_pl_manager, SLOT(clear())));
    m_ui.menuEdit->addAction(SET_ACTION(ActionManager::PL_REMOVE_INVALID, m_pl_manager,
                                        SLOT(removeInvalidTracks())));
    m_ui.menuEdit->addAction(SET_ACTION(ActionManager::PL_REMOVE_DUPLICATES, m_pl_manager,
                                        SLOT(removeDuplicates())));
    m_ui.menuEdit->addSeparator();
    //view menu
    m_ui.menuView->addAction(SET_ACTION(ActionManager::WM_ALLWAYS_ON_TOP, this, SLOT(readSettings())));
    m_ui.menuView->addSeparator();
    m_ui.menuView->addAction(SET_ACTION(ActionManager::UI_ANALYZER, this, SLOT(readSettings())));

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

    QAction* discAct = sort_mode_menu->addAction (tr("By Disc Number"));
    connect (discAct, SIGNAL (triggered (bool)), signalMapper, SLOT (map()));
    signalMapper->setMapping (discAct, PlayListModel::DISCNUMBER);

    QAction* fileCreationDateAct = sort_mode_menu->addAction (tr("By File Creation Date"));
    connect (fileCreationDateAct, SIGNAL (triggered (bool)), signalMapper, SLOT (map()));
    signalMapper->setMapping (fileCreationDateAct, PlayListModel::FILE_CREATION_DATE);

    connect (signalMapper, SIGNAL (mapped (int)), m_pl_manager, SLOT (sort (int)));

    m_ui.menuEdit->addMenu (sort_mode_menu);

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

    discAct = sort_mode_menu->addAction (tr("By Disc Number"));
    connect (discAct, SIGNAL (triggered (bool)), signalMapper, SLOT (map()));
    signalMapper->setMapping (discAct, PlayListModel::DISCNUMBER);

    fileCreationDateAct = sort_mode_menu->addAction (tr("By File Creation Date"));
    connect (fileCreationDateAct, SIGNAL (triggered (bool)), signalMapper, SLOT (map()));
    signalMapper->setMapping (fileCreationDateAct, PlayListModel::FILE_CREATION_DATE);

    connect (signalMapper, SIGNAL (mapped (int)), m_pl_manager, SLOT (sortSelection (int)));
    m_ui.menuEdit->addMenu (sort_mode_menu);
    m_ui.menuEdit->addSeparator();
    m_ui.menuEdit->addAction (QIcon::fromTheme("media-playlist-shuffle"), tr("Randomize List"),
                              m_pl_manager, SLOT(randomizeList()));
    m_ui.menuEdit->addAction (QIcon::fromTheme("view-sort-descending"), tr("Reverse List"),
                              m_pl_manager, SLOT(reverseList()));
    m_ui.menuEdit->addAction(SET_ACTION(ActionManager::PL_GROUP_TRACKS, m_pl_manager, SLOT(setGroupsEnabled(bool))));
    ACTION(ActionManager::PL_GROUP_TRACKS)->setChecked(m_pl_manager->isGroupsEnabled());
    m_ui.menuEdit->addSeparator();
    m_ui.menuEdit->addAction(SET_ACTION(ActionManager::SETTINGS, this, SLOT(showSettings())));
    //tools
    m_ui.menuTools->addMenu(m_uiHelper->createMenu(UiHelper::TOOLS_MENU, tr("Actions"), this));
    //playback menu
    m_ui.menuPlayback->addAction(ACTION(ActionManager::PLAY));
    m_ui.menuPlayback->addAction(ACTION(ActionManager::STOP));
    m_ui.menuPlayback->addAction(ACTION(ActionManager::PAUSE));
    m_ui.menuPlayback->addAction(ACTION(ActionManager::NEXT));
    m_ui.menuPlayback->addAction(ACTION(ActionManager::PREVIOUS));
    m_ui.menuPlayback->addAction(SET_ACTION(ActionManager::PLAY_PAUSE,this,SLOT(playPause())));
    m_ui.menuPlayback->addSeparator();
    m_ui.menuPlayback->addAction(SET_ACTION(ActionManager::JUMP, this, SLOT(jumpTo())));
    m_ui.menuPlayback->addSeparator();
    m_ui.menuPlayback->addAction(ACTION(ActionManager::PL_ENQUEUE));
    m_ui.menuPlayback->addAction(SET_ACTION(ActionManager::CLEAR_QUEUE, m_pl_manager, SLOT(clearQueue())));
    m_ui.menuPlayback->addSeparator();
    m_ui.menuPlayback->addAction(ACTION(ActionManager::REPEAT_ALL));
    m_ui.menuPlayback->addAction(ACTION(ActionManager::REPEAT_TRACK));
    m_ui.menuPlayback->addAction(ACTION(ActionManager::SHUFFLE));
    m_ui.menuPlayback->addAction(ACTION(ActionManager::NO_PL_ADVANCE));
    m_ui.menuPlayback->addAction(SET_ACTION(ActionManager::STOP_AFTER_SELECTED, m_pl_manager,
                                            SLOT(stopAfterSelected())));
    //help menu
    m_ui.menuHelp->addAction(SET_ACTION(ActionManager::ABOUT_UI, this, SLOT(aboutUi())));
    m_ui.menuHelp->addAction(SET_ACTION(ActionManager::ABOUT, this, SLOT(about())));
    m_ui.menuHelp->addAction(SET_ACTION(ActionManager::ABOUT_QT, qApp, SLOT(aboutQt())));
    //playlist menu
    m_pl_menu->addAction(SET_ACTION(ActionManager::PL_SHOW_INFO, m_pl_manager, SLOT(showDetails())));
    m_pl_menu->addSeparator();
    m_pl_menu->addAction(ACTION(ActionManager::PL_REMOVE_SELECTED));
    m_pl_menu->addAction(ACTION(ActionManager::PL_REMOVE_ALL));
    m_pl_menu->addAction(ACTION(ActionManager::PL_REMOVE_UNSELECTED));
    m_pl_menu->addMenu(UiHelper::instance()->createMenu(UiHelper::PLAYLIST_MENU,
                                                        tr("Actions"), this));
    m_pl_menu->addSeparator();
    m_pl_menu->addAction(SET_ACTION(ActionManager::PL_ENQUEUE, m_pl_manager, SLOT(addToQueue())));
    //tools menu
    m_ui.menuTools->addAction(SET_ACTION(ActionManager::EQUALIZER, this, SLOT(showEqualizer())));

    //tab menu
    m_tab_menu->addAction(ACTION(ActionManager::PL_RENAME));
    m_tab_menu->addAction(ACTION(ActionManager::PL_CLOSE));
    //seeking
    QAction* forward = new QAction(this);
    forward->setShortcut(QKeySequence(Qt::Key_Right));
    connect(forward,SIGNAL(triggered(bool)),this,SLOT(forward()));
    QAction* backward = new QAction(this);
    backward->setShortcut(QKeySequence(Qt::Key_Left));
    connect(backward,SIGNAL(triggered(bool)),this,SLOT(backward()));

    addActions(QList<QAction*>() << forward << backward);
    addActions(ActionManager::instance()->actions());
    addActions(m_key_manager->actions());
}

void MainWindow::readSettings()
{
    QSettings settings (Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("Simple");
    if(!m_update)
    {
        restoreGeometry(settings.value("mw_geometry").toByteArray());
        restoreState(settings.value("mw_state").toByteArray());
        if(settings.value("always_on_top", false).toBool())
        {
            ACTION(ActionManager::WM_ALLWAYS_ON_TOP)->setChecked(true);
            setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
        }
        show();
        qApp->processEvents();
        if(settings.value("start_hidden").toBool())
            hide();

        ACTION(ActionManager::UI_ANALYZER)->setChecked(settings.value("show_analyzer", true).toBool());
        m_ui.splitter->setSizes(QList<int>() << 200 << 100);
        m_ui.splitter->restoreState(settings.value("splitter_sizes").toByteArray());

        m_update = true;
    }
    else
    {
        for(int i = 0; i < m_ui.tabWidget->count(); ++i)
        {
            qobject_cast<ListWidget *>(m_ui.tabWidget->widget(i))->readSettings();
        }
        if(ACTION(ActionManager::WM_ALLWAYS_ON_TOP)->isChecked())
            setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
        else
            setWindowFlags(windowFlags() & ~Qt::WindowStaysOnTopHint);

        show();
    }
    m_hideOnClose = settings.value("hide_on_close", false).toBool();
    m_ui.tabWidget->setTabsClosable(settings.value("pl_tabs_closable", false).toBool());
    m_ui.visualWidget->setVisible(ACTION(ActionManager::UI_ANALYZER)->isChecked());
    settings.endGroup();

    addActions(m_uiHelper->actions(UiHelper::TOOLS_MENU));
    addActions(m_uiHelper->actions(UiHelper::PLAYLIST_MENU));
}

void MainWindow::showTabMenu(QPoint pos)
{
    QTabBar *tabBar = qobject_cast<QTabBar *> (m_ui.tabWidget->childAt(pos));
    if(!tabBar)
        return;

    int index = tabBar->tabAt(pos);
    if(index == -1)
        return;

    m_pl_manager->selectPlayList(index);
    m_tab_menu->popup(m_ui.tabWidget->mapToGlobal(pos));
}

void MainWindow::writeSettings()
{
    QSettings settings (Qmmp::configFile(), QSettings::IniFormat);
    settings.setValue("Simple/mw_geometry", saveGeometry());
    settings.setValue("Simple/mw_state", saveState());
    settings.setValue("Simple/splitter_sizes", m_ui.splitter->saveState());
    settings.setValue("Simple/always_on_top", ACTION(ActionManager::WM_ALLWAYS_ON_TOP)->isChecked());
    settings.setValue("Simple/show_analyzer", ACTION(ActionManager::UI_ANALYZER)->isChecked());
}

void MainWindow::savePlayList()
{
    m_uiHelper->savePlayList(this);
}

void MainWindow::loadPlayList()
{
    m_uiHelper->loadPlayList(this);
}

void MainWindow::on_tabWidget_tabCloseRequested(int index)
{
    m_pl_manager->removePlayList(index);
}

void MainWindow::showBuffering(int percent)
{
    if(m_core->state() == Qmmp::Buffering)
        m_statusLabel->setText(tr("Buffering: %1%").arg(percent));
}

void MainWindow::showEqualizer()
{
    Equalizer equalizer(this);
    equalizer.exec();
}

void MainWindow::forward()
{
    m_core->seek(m_core->elapsed() + KEY_OFFSET);
}

void MainWindow::backward()
{
    m_core->seek(qMax(qint64(0), m_core->elapsed() - KEY_OFFSET));
}

void MainWindow::showMetaData()
{
    PlayListModel *model = m_pl_manager->currentPlayList();
    PlayListTrack *track = model->currentTrack();
    if(track && track->url() == m_core->metaData().value(Qmmp::URL))
    {
        setWindowTitle(track->formattedTitle());
    }
}
