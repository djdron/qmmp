/***************************************************************************
 *   Copyright (C) 2008-2010 by Ilya Kotov                                 *
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
#include <QTimer>
#include <QSettings>
#include <QApplication>
#include <qmmp/soundcore.h>
#include <qmmp/metadatamanager.h>
#include <qmmpui/metadataformatter.h>
#include <qmmpui/playlistitem.h>

#include "popupwidget.h"

using namespace PlayListPopup;

PopupWidget::PopupWidget(QWidget *parent)
        : QFrame(parent)
{
    setWindowFlags(Qt::X11BypassWindowManagerHint |
                   Qt::WindowStaysOnTopHint | Qt::Dialog | Qt::FramelessWindowHint);
    setFrameStyle(QFrame::Box | QFrame::Plain);
    setAttribute(Qt::WA_QuitOnClose, FALSE);
    m_lastItem = 0;
    QHBoxLayout *hlayout = new QHBoxLayout(this); //layout
    m_pixlabel = new QLabel(this);
    m_pixlabel->setPixmap(QPixmap(":/32x32/qmmp.png"));
    m_pixlabel->setFixedSize(32,32);
    hlayout->addWidget(m_pixlabel);

    m_label1 = new QLabel(this);
    hlayout->addWidget (m_label1);

    //settings
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("PlayList");
    setWindowOpacity(settings.value("popup_opacity", 1.0).toDouble());
    m_coverSize = settings.value("popup_cover_size", 48).toInt();
    m_template = settings.value("popup_template",DEFAULT_TEMPLATE).toString();
    settings.endGroup();
    //timer
    m_timer = new QTimer(this);
    m_timer->setInterval(10000);
    m_timer->setSingleShot (TRUE);
    connect(m_timer, SIGNAL(timeout ()), SLOT(hide()));
}

PopupWidget::~PopupWidget()
{}

void PopupWidget::mousePressEvent (QMouseEvent *)
{
    deleteLater();
}

void PopupWidget::popup(PlayListItem *item, QPoint pos)
{
    if(m_lastItem == item)
    {
        show();
        m_timer->start();
        return;
    }
    m_timer->stop();
    m_lastItem = item;
    move(pos);
    QString title = m_template;
    MetaDataFormatter f(title);
    title = f.parse(item);

    m_label1->setText(title);

    QPixmap pix = MetaDataManager::instance()->getCover(item->url());
    if(!pix.isNull())
    {
        m_pixlabel->setFixedSize(m_coverSize,m_coverSize);
        m_pixlabel->setPixmap(pix.scaled(m_coverSize,m_coverSize));
    }
    else
    {
        m_pixlabel->setPixmap(QPixmap(":/notifier_icon.png"));
        m_pixlabel->setFixedSize(32,32);
    }
    qApp->processEvents();
    resize(sizeHint());
    qApp->processEvents();
    show();
    m_timer->start();
}
