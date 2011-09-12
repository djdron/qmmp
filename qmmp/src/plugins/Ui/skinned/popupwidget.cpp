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
#include <QDesktopWidget>
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
        : QWidget(parent)
{
    setWindowFlags(Qt::X11BypassWindowManagerHint | Qt::Dialog | Qt::FramelessWindowHint);
    //setFrameStyle(QFrame::Box | QFrame::Plain);
    setAttribute(Qt::WA_QuitOnClose, false);
    m_item = 0;
    QHBoxLayout *hlayout = new QHBoxLayout(this); //layout
    m_pixlabel = new QLabel(this);
    hlayout->addWidget(m_pixlabel);

    m_label1 = new QLabel(this);
    hlayout->addWidget (m_label1);

    //settings
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("Skinned");
    setWindowOpacity(settings.value("popup_opacity", 1.0).toDouble());
    m_coverSize = settings.value("popup_cover_size", 48).toInt();
    m_template = settings.value("popup_template",DEFAULT_TEMPLATE).toString();
    int delay = settings.value("popup_delay", 2500).toInt();
    bool show_cover = settings.value("popup_show_cover",true).toBool();
    settings.endGroup();
    //timer
    m_timer = new QTimer(this);
    m_timer->setInterval(delay);
    m_timer->setSingleShot (true);
    connect(m_timer, SIGNAL(timeout ()), SLOT(show()));
    if(show_cover)
        connect(m_timer, SIGNAL(timeout ()), SLOT(loadCover()));
    else
        m_pixlabel->hide();
    setMouseTracking(true);
}

PopupWidget::~PopupWidget()
{}

void PopupWidget::mousePressEvent (QMouseEvent *)
{
    hide();
}

void PopupWidget::mouseMoveEvent (QMouseEvent *)
{
    hide();
}

void PopupWidget::prepare(PlayListItem *item, QPoint pos)
{
    pos += QPoint(15,10);

    m_item = item;
    hide();
    if(!item)
    {
        m_timer->stop();
        return;
    }

    QString title = m_template;
    MetaDataFormatter f(title);
    title = f.parse(item);
    m_label1->setText(title);
    qApp->processEvents();
    updateGeometry ();
    resize(sizeHint());
    qApp->processEvents();
    m_timer->start();
    QRect rect = QApplication::desktop()->availableGeometry(this);
    if(pos.x() + width() > rect.x() + rect.width())
        pos.rx() -= width();
    move(pos);
}

void PopupWidget::deactivate()
{
    m_timer->stop();
    hide();
}

PlayListItem *PopupWidget::item()
{
    return m_item;
}

void PopupWidget::loadCover()
{
    if(!m_item)
        return;
    QPixmap pix = MetaDataManager::instance()->getCover(m_item->url());
    if(pix.isNull())
        pix = QPixmap(":/skinned/ui_no_cover.png");
    m_pixlabel->setFixedSize(m_coverSize,m_coverSize);
    m_pixlabel->setPixmap(pix.scaled(m_coverSize,m_coverSize));
    qApp->processEvents();
    updateGeometry ();
    resize(sizeHint());
    qApp->processEvents();
}
