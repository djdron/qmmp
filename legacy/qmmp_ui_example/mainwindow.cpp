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
#include <qmmp/soundcore.h>
#include <qmmp/decoder.h>
#include <qmmp/metadatamanager.h>
#include <qmmpui/general.h>
#include <qmmpui/playlistparser.h>
#include <qmmpui/playlistformat.h>
#include <qmmpui/filedialog.h>
#include <qmmpui/playlistmodel.h>
#include <qmmpui/mediaplayer.h>
#include <qmmpui/generalhandler.h>
#include "configdialog.h"
#include "visualmenu.h"
#include "listwidget.h"
#include "mainwindow.h"
#include "renamedialog.h"

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent)
{
    ui.setupUi(this);
    //qmmp objects
    m_player = new MediaPlayer(this);
    m_core = new SoundCore(this);
    m_pl_manager = new PlayListManager(this);
    m_player->initialize(m_core, m_pl_manager);
    new PlaylistParser(this);
    m_generalHandler = new GeneralHandler(this);
    connect(m_generalHandler, SIGNAL(toggleVisibilityCalled()), SLOT(toggleVisibility()));
    connect(m_generalHandler, SIGNAL(exitCalled()), qApp, SLOT(closeAllWindows()));
    m_visMenu = new VisualMenu(this); //visual menu
    //actions
    //playback
    connect(ui.actionPlay, SIGNAL(triggered()), m_player, SLOT(play()));
    connect(ui.actionPause, SIGNAL(triggered()), m_core, SLOT(pause()));
    connect(ui.actionNext, SIGNAL(triggered()), m_player, SLOT(next()));
    connect(ui.actionPrevious, SIGNAL(triggered()), m_player, SLOT(previous()));
    connect(ui.actionStop, SIGNAL(triggered()), m_player, SLOT(stop()));
    //file menu
    connect(ui.actionAddFile, SIGNAL(triggered()),SLOT(addFiles()));
    connect(ui.actionAddDirectory, SIGNAL(triggered()),SLOT(addDir()));
    connect(ui.actionNewPlayList, SIGNAL(triggered()),SLOT(addPlaylist()));
    connect(ui.actionClosePlayList,SIGNAL(triggered()),SLOT(removePlaylist()));
    connect(ui.actionRenamePlayList,SIGNAL(triggered()),SLOT(renameTab()));
    connect(ui.actionExit, SIGNAL(triggered()),qApp, SLOT(closeAllWindows()));
    //edit menu
    connect(ui.actionSelectAll, SIGNAL(triggered()), m_pl_manager, SLOT(selectAll()));
    connect(ui.actionRemoveSelected, SIGNAL(triggered()), m_pl_manager, SLOT(removeSelected()));
    connect(ui.actionRemoveUnselected, SIGNAL(triggered()), m_pl_manager, SLOT(removeUnselected()));
    connect(ui.actionClearPlayList, SIGNAL(triggered()),m_pl_manager, SLOT(clear()));
    //tools menu
    connect(ui.actionSettings, SIGNAL(triggered()),SLOT(showSettings()));
    ui.actionVisualization->setMenu(m_visMenu);
    //help menu
    connect(ui.actionAbout, SIGNAL(triggered()), SLOT(about()));
    connect(ui.actionAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
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
    m_slider = new QSlider (Qt::Horizontal, this);
    m_label = new QLabel(this);
    m_label->setText("--:--/--:--");
    ui.progressToolBar->addWidget(m_slider);
    ui.progressToolBar->addWidget(m_label);
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
    ui.tabWidget->setTabsClosable(1);
    connect(ui.tabWidget, SIGNAL(tabCloseRequested(int)), SLOT(removePlaylistWithIndex(int)));
}

MainWindow::~MainWindow()
{
}

void MainWindow::addDir()
{
    FileDialog::popup(this, FileDialog::AddDirs, &m_lastDir,
                      m_pl_manager->selectedPlayList(), SLOT(add(const QStringList&)),
                      tr("Choose a directory"));
}

void MainWindow::addFiles()
{
    QStringList filters;
    filters << tr("All Supported Bitstreams")+" (" +
            MetaDataManager::instance()->nameFilters().join (" ") +")";
    filters << MetaDataManager::instance()->filters();
    FileDialog::popup(this, FileDialog::AddDirsFiles, &m_lastDir,
                      m_pl_manager->selectedPlayList(), SLOT(add(const QStringList&)),
                      tr("Select one or more files to open"), filters.join(";;"));
}

void MainWindow::updatePosition(qint64 pos)
{
    m_slider->setMaximum(m_core->totalTime()/1000);
    if(!m_slider->isSliderDown())
        m_slider->setValue(pos/1000);
    m_label->setText(QString("%1:%2/%3:%4").arg(pos/1000/60, 2, 10, QChar('0'))
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
        ui.statusbar->showMessage(tr("Playing"));
        if(m_label->text() != "--:--/--:--")
            showBitrate(m_core->bitrate());
        break;
    case Qmmp::Paused:
        ui.statusbar->showMessage(tr("Paused"));
        break;
    case Qmmp::Stopped:
         ui.statusbar->showMessage(tr("Stopped"));
        m_label->setText("--:--/--:--");
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
	renameDialog *dialog = new renameDialog(this);
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
     QMessageBox::about (this, tr("About Qmmp UI example"),
                         tr("<p>The <b>Qmmp UI example</b> shows how to develop an alternative "
                            "user interface for <a href=\"http://qmmp.ylsoftware.com\">"
                            "Qt-based Multimedia Player</a></p>"
                            "<p>Written by Ilya Kotov "
                            "<a href=\"mailto:trialuser02@gmail.com\">trialuser02@gmail.com</a></p>"));
 }

 void MainWindow::toggleVisibility()
 {
     if (isHidden())
     {
         show();
         raise();
         activateWindow();
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
         hide();
     qApp->processEvents();
 }

 void MainWindow::showSettings()
 {
     ConfigDialog *confDialog = new ConfigDialog(this);
     confDialog->exec();
     m_visMenu->updateActions();
     confDialog->deleteLater();
 }

void MainWindow::showBitrate(int)
{
    ui.statusbar->showMessage(QString(tr("Playing [%1 kbps/%2 bit/%3]")).arg(m_core->bitrate())
                                    .arg(m_core->precision())
                                    .arg(m_core->channels() > 1 ? tr("Stereo"):tr("Mono")));
}
