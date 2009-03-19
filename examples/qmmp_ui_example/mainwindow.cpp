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
#include <qmmp/soundcore.h>
#include <qmmp/decoder.h>
#include <qmmpui/general.h>
#include <qmmpui/playlistparser.h>
#include <qmmpui/playlistformat.h>
#include <qmmpui/filedialog.h>
#include <qmmpui/playlistmodel.h>
#include <qmmpui/mediaplayer.h>
#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
        : QWidget(parent)
{
    //control buttons
    QPushButton *play = new QPushButton("Play", this);
    QPushButton *pause = new QPushButton("Pause", this);
    QPushButton *next = new QPushButton("Next", this);
    QPushButton *previous = new QPushButton("Previous", this);
    QPushButton *stop = new QPushButton("Stop", this);
    QPushButton *open = new QPushButton("Open", this);
    QPushButton *clear = new QPushButton("Clear playlist",this);
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(play);
    layout->addWidget(pause);
    layout->addWidget(next);
    layout->addWidget(previous);
    layout->addWidget(stop);
    layout->addWidget(open);
    layout->addWidget(clear);
    //qmmp objects
    MediaPlayer *player = new MediaPlayer(this);
    SoundCore *core = new SoundCore(this);
    m_model = new PlayListModel(this);
    player->initialize(core, m_model);
    new PlaylistParser(this);
    //connections
    connect(play, SIGNAL(pressed()), player, SLOT(play()));
    connect(pause, SIGNAL(pressed()), core, SLOT(pause()));
    connect(next, SIGNAL(pressed()), player, SLOT(next()));
    connect(previous, SIGNAL(pressed()), player, SLOT(previous()));
    connect(stop, SIGNAL(pressed()), player, SLOT(stop()));
    connect(open,SIGNAL(pressed()),SLOT(addFiles()));
    connect(clear, SIGNAL(pressed()),m_model,SLOT(clear()));
}


MainWindow::~MainWindow()
{
}

void MainWindow::addFiles()
{
    QString lastDir;
    QStringList filters;
    filters << tr("All Supported Bitstreams")+" (" + Decoder::nameFilters().join (" ") +")";
    filters << Decoder::filters();
    FileDialog::popup(this, FileDialog::AddDirsFiles, &lastDir,
                            m_model, SLOT(addFileList(const QStringList&)),
                            tr("Select one or more files to open"), filters.join(";;"));
}
