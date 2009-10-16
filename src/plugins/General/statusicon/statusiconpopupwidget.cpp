/***************************************************************************
 *   Copyright (C) 2009 by Artur Guzik                                     *
 *   a.guzik88@gmail.com
 *
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

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QTimer>
#include <QPixmap>
#include <QFileInfo>
#include <QApplication>
#include <QDesktopWidget>
#include <QSpacerItem>
#include <QProgressBar>
#include <qmmp/soundcore.h>
#include <qmmp/metadatamanager.h>
#include "coverwidget.h"
#include "statusiconpopupwidget.h"

StatusIconPopupWidget::StatusIconPopupWidget(QWidget * parent)
        : QFrame(parent)
{
    setWindowFlags(Qt::X11BypassWindowManagerHint |
                   Qt::WindowStaysOnTopHint | Qt::Dialog | Qt::FramelessWindowHint);
    setFrameStyle(QFrame::NoFrame | QFrame::Plain);
    setAttribute(Qt::WA_QuitOnClose, FALSE);

    m_hLayout = new QHBoxLayout();
    m_vLayout = new QVBoxLayout();

    m_cover = new CoverWidget(this);
    m_cover->setFixedSize(100,100);
    m_hLayout->addWidget(m_cover);

    m_textLabel = new QLabel(this);
    m_vLayout->addWidget(m_textLabel);

    m_spacer = new QSpacerItem(20,0,QSizePolicy::Expanding,QSizePolicy::Expanding);
    m_vLayout->addItem(m_spacer);

    m_bar = new TimeBar(this);
    m_vLayout->addWidget(m_bar);

    m_hLayout->addLayout(m_vLayout);
    setLayout(m_hLayout);

    m_timer = new QTimer(this);
    m_timer->setSingleShot(TRUE);

    m_bar->setMinimumWidth(110);

    connect(m_timer,SIGNAL(timeout()),SLOT(deleteLater()));
    connect(SoundCore::instance(),SIGNAL(metaDataChanged()),this,SLOT(updateMetaData()));
    connect(SoundCore::instance(),SIGNAL(elapsedChanged(qint64)),this,SLOT(updateTime(qint64)));
    connect(SoundCore::instance(),SIGNAL(stateChanged(Qmmp::State)),this,SLOT(updateMetaData()));

    setVisible(FALSE);
}

StatusIconPopupWidget::~StatusIconPopupWidget()
{
}

void StatusIconPopupWidget::mousePressEvent(QMouseEvent *)
{
    deleteLater();
}

void StatusIconPopupWidget::updateMetaData()
{
    m_timer->stop();
    SoundCore *core = SoundCore::instance();
    if(core->state() == Qmmp::Playing || core->state() == Qmmp::Paused)
    {
        QString text;
        QString title = core->metaData(Qmmp::TITLE);
        QString artist = core->metaData(Qmmp::ARTIST);
        QString album = core->metaData(Qmmp::ALBUM);
        int year = core->metaData(Qmmp::YEAR).toInt();
        if(title.isEmpty())
        {
            title = QFileInfo(core->metaData(Qmmp::URL)).completeBaseName();
            if(m_splitFileName && title.contains("-"))
            {
                artist = title.section('-',0,0).trimmed();
                title = title.section('-',1,1).trimmed();
            }
        }
        text.append("<b>" + title + "</b>");
        if(core->totalTime() > 0)
        {
            text.append(" ");
            QString time;
            int l = core->totalTime()/1000;
            if(l > 3600)
                time += QString("(%1:%2:%3)").arg(l/3600,2,10,QChar('0')).arg(l%3600/60,2,10,QChar('0'))
                .arg(l%60,2,10,QChar('0'));

            else
                time = QString("(%1:%2)").arg(l/60,2,10,QChar('0')).arg(l%60,2,10,QChar('0'));
            text.append(time);
        }
        if(!artist.isEmpty())
        {
            text.append("<br>");
            text.append(artist);
        }
        if(!album.isEmpty())
        {
            text.append("<br>");
            text.append(album);
        }
        if(year > 0)
            text.append(QString("<br>%1").arg(year));

        m_textLabel->setText(text);
        QPixmap cover = MetaDataManager::instance()->getCover(core->metaData(Qmmp::URL));
        m_cover->show();
        m_bar->show();
        if(cover.isNull())
            m_cover->setPixmap(QPixmap(":/empty_cover.png"));
        else
            m_cover->setPixmap(cover);
        updateTime(core->elapsed());
    }
    else
    {
        m_cover->hide();
        m_bar->hide();
        m_textLabel->setText(tr("Stopped"));
    }
    qApp->processEvents();
    resize(sizeHint());
    qApp->processEvents();
    if(isVisible())
        updatePosition(m_lastTrayX,m_lastTrayY);
    m_timer->start();
}

void StatusIconPopupWidget::updateTime(qint64 elapsed)
{
    m_bar->setMaximum(SoundCore::instance()->totalTime()/1000);
    m_bar->setValue(elapsed/1000);
    m_bar->update();
}

void StatusIconPopupWidget::updatePosition(int trayx, int trayy)
{
    QRect screenGeometry = QApplication::desktop()->availableGeometry();
    int xpos = 0;
    int ypos = 0;

    xpos = screenGeometry.x() + trayx -5;
    if(xpos + width() > screenGeometry.width())
        xpos = screenGeometry.width() - width() -5;

    if(trayy < screenGeometry.y()) //tray on top of screen
    {
        ypos = screenGeometry.y() + 5;
    }

    if(trayy > screenGeometry.y()) //tray on bottom
    {
        ypos = screenGeometry.y() + screenGeometry.height() - height() -5;
    }
    move(xpos,ypos);
    return;
}

void StatusIconPopupWidget::showInfo(int x, int y, int delay, bool splitFileName)
{
    m_timer->stop();
    m_timer->setInterval(delay);
    m_lastTrayX = x;
    m_lastTrayY = y;
    m_splitFileName = splitFileName;
    updateMetaData();
    qApp->processEvents();
    updatePosition(x,y);
    qApp->processEvents();
    show();
    m_timer->start();
}

TimeBar::TimeBar(QWidget *parent) : QProgressBar(parent)
{}

QString TimeBar::text() const
{
    return QString("%1:%2").arg(value()/60,2,10,QChar('0')).arg(value()%60,2,10,QChar('0'));
}
