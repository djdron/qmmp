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
#include <QBitmap>
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
#include <qmmpui/playlistheadermodel.h>
#include <qmmpui/playlistmanager.h>
#include "skin.h"
#include "playlistheader.h"

static const char * const skinned_arrow_down_xpm[] = {
    "11 6 2 1",
    " 	c None",
    "x	c #000000",
    "           ",
    " xxxxxxxxx ",
    "  xxxxxxx  ",
    "   xxxxx   ",
    "    xxx    ",
    "     x     "};

static const char * const skinned_arrow_up_xpm[] = {
    "11 6 2 1",
    " 	c None",
    "x	c #000000",
    "     x     ",
    "    xxx    ",
    "   xxxxx   ",
    "  xxxxxxx  ",
    " xxxxxxxxx ",
    "           "};

PlayListHeader::PlayListHeader(QWidget *parent) :
    QWidget(parent)
{
    setMouseTracking(true);
    m_metrics = 0;
    m_padding = 0;
    m_show_number = false;
    m_align_numbres = false;
    m_number_width = 0;
    m_task = NO_TASK;
    m_model = QmmpUiSettings::instance()->headerModel();
    m_skin = Skin::instance();
    m_menu = new QMenu(this);
    m_menu->addAction(QIcon::fromTheme("list-add"), tr("Add column"), this, SLOT(addColumn()));
    m_menu->addAction(QIcon::fromTheme("configure"), tr("Edit column"), this, SLOT(editColumn()));
    m_autoResize = m_menu->addAction(tr("Auto-resize"), this, SLOT(setAutoResize(bool)));
    m_autoResize->setCheckable(true);
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
    QFont pl_font;
    pl_font.fromString(settings.value("pl_font", qApp->font().toString()).toString());
    m_font.fromString(settings.value("header_font", qApp->font().toString()).toString());

    if (m_metrics)
    {
        delete m_metrics;
        m_metrics = 0;
    }

    m_metrics = new QFontMetrics(pl_font);
    m_show_number = settings.value ("pl_show_numbers", true).toBool();
    m_align_numbres = settings.value ("pl_align_numbers", false).toBool();
    m_padding = m_metrics->width("9")/2;

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

    int sx = 5;
    if(m_number_width)
        sx += m_number_width + 2 * m_padding;

    if(m_model->count() == 1)
    {
        m_rects << QRect(sx, 0, width() - sx - 5, height());
        m_names << m_model->name(0);
        return;
    }

    for(int i = 0; i < m_model->count(); ++i)
    {
        m_rects << QRect(sx, 0, m_model->size(i), height());
        m_names << m_metrics->elidedText(m_model->name(i), Qt::ElideRight,
                                         m_model->size(i) - 2 * m_padding);

        sx += m_model->size(i);
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
        column = m_model->count();
    else if(column < 0 && m_pressed_pos.x() < m_rects.first().x())
        column = 0;

    if(column < 0)
        return;

    m_model->execInsert(column);
}

void PlayListHeader::editColumn()
{
    if(m_pressed_column < 0)
         return;

    m_model->execEdit(m_pressed_column);
}

void PlayListHeader::removeColumn()
{
    if(m_pressed_column < 0)
         return;

    m_model->remove(m_pressed_column);
}

void PlayListHeader::setAutoResize(bool yes)
{
   if(m_pressed_column < 0)
        return;

   m_model->setAutoResize(yes ? m_pressed_column : -1);
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
                m_old_size = m_model->size(m_pressed_column);
                m_task = RESIZE;
            }
            else
            {
                m_press_offset = e->pos().x() - m_rects.at(m_pressed_column).x();
                m_task = SORT;
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
    if(m_task == SORT)
    {
        qDebug("sort column");
        PlayListManager::instance()->selectedPlayList()->sortByColumn(m_pressed_column);
    }
    m_task = NO_TASK;
    update();
}

void PlayListHeader::mouseMoveEvent(QMouseEvent *e)
{
    if(m_task == SORT)
    {
        m_task = MOVE;
    }
    if(m_task == RESIZE && m_model->count() > 1)
    {
        m_model->resize(m_pressed_column, m_old_size + e->pos().x() - m_pressed_pos.x());
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
        m_model->move(m_pressed_column, dest);
        m_pressed_column = dest;
        update();
    }
    else if(e->button() == Qt::NoButton)
    {
        int column = findColumn(e->pos());
        if(column >= 0 && e->pos().x() > m_rects[column].right() - m_metrics->width("9"))
            setCursor(Qt::SplitHCursor);
        else
            setCursor(Qt::ArrowCursor);
    }
}

void PlayListHeader::resizeEvent(QResizeEvent *e)
{
    if(m_model->count() == 1)
    {
        updateColumns();
        return;
    }

    if(e->oldSize().width() <= 10)
        return;

    int delta = e->size().width() - e->oldSize().width();
    int index = m_model->autoResizeColumn();
    if(index >= 0)
    {
        m_model->resize(index, m_model->size(index) + delta);
        updateColumns();
    }
}

void PlayListHeader::contextMenuEvent(QContextMenuEvent *e)
{
    m_pressed_pos = e->pos();
    m_pressed_column = findColumn(e->pos());
    if(m_pressed_column >= 0)
    {
        m_autoResize->setChecked(m_model->autoResizeColumn() == m_pressed_column);
        m_autoResize->setEnabled(true);
        foreach (QAction *action, m_menu->actions())
            action->setVisible(true);

    }
    else
    {
        foreach (QAction *action, m_menu->actions())
        {
            if(action != m_menu->actions().first())
                action->setVisible(false);
        }
    }
    m_menu->exec(e->globalPos());
}

void PlayListHeader::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    painter.setBrush(m_normal);
    painter.setPen(m_normal);
    painter.setFont(m_font);
    painter.drawRect(5,-1,width()-10,height()+1);

    painter.setPen(m_normal_bg);

    if(m_number_width)
    {
        painter.drawLine(m_rects.at(0).x(), 0,
                         m_rects.at(0).x(), height());
    }

    if(m_names.count() == 1)
    {
        painter.drawText(m_rects[0].x() + m_padding, m_metrics->ascent(), m_names[0]);
        return;
    }

    for(int i = 0; i < m_rects.count(); ++i)
    {
        if(m_task == MOVE && i == m_pressed_column)
        {
            painter.setBrush(m_normal_bg);
            painter.setPen(m_current);
            painter.drawRect(m_rects[i].x(), 0,
                             m_rects[i].width(), height()-1);
            painter.setBrush(m_normal);
            painter.setPen(m_normal_bg);
            continue;
        }

        painter.drawText(m_rects[i].x() + m_padding, m_metrics->ascent(), m_names[i]);

        painter.drawLine(m_rects[i].right()+1, 0,
                         m_rects[i].right()+1, height()+1);

        painter.drawPixmap(m_rects[i].right() - m_arrow_up.width() - 4, (height() - m_arrow_up.height()) / 2, m_arrow_up);

    }

    if(m_task == MOVE)
    {
        painter.setPen(m_normal);
        painter.drawRect(m_mouse_pos.x() - m_press_offset, 0,
                         m_rects.at(m_pressed_column).width(), height());

        painter.setPen(m_normal_bg);
        painter.drawText(m_mouse_pos.x() - m_press_offset + m_padding,
                         m_metrics->ascent(), m_names.at(m_pressed_column));
    }
}

void PlayListHeader::loadColors()
{
    m_normal.setNamedColor(m_skin->getPLValue("normal"));
    m_normal_bg.setNamedColor(m_skin->getPLValue("normalbg"));
    m_current.setNamedColor(m_skin->getPLValue("current"));

    QPixmap px1(skinned_arrow_up_xpm);
    QPixmap px2(skinned_arrow_up_xpm);
    m_arrow_up = px1;
    m_arrow_down = px2;
    m_arrow_up.fill(m_normal_bg);
    m_arrow_down.fill(m_normal_bg);
    m_arrow_up.setMask(px1.createMaskFromColor(Qt::transparent));
    m_arrow_down.setMask(px2.createMaskFromColor(Qt::transparent));
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
