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

#include <QPainter>
#include <QFont>
#include <QFontMetrics>
#include <QSettings>
#include <QApplication>
#include <QMouseEvent>
#include <qmmp/qmmp.h>
#include <qmmpui/playlistmanager.h>
#include "skin.h"
#include "playlistselector.h"

PlayListSelector::PlayListSelector(PlayListManager *manager, QWidget *parent) : QWidget(parent)
{
    m_update = FALSE;
    m_skin = Skin::instance();
    m_pl_manager = manager;
    connect(m_pl_manager, SIGNAL(playListsChanged()), SLOT(updateTabs()));
    connect(m_skin, SIGNAL(skinChanged()), this, SLOT(updateSkin()));
    readSettings();
    loadColors();
    updateTabs();
}

PlayListSelector::~PlayListSelector()
{}

void PlayListSelector::readSettings()
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    m_font.fromString(settings.value("PlayList/Font", QApplication::font().toString()).toString());
    if (m_update)
    {
        delete m_metrics;
        m_metrics = new QFontMetrics(m_font);
    }
    else
    {
        m_update = TRUE;
    }
    m_metrics = new QFontMetrics(m_font);

    resize(width(), m_metrics->height () +1);
}

void PlayListSelector::updateTabs()
{
    m_rects.clear();
    QRect rect;
    foreach(QString text, m_pl_manager->playListNames())
    {
        if(m_rects.isEmpty())
            rect.setX(9);
        else
            rect.setX(m_rects.last().x() + m_rects.last().width() + m_metrics->width(" | "));
        rect.setY(0);
        rect.setWidth(m_metrics->width(text));
        rect.setHeight(m_metrics->ascent ());
        m_rects.append(rect);
    }
    update();
}

void PlayListSelector::updateSkin()
{
    loadColors();
    updateTabs();
}

void PlayListSelector::paintEvent(QPaintEvent *)
{
    QPainter m_painter(this);
    m_painter.setFont(m_font);
    m_painter.setBrush(QBrush(m_normal_bg));
    m_painter.drawRect(-1,-1,width()+1,height()+1);
    QStringList names = m_pl_manager->playListNames();
    int current = m_pl_manager->indexOf(m_pl_manager->currentPlayList());
    int selected = m_pl_manager->indexOf(m_pl_manager->selectedPlayList());
    m_painter.setBrush(QBrush(m_selected_bg));
    m_painter.setPen(m_selected_bg);
    m_painter.drawRect(m_rects.at(selected).x()-3, 0, m_rects.at(selected).width()+4, height()-1);

    for (int i = 0; i < m_rects.size(); ++i)
    {
        if(i == current)
             m_painter.setPen(m_current);
        else
             m_painter.setPen(m_normal);
        m_painter.drawText(m_rects[i].x(), m_metrics->ascent(), names.at(i));
        if(i < m_rects.size() - 1)
        {
            m_painter.setPen(m_normal);
            m_painter.drawText(m_rects[i].x() + m_rects[i].width(), m_metrics->ascent(), " | ");
        }
    }
}

void PlayListSelector::mousePressEvent (QMouseEvent *e)
{
    for(int i = 0; i < m_rects.count(); ++i)
    {
        if(m_rects.at(i).contains(e->pos()))
        {
            m_pl_manager->selectPlayList(i);
            break;
        }
    }
}

void PlayListSelector::loadColors()
{
    m_normal.setNamedColor(m_skin->getPLValue("normal"));
    m_current.setNamedColor(m_skin->getPLValue("current"));
    m_normal_bg.setNamedColor(m_skin->getPLValue("normalbg"));
    m_selected_bg.setNamedColor(m_skin->getPLValue("selectedbg"));
}
