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
#include <QContextMenuEvent>
#include <QMenu>
#include <QLineEdit>
#include <QInputDialog>
#include <QIcon>
#include <qmmp/qmmp.h>
#include <qmmpui/qmmpuisettings.h>
#include <qmmpui/columnmanager.h>
#include "skin.h"
#include "playlistheader.h"

#define PADDING 3

PlayListHeader::PlayListHeader(QWidget *parent) :
    QWidget(parent)
{
    m_metrics = 0;
    m_show_number = false;
    m_align_numbres = false;
    m_number_width = 0;
    m_task = NO_TASK;
    m_manager = QmmpUiSettings::instance()->columnManager();
    m_skin = Skin::instance();
    m_menu = new QMenu(this);
    m_menu->addAction(QIcon::fromTheme("list-add"), tr("Add column"), this, SLOT(addColumn()));
    m_menu->addAction(QIcon::fromTheme("configure"), tr("Edit column"), this, SLOT(editColumn()));
    m_menu->addSeparator();
    m_menu->addAction(QIcon::fromTheme("list-remove"), tr("Remove column"), this, SLOT(removeColumn()));

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

void PlayListHeader::addColumn()
{
    int column = findColumn(m_pressed_pos);
    if(column < 0 && m_pressed_pos.x() > m_rects.last().right())
        column = m_manager->count();
    else if(column < 0 && m_pressed_pos.x() < m_rects.first().x())
        column = 0;

    if(column < 0)
        return;

    m_manager->insert(column, tr("Title"), "%t");
    m_manager->execEditor(column);
}

void PlayListHeader::editColumn()
{
    int column = findColumn(m_pressed_pos);
    if(column >= 0)
    {
        m_manager->execEditor(column);
    }
}

void PlayListHeader::removeColumn()
{
    int column = findColumn(m_pressed_pos);
    if(column >= 0)
    {
        m_manager->remove(column);
    }
}

void PlayListHeader::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton)
    {
        m_pressed_column = findColumn(e->pos());
        if(m_pressed_column >= 0)
        {
            m_pressed_pos = e->pos();
            m_mouse_pos = e->pos();

            if(e->pos().x() > m_rects[m_pressed_column].right() - m_metrics->width("9"))
            {
                m_old_size = m_manager->size(m_pressed_column);
                m_task = RESIZE;
            }
            else
            {
                m_press_offset = e->pos().x() - m_rects.at(m_pressed_column).x();
                m_task = MOVE;
            }
        }
    }
    else
    {
        m_task = NO_TASK;
        update();
    }
}

void PlayListHeader::mouseReleaseEvent(QMouseEvent *)
{
    m_task = NO_TASK;
    update();
}

void PlayListHeader::mouseMoveEvent(QMouseEvent *e)
{
    if(m_task == RESIZE && m_manager->count() > 1)
    {
        m_manager->resize(m_pressed_column, m_old_size + e->pos().x() - m_pressed_pos.x());
    }
    else if(m_task == MOVE)
    {
        m_mouse_pos = e->pos();

        int dest = -1;
        for(int i = 0; i < m_rects.count(); ++i)
        {
            int x_delta = m_mouse_pos.x() - m_rects.at(i).x();
            if(x_delta < 0 || x_delta > m_rects.at(i).width())
                continue;

            if((x_delta > m_rects.at(i).width()/2 && m_pressed_column < i) ||
                    (x_delta < m_rects.at(i).width()/2 && m_pressed_column > i))
            {
                dest = i;
                break;
            }
        }
        if(dest == -1 || dest == m_pressed_column)
        {
            update();
            QWidget::mouseMoveEvent(e);
            return;
        }
        m_manager->move(m_pressed_column, dest);
        m_pressed_column = dest;
        update();
    }
}

void PlayListHeader::resizeEvent(QResizeEvent *)
{
    if(m_manager->count() == 1)
        updateColumns();
}

void PlayListHeader::contextMenuEvent(QContextMenuEvent *e)
{
    m_pressed_pos = e->pos();
    m_menu->exec(e->globalPos());
}

void PlayListHeader::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    painter.setBrush(m_normal);
    painter.setPen(m_normal);
    painter.drawRect(5,-1,width()-10,height()+1);

    painter.setPen(m_normal_bg);

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
        if(m_task == MOVE && i == m_pressed_column)
        {
            painter.setBrush(m_normal_bg);
            painter.setPen(m_current);
            painter.drawRect(m_rects[i].x() - m_metrics->width("9")/2, 0,
                             m_rects[i].width(), height()-1);
            painter.setBrush(m_normal);
            painter.setPen(m_normal_bg);
            continue;
        }

        painter.drawText((m_rects[i].x() + m_rects[i].right())/2 - m_metrics->width(m_names[i])/2,
                         m_metrics->ascent(), m_names[i]);

        painter.drawLine(m_rects[i].right() - m_metrics->width("9")/2, 0,
                         m_rects[i].right() - m_metrics->width("9")/2, height()+1);

    }

    if(m_task == MOVE)
    {
        painter.setPen(m_normal);
        painter.drawRect(m_mouse_pos.x() - m_press_offset - m_metrics->width("9")/2, 0,
                         m_rects.at(m_pressed_column).width(), height());

        painter.setPen(m_normal_bg);
        painter.drawText(m_mouse_pos.x() - m_press_offset + m_rects[m_pressed_column].width()/2 -
                         m_metrics->width(m_names[m_pressed_column])/2,
                         m_metrics->ascent(), m_names.at(m_pressed_column));
    }
}

void PlayListHeader::loadColors()
{
    m_normal.setNamedColor(m_skin->getPLValue("normal"));
    m_normal_bg.setNamedColor(m_skin->getPLValue("normalbg"));
    m_current.setNamedColor(m_skin->getPLValue("current"));
}

int PlayListHeader::findColumn(QPoint pos)
{
    for(int i = 0; i < m_rects.count(); ++i)
    {
        if(m_rects.at(i).contains(pos))
            return i;
    }
    return -1;
}
