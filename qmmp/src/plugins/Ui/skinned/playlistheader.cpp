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
#include <qmmpui/qmmpuisettings.h>
#include <qmmpui/playlistmodel.h>
#include <qmmpui/columnmanager.h>
#include "skin.h"
#include "playlistheader.h"

#define PADDING 3

PlayListHeader::PlayListHeader(QWidget *parent) :
    QWidget(parent)
{
    m_scrollable = false;
    m_resize = false;
    m_metrics = 0;
    m_model = 0;
    m_show_number = false;
    m_align_numbres = false;
    m_number_width = 0;
    m_manager = QmmpUiSettings::instance()->columnManager();
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
    settings.beginGroup("Skinned");
    m_font.fromString(settings.value("pl_font", QApplication::font().toString()).toString());

    if (m_metrics)
    {
        delete m_metrics;
        m_metrics = 0;
    }

    m_metrics = new QFontMetrics(m_font);
    m_show_number = settings.value ("pl_show_numbers", true).toBool();
    m_align_numbres = settings.value ("pl_align_numbers", false).toBool();
    settings.endGroup();
    updateColumns();
}

void PlayListHeader::setNumberWidth(int width)
{
    if(width != m_number_width)
    {
        m_number_width = width;
        updateColumns();
    }
}

void PlayListHeader::updateColumns()
{
    m_rects.clear();
    m_names.clear();

    int sx = 5 + PADDING;
    if(m_number_width)
        sx += m_number_width + m_metrics->width("9");

    if(m_manager->count() == 1)
    {
        m_rects << QRect(sx, 0, width() - sx - 5, height());
        m_names << m_manager->name(0);
        return;
    }

    for(int i = 0; i < m_manager->count(); ++i)
    {
        m_rects << QRect(sx, 0, m_manager->size(i)+1, height());
        m_names << m_metrics->elidedText(m_manager->name(i), Qt::ElideRight,
                                         m_manager->size(i) - m_metrics->width("9"));

        sx += m_manager->size(i);
    }
    update();
}

void PlayListHeader::updateSkin()
{
    loadColors();
    update();
}

void PlayListHeader::mousePressEvent(QMouseEvent *e)
{
    for(int i = 0; i < m_rects.count(); ++i)
    {
        if(m_rects.at(i).contains(e->pos()))
        {
            m_pressed_pos = e->pos();

            if(e->pos().x() > m_rects[i].right() - m_metrics->width("9"))
            {
                m_pressed_index = i;
                m_size = m_manager->size(i);
                m_resize = true;
                qDebug("resize = %d", i);
            }
            else
                qDebug("move = %d", i);
        }
    }
}

void PlayListHeader::mouseReleaseEvent(QMouseEvent *e)
{
    m_resize = false;
}

void PlayListHeader::mouseMoveEvent(QMouseEvent *e)
{
    if(m_resize && m_pressed_index >= 0)
    {
        //qDebug("delta = %d", e->pos().x() - m_pressed_pos.x());
        m_manager->resize(m_pressed_index, m_size + e->pos().x() - m_pressed_pos.x());
        updateColumns();
    }
}

void PlayListHeader::resizeEvent(QResizeEvent *)
{
    if(m_manager->count() == 1)
        updateColumns();
}

void PlayListHeader::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    painter.setBrush(m_normal);
    painter.setPen(m_normal);
    painter.drawRect(5,-1,width()-10,height()+1);

    painter.setPen(m_selected_bg);

    if(m_number_width)
    {
        painter.drawLine(m_rects.at(0).x() - m_metrics->width("9")/2 - 1, 0,
                         m_rects.at(0).x() - m_metrics->width("9")/2 - 1, height());
    }

    if(m_names.count() == 1)
    {
        painter.drawText((m_rects[0].x() + m_rects[0].right())/2 - m_metrics->width(m_names[0])/2,
                         m_metrics->ascent(), m_names[0]);
        return;
    }

    for(int i = 0; i < m_rects.count(); ++i)
    {
        painter.drawText((m_rects[i].x() + m_rects[i].right())/2 - m_metrics->width(m_names[i])/2,
                         m_metrics->ascent(), m_names[i]);

        painter.drawLine(m_rects[i].right() - m_metrics->width("9")/2, 0,
                         m_rects[i].right() - m_metrics->width("9")/2, height()+1);

    }
}

void PlayListHeader::loadColors()
{
    m_normal.setNamedColor(m_skin->getPLValue("normal"));
    m_selected_bg.setNamedColor(m_skin->getPLValue("selectedbg"));
}
