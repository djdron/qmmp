/***************************************************************************
 *   Copyright (C) 2007-2009 by Ilya Kotov                                 *
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
#include <QMouseEvent>
#include <QMenu>
#include <QSettings>
#include "eqwidget.h"
#include "skin.h"
#include "shadedbar.h"
#include "dock.h"
#include "mainwindow.h"
#include "button.h"
#include "eqtitlebar.h"

EqTitleBar::EqTitleBar(QWidget *parent)
        : PixmapWidget(parent)
{
    m_volumeBar = 0;
    m_balanceBar = 0;
    m_shade2 = 0;
    m_left = 0;
    m_right = 0;
    m_shaded = false;
    m_align = false;
    m_skin = Skin::instance();
    m_eq = parentWidget();
    m_mw = qobject_cast<MainWindow*>(m_eq->parent());
    m_close = new Button(this, Skin::EQ_BT_CLOSE_N, Skin::EQ_BT_CLOSE_P, Skin::CUR_EQCLOSE);
    connect(m_close, SIGNAL(clicked()),m_eq, SIGNAL(closed()));
    m_shade = new Button(this, Skin::EQ_BT_SHADE1_N, Skin::EQ_BT_SHADE1_P, Skin::CUR_EQNORMAL);
    connect(m_shade, SIGNAL(clicked()), SLOT(shade()));
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    if (settings.value("Equalizer/shaded", false).toBool())
        shade();
    m_align = true;
    setActive(false);
    setCursor(m_skin->getCursor(Skin::CUR_EQTITLE));
    connect(m_skin, SIGNAL(skinChanged()), SLOT(updateSkin()));
    updatePositions();
}


EqTitleBar::~EqTitleBar()
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.setValue("Equalizer/shaded", m_shaded);
}

void EqTitleBar::updatePositions()
{
     int r = m_skin->ratio();
     m_close->move(r*264,r*3);
     m_shade->move(r*254,r*3);
     if(m_volumeBar)
         m_volumeBar->move(r*61,r*4);
     if(m_balanceBar)
         m_balanceBar->move(r*164,r*4);
     if(m_shade2)
         m_shade2->move(r*254,r*3);
}

void EqTitleBar::setActive(bool active)
{
    if (active)
    {
        if (m_shaded)
            setPixmap(m_skin->getEqPart(Skin::EQ_TITLEBAR_SHADED_A));
        else
            setPixmap(m_skin->getEqPart(Skin::EQ_TITLEBAR_A));
    }
    else
    {
        if (m_shaded)
            setPixmap(m_skin->getEqPart(Skin::EQ_TITLEBAR_SHADED_I));
        else
            setPixmap(m_skin->getEqPart(Skin::EQ_TITLEBAR_I));
    }
}

void EqTitleBar::setVolume(int left, int right)
{
    m_left = left;
    m_right = right;
    if (m_volumeBar && m_balanceBar)
    {
        int maxVol = qMax(left, right);
        m_volumeBar->setValue(maxVol);
        if (maxVol && !m_volumeBar->isPressed())
            m_balanceBar->setValue((right - left)*100/maxVol);
    }
}

void EqTitleBar::mousePressEvent(QMouseEvent* event)
{
    switch ((int) event->button ())
    {
    case Qt::LeftButton:
        m_pos = event->pos();
        break;
    case Qt::RightButton:
        m_mw->menu()->exec(event->globalPos());
    }
}

void EqTitleBar::mouseMoveEvent(QMouseEvent* event)
{
    if (m_pos.x() < width() - 30 * m_skin->ratio())
    {
        QPoint npos = (event->globalPos()-m_pos);
        Dock::instance()->move(m_eq, npos);
    }
}

void EqTitleBar::mouseReleaseEvent(QMouseEvent*)
{
    Dock::instance()->updateDock();
}

void EqTitleBar::shade()
{
    m_shaded = !m_shaded;
    int r = m_skin->ratio();

    if (m_shaded)
    {
        setPixmap(m_skin->getEqPart(Skin::EQ_TITLEBAR_SHADED_A));
        m_shade->hide();
        m_shade2 = new Button(this, Skin::EQ_BT_SHADE2_N, Skin::EQ_BT_SHADE2_P, Skin::CUR_EQNORMAL);
        m_shade2->move(r*254,r*3);
        connect(m_shade2, SIGNAL(clicked()), SLOT(shade()));
        m_shade2->show();
        m_volumeBar = new ShadedBar(this, Skin::EQ_VOLUME1, Skin::EQ_VOLUME2, Skin::EQ_VOLUME3);
        m_volumeBar->move(r*61,r*4);
        m_volumeBar->show();
        connect(m_volumeBar, SIGNAL(sliderMoved(int)),SLOT(updateVolume()));
        m_balanceBar = new ShadedBar(this, Skin::EQ_BALANCE1, Skin::EQ_BALANCE2, Skin::EQ_BALANCE3);
        m_balanceBar->move(r*164,r*4);
        m_balanceBar->setRange(-100, 100);
        m_balanceBar->show();
        connect(m_balanceBar, SIGNAL(sliderMoved(int)),SLOT(updateVolume()));
        setVolume(m_left, m_right); //show current volume and balance
    }
    else
    {
        setPixmap(m_skin->getEqPart(Skin::EQ_TITLEBAR_A));
        m_shade2->deleteLater();
        m_volumeBar->deleteLater();
        m_balanceBar->deleteLater();
        m_volumeBar = 0;
        m_balanceBar = 0;
        m_shade2 = 0;
        m_shade->show();
    }
    qobject_cast<EqWidget *>(m_eq)->setMimimalMode(m_shaded);
    if (m_align)
        Dock::instance()->align(m_eq, m_shaded? -102*r: 102*r);
}

void EqTitleBar::updateVolume()
{
    m_mw->setVolume(m_volumeBar->value(), m_balanceBar->value());
}

void EqTitleBar::updateSkin()
{
    setCursor(m_skin->getCursor(Skin::CUR_EQTITLE));
    updatePositions();
}
