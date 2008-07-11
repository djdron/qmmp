/***************************************************************************
 *   Copyright (C) 2008 by Ilya Kotov                                      *
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

#include <QVBoxLayout>
#include <QLabel>
#include <QApplication>
#include <QDesktopWidget>
#include <QTimer>
#include <QPalette>
#include <QSettings>
#include <QDir>

#include "popupwidget.h"

PopupWidget::PopupWidget(const SongInfo &song, QWidget *parent)
        : QFrame(parent)
{
    setWindowFlags(Qt::X11BypassWindowManagerHint |
                   Qt::WindowStaysOnTopHint);
    setFrameStyle(QFrame::Box | QFrame::Plain);
    QString title = song.title();
    if(title.isEmpty())
        title = song.path().section('/',-1);
    title.append(" ");
    if (!song.isStream())
        title.append(QString("(%1:%2)").arg(song.length()/60).arg(song.length()%60, 2, 10, QChar('0')));

    QHBoxLayout *hlayout = new QHBoxLayout(this);
    QLabel *pixlabel = new QLabel(this);
    pixlabel->setPixmap(QPixmap(":/notifier_icon.png"));
    pixlabel->setFixedSize(32,32);
    hlayout->addWidget(pixlabel);

    QVBoxLayout *vlayout = new QVBoxLayout();
    QLabel *label1 = new QLabel("<b>"+title+"</b>", this);
    vlayout->addWidget(label1);

    QString info = song.artist();
    if (!info.isEmpty() && !song.album().isEmpty())
        info.append(" - " + song.album());
    if (!info.isEmpty())
    {
        QLabel *label2 = new QLabel(info, this);
        vlayout->addWidget(label2);
    }

    hlayout->addLayout (vlayout);
    setLayout(hlayout);
    resize(sizeHint());
    QSettings settings(QDir::homePath()+"/.qmmp/qmmprc", QSettings::IniFormat);
    settings.beginGroup("Notifier");
    int delay = settings.value("message_delay", 2000).toInt();
    uint pos = settings.value("message_pos", PopupWidget::BOTTOMLEFT).toUInt();
    settings.endGroup();
    //calculate widget position
    int x = 5, y = 5;
    QSize desktopSize = QApplication::desktop()->size();
    if (pos == LEFT || pos == RIGHT || pos == CENTER)
        y = desktopSize.height()/2 - height()/2;
    else if (pos == BOTTOMLEFT || pos == BOTTOM || pos == BOTTOMRIGHT)
        y = desktopSize.height() - height() - 5;
    if (pos == TOP || pos == BOTTOM || pos == CENTER)
        x = desktopSize.width()/2 - width()/2;
    else if (pos == TOPRIGHT || pos == RIGHT || pos == BOTTOMRIGHT)
        x = desktopSize.width() - width() - 5;

    move (x,y);
    QTimer::singleShot(delay, this, SLOT(deleteLater()));
    show();
}

PopupWidget::~PopupWidget()
{}

void PopupWidget::mousePressEvent (QMouseEvent *)
{
    deleteLater();
}

