/***************************************************************************
 *   Copyright (C) 2015 by Ilya Kotov                                      *
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
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#include <QPainter>
#include <QFont>
#include <QFontMetrics>
#include <QSettings>
#include <QApplication>
#include <QMouseEvent>
#include <QMenu>
#include <QLineEdit>
#include <QInputDialog>
#include <qmmp/qmmp.h>
#include <qmmpui/playlistmanager.h>
#include "skin.h"
#include "playlistheader.h"

PlayListHeader::PlayListHeader(QWidget *parent) :
    QWidget(parent)
{
    m_scrollable = false;
    m_metrics = 0;
    m_skin = Skin::instance();
    connect(m_skin, SIGNAL(skinChanged()), this, SLOT(updateSkin()));
    loadColors();
    readSettings();
}

PlayListHeader::~PlayListHeader()
{
    if (m_metrics)
        delete m_metrics;
    m_metrics = 0;
}

void PlayListHeader::readSettings()
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    m_font.fromString(settings.value("Skinned/pl_font", QApplication::font().toString()).toString());
    if (m_metrics)
    {
        delete m_metrics;
        m_metrics = 0;
    }
    m_metrics = new QFontMetrics(m_font);
    resize(width(), m_metrics->height () +1);
    //drawButtons();
}

void PlayListHeader::updateSkin()
{
    loadColors();
    //drawButtons();
}

void PlayListHeader::paintEvent(QPaintEvent *)
{
    /*QPainter painter(this);
    painter.setFont(m_font);
    painter.setBrush(QBrush(m_normal_bg));
    painter.drawRect(-1,-1,width()+1,height()+1);

    if(m_moving)
    {
        painter.setBrush(QBrush(m_normal_bg));
        painter.setPen(m_current);
        painter.drawRect(m_rects.at(selected).x() - 2 - m_offset, 0,
                         m_rects.at(selected).width() + 3, height()-1);
    }
    else
    {
        painter.setBrush(QBrush(m_selected_bg));
        painter.setPen(m_selected_bg);
        painter.drawRect(m_rects.at(selected).x() - 2 - m_offset, 0,
                         m_rects.at(selected).width() + 3, height()-1);
    }

    for (int i = 0; i < m_rects.size(); ++i)
    {
        if(i == current)
             painter.setPen(m_current);
        else
             painter.setPen(m_normal);

        if(!m_moving || i != selected)
            painter.drawText(m_rects[i].x() - m_offset, m_metrics->ascent(), names.at(i));
        if(i < m_rects.size() - 1)
        {
            painter.setPen(m_normal);
            painter.drawText(m_rects[i].x() + m_rects[i].width() - m_offset, m_metrics->ascent(),
                             m_pl_separator);
        }
    }


    for(int i = 0; i < m_extra_rects.size(); ++i)
    {
        painter.setPen(m_pressed_button == BUTTON_NEW_PL ? m_current : m_normal);
        painter.drawText(m_extra_rects[i].x() - m_offset, m_metrics->ascent(), m_pl_button);
    }


    if(m_moving)
    {
        painter.setBrush(QBrush(m_selected_bg));
        painter.setPen(m_selected_bg);
        painter.drawRect(m_mouse_pos.x() - m_press_offset - 2, 0,
                         m_rects.at(selected).width() + 3, height());

        painter.setPen(selected == current ? m_current : m_normal);
        painter.drawText(m_mouse_pos.x() - m_press_offset,
                         m_metrics->ascent(), names.at(selected));
    }

    if(m_scrollable)
    {
        painter.drawPixmap(width()-40, 0, m_pixmap);
        painter.setBrush(QBrush(m_normal_bg));
        painter.setPen(m_normal_bg);
        painter.drawRect(0,0,6,height());
    }*/
}

void PlayListHeader::loadColors()
{
    m_normal.setNamedColor(m_skin->getPLValue("normal"));
    m_current.setNamedColor(m_skin->getPLValue("current"));
    m_normal_bg.setNamedColor(m_skin->getPLValue("normalbg"));
    m_selected_bg.setNamedColor(m_skin->getPLValue("selectedbg"));
}
