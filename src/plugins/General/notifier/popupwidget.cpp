/***************************************************************************
 *   Copyright (C) 2008-2009 by Ilya Kotov                                 *
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
#include <QApplication>
#include <qmmp/soundcore.h>

#include "popupwidget.h"

PopupWidget::PopupWidget(QWidget *parent)
        : QFrame(parent)
{
    setWindowFlags(Qt::X11BypassWindowManagerHint |
                   Qt::WindowStaysOnTopHint | Qt::Dialog | Qt::FramelessWindowHint);
    setFrameStyle(QFrame::Box | QFrame::Plain);

    QHBoxLayout *hlayout = new QHBoxLayout(this);
    QLabel *pixlabel = new QLabel(this);
    pixlabel->setPixmap(QPixmap(":/notifier_icon.png"));
    pixlabel->setFixedSize(32,32);
    hlayout->addWidget(pixlabel);
    //layout
    QVBoxLayout *vlayout = new QVBoxLayout();
    hlayout->addLayout (vlayout);
    setLayout(hlayout);
    //first line
    m_label1 = new QLabel(this);
    vlayout->addWidget(m_label1);
    //second line
    m_label2 = new QLabel(this);
    vlayout->addWidget(m_label2);
    //resize(sizeHint()); //update size hint
    //settings
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("Notifier");
    int delay = settings.value("message_delay", 2000).toInt();
    m_pos = settings.value("message_pos", PopupWidget::BOTTOMLEFT).toUInt();
    setWindowOpacity(settings.value("opacity", 1.0).toDouble());
    settings.endGroup();
    //timer
    m_timer = new QTimer(this);
    m_timer->setInterval(delay);
    m_timer->setSingleShot (TRUE);
    connect(m_timer, SIGNAL(timeout ()), SLOT(deleteLater()));
}

PopupWidget::~PopupWidget()
{}

void PopupWidget::mousePressEvent (QMouseEvent *)
{
    deleteLater();
}

void PopupWidget::showMetaData()
{
    m_timer->stop();
    SoundCore *core = SoundCore::instance();
    QString title = core->metaData(Qmmp::TITLE);
    if (title.isEmpty())
        title = core->metaData(Qmmp::URL).section('/',-1);
    if (core->totalTime() > 0)
    {
        title.append(" ");
        title.append(QString("(%1:%2)").arg(core->totalTime()/60000).arg(core->totalTime()%60000/1000, 2, 10, QChar('0')));
    }
    m_label1->setText("<b>" + title + "</b>");

    QString info = core->metaData(Qmmp::ARTIST);
    if (!info.isEmpty() && !core->metaData(Qmmp::ALBUM).isEmpty())
        info.append(" - " + core->metaData(Qmmp::ALBUM));
    if (!info.isEmpty())
    {
        m_label2->setText(info);
        m_label2->show();
    }
    else
        m_label2->hide();
    qApp->processEvents();
    resize(sizeHint());
    qApp->processEvents();
    updatePosition();
    qApp->processEvents();
    show();
    m_timer->start();
}

void PopupWidget::showVolume(int v)
{
    m_timer->stop();
    m_label1->setText("<b>" + tr("Volume:") + QString (" %1\%").arg(v)+ + "</b>");

    m_label2->hide();
    qApp->processEvents();
    resize(sizeHint());
    qApp->processEvents();
    updatePosition();
    qApp->processEvents();
    show();
    m_timer->start();
}

void PopupWidget::updatePosition()
{
    //calculate widget position
    QRect desktopRect = QApplication::desktop()->availableGeometry();
    int x = desktopRect.x() + 5, y = desktopRect.y() + 5;
    if (m_pos == LEFT || m_pos == RIGHT || m_pos == CENTER)
        y =  desktopRect.y() + desktopRect.height()/2 - height()/2 + 5;
    else if (m_pos == BOTTOMLEFT || m_pos == BOTTOM || m_pos == BOTTOMRIGHT)
        y = desktopRect.y() + desktopRect.height() - height() - 5;
    if (m_pos == TOP || m_pos == BOTTOM || m_pos == CENTER)
        x = desktopRect.x() + desktopRect.width()/2 - width()/2;
    else if (m_pos == TOPRIGHT || m_pos == RIGHT || m_pos == BOTTOMRIGHT)
        x = desktopRect.x() + desktopRect.width() - width() - 5;
    move (x,y);
}
