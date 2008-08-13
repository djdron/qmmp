/***************************************************************************
 *   Copyright (C) 2007-2008 by Ilya Kotov                                 *
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
    m_shaded = FALSE;
    m_align = FALSE;
    m_skin = Skin::getPointer();
    m_eq = parentWidget();
    m_mw = qobject_cast<MainWindow*>(m_eq->parent());
    m_close = new Button(this, Skin::EQ_BT_CLOSE_N, Skin::EQ_BT_CLOSE_P);
    connect(m_close, SIGNAL(clicked()),m_eq, SIGNAL(closed()));
    m_close->move(264,3);
    m_shade = new Button(this, Skin::EQ_BT_SHADE1_N, Skin::EQ_BT_SHADE1_P);
    connect(m_shade, SIGNAL(clicked()), SLOT(shade()));
    m_shade->move(254,3);
    QSettings settings(QDir::homePath()+"/.qmmp/qmmprc", QSettings::IniFormat);
    if (settings.value("Equalizer/shaded", FALSE).toBool())
        shade();
    else
        updateMask();
    m_align = TRUE;
    setActive(FALSE);
    connect(m_skin, SIGNAL(skinChanged()), SLOT(updateMask()));
}


EqTitleBar::~EqTitleBar()
{
    QSettings settings(QDir::homePath()+"/.qmmp/qmmprc", QSettings::IniFormat);
    settings.setValue("Equalizer/shaded", m_shaded);
}

void EqTitleBar::setActive(bool active)
{
    if (active)
    {
        if (m_shaded)
        {
            setPixmap(m_skin->getEqPart(Skin::EQ_TITLEBAR_SHADED_A));
            m_shade2->show();
        }
        else
        {
            setPixmap(m_skin->getEqPart(Skin::EQ_TITLEBAR_A));
            m_shade->show();
        }
        m_close->show();
    }
    else
    {
        if (m_shaded)
        {
            setPixmap(m_skin->getEqPart(Skin::EQ_TITLEBAR_SHADED_I));
            m_shade2->hide();
        }
        else
        {
            setPixmap(m_skin->getEqPart(Skin::EQ_TITLEBAR_I));
            m_shade->hide();
        }
        m_close->hide();
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
    {
        m_pos = event->pos();
        break;
    }
    case Qt::RightButton:
    {
        m_mw->menu()->exec(event->globalPos());
    }
    }
}

void EqTitleBar::mouseMoveEvent(QMouseEvent* event)
{
    QPoint npos = (event->globalPos()-m_pos);
    Dock::getPointer()->move(m_eq, npos);
}

void EqTitleBar::mouseReleaseEvent(QMouseEvent*)
{
    Dock::getPointer()->updateDock();
}

void EqTitleBar::shade()
{
    m_shaded = !m_shaded;

    if (m_shaded)
    {
        m_eq->setFixedSize(275,14);
        setPixmap(m_skin->getEqPart(Skin::EQ_TITLEBAR_SHADED_A));
        m_shade->hide();
        m_shade2 = new Button(this, Skin::EQ_BT_SHADE2_N, Skin::EQ_BT_SHADE2_P);
        m_shade2->move(254,3);
        connect(m_shade2, SIGNAL(clicked()), SLOT(shade()));
        m_shade2->show();
        m_volumeBar = new ShadedBar(this, Skin::EQ_VOLUME1, Skin::EQ_VOLUME2, Skin::EQ_VOLUME3);
        m_volumeBar->move(61,4);
        m_volumeBar->show();
        connect(m_volumeBar, SIGNAL(sliderMoved(int)),SLOT(updateVolume()));
        m_balanceBar = new ShadedBar(this, Skin::EQ_BALANCE1, Skin::EQ_BALANCE2, Skin::EQ_BALANCE3);
        m_balanceBar->move(164,4);
        m_balanceBar->setFixedSize(42,7);
        m_balanceBar->setRange(-100,100);
        m_balanceBar->show();
        connect(m_balanceBar, SIGNAL(sliderMoved(int)),SLOT(updateVolume()));
        setVolume(m_left, m_right); //show current volume and balance
    }
    else
    {
        m_eq->setFixedSize(275,116);
        setPixmap(m_skin->getEqPart(Skin::EQ_TITLEBAR_A));
        m_shade2->deleteLater();
        m_volumeBar->deleteLater();
        m_balanceBar->deleteLater();
        m_volumeBar = 0;
        m_balanceBar = 0;
        m_shade2 = 0;
        m_shade->show();
    }
    if (m_align)
        Dock::getPointer()->align(m_eq, m_shaded? -102: 102);
    updateMask();
}

void EqTitleBar::updateVolume()
{
    m_mw->setVolume(m_volumeBar->value(), m_balanceBar->value());
}

void EqTitleBar::updateMask()
{
    m_eq->clearMask();
    m_eq->setMask(QRegion(0,0,m_eq->width(),m_eq->height()));
    QRegion region = m_skin->getRegion(m_shaded? Skin::EQUALIZER_WS : Skin::EQUALIZER);
    if (!region.isEmpty())
        m_eq->setMask(region);
}
