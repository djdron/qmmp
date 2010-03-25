/***************************************************************************
 *   Copyright (C) 2009 by Ilya Kotov                                      *
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
#include <qmmp/soundcore.h>
#include <qmmp/decoder.h>
#include <qmmpui/general.h>
#include <qmmpui/playlistparser.h>
#include <qmmpui/playlistformat.h>
#include <qmmpui/filedialog.h>
#include <qmmpui/playlistmodel.h>
#include <qmmpui/mediaplayer.h>
#include <qmmpui/generalhandler.h>
#include "abstractplaylistmodel.h"
#include "playlistitemdelegate.h"
#include "mainwindow.h"

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
    //m_generalHandler = new GeneralHandler(this);


    //connections
    connect(ui.actionPlay, SIGNAL(triggered()), m_player, SLOT(play()));
    connect(ui.actionPause, SIGNAL(triggered()), m_core, SLOT(pause()));
    connect(ui.actionNext, SIGNAL(triggered()), m_player, SLOT(next()));
    connect(ui.actionPrevious, SIGNAL(triggered()), m_player, SLOT(previous()));
    connect(ui.actionStop, SIGNAL(triggered()), m_player, SLOT(stop()));
    connect(ui.actionOpen, SIGNAL(triggered()),SLOT(addFiles()));
    connect(ui.actionClear, SIGNAL(triggered()),m_pl_manager,SLOT(clear()));
    connect(m_core, SIGNAL(elapsedChanged(qint64)), SLOT(updatePosition(qint64)));
    connect(m_core, SIGNAL(stateChanged(Qmmp::State)), SLOT(showState(Qmmp::State)));
    connect(m_core, SIGNAL(bitrateChanged(int)), SLOT(showBitrate(int)));
    foreach(PlayListModel *model, m_pl_manager->playLists())
    {
        QTreeView *view = new QTreeView(this);
        view->setRootIsDecorated(false);
        AbstractPlaylistModel *m = new AbstractPlaylistModel(model, this);

        //view->setItemDelegate(new PlaylistDelegate(this));
        view->setModel(m);
        ui.tabWidget->addTab(view, model->name());
    }


    /*AbstractPlaylistModel *m = new AbstractPlaylistModel(m_model, this);
    ui.listView->setItemDelegate(new PlaylistDelegate(this));
    ui.listView->setModel(m);
    connect(m_model, SIGNAL(listChanged()), ui.listView, SLOT(reset()));
    connect(ui.listView, SIGNAL(doubleClicked (const QModelIndex &)),
                                SLOT (playSelected(const QModelIndex &)));*/

    m_slider = new QSlider (Qt::Horizontal, this);
    m_label = new QLabel(this);
    m_label->setText("--:--/--:--");
    ui.toolBar->addWidget(m_slider);
    ui.toolBar->addWidget(m_label);

    connect(m_slider, SIGNAL(sliderReleased()), SLOT(seek()));
}

MainWindow::~MainWindow()
{
}

void MainWindow::addFiles()
{
    /*QString lastDir;
    QStringList filters;
    filters << tr("All Supported Bitstreams")+" (" + Decoder::nameFilters().join (" ") +")";
    filters << Decoder::filters();
    FileDialog::popup(this, FileDialog::AddDirsFiles, &lastDir,
                            m_model, SLOT(addFileList(const QStringList&)),
                            tr("Select one or more files to open"), filters.join(";;"));*/
}

void MainWindow::playSelected(const QModelIndex &i)
{
    m_player->stop();
    //m_model->setCurrent(i.row());
    m_player->play();
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

void MainWindow::showBitrate(int)
{
    ui.statusbar->showMessage(QString(tr("Playing [%1 kbps/%2 bit/%3]")).arg(m_core->bitrate())
                                    .arg(m_core->precision())
                                    .arg(m_core->channels() > 1 ? tr("Stereo"):tr("Mono")));
}

void MainWindow::initPlayLists()
{


}
