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
#include <QStyleOptionHeader>
#include <qmmp/qmmp.h>
#include <qmmpui/qmmpuisettings.h>
#include <qmmpui/playlistheadermodel.h>
#include <qmmpui/playlistmanager.h>
#include "playlistheader.h"

PlayListHeader::PlayListHeader(QWidget *parent) :
    QWidget(parent)
{
    setMouseTracking(true);
    m_metrics = new QFontMetrics(qApp->font());
    m_pl_padding = 0;
    m_number_width = 0;
    m_sorting_column = -1;

    m_task = NO_TASK;
    m_model = QmmpUiSettings::instance()->headerModel();
    m_menu = new QMenu(this);
    m_menu->addAction(QIcon::fromTheme("list-add"), tr("Add Column"), this, SLOT(addColumn()));
    m_menu->addAction(QIcon::fromTheme("configure"), tr("Edit Column"), this, SLOT(editColumn()));
    m_autoResize = m_menu->addAction(tr("Auto-resize"), this, SLOT(setAutoResize(bool)));
    m_autoResize->setCheckable(true);
    m_menu->addAction(tr("Restore Size"), this, SLOT(restoreSize()));
    m_menu->addSeparator();
    m_menu->addAction(QIcon::fromTheme("list-remove"), tr("Remove Column"), this, SLOT(removeColumn()));

    QStyleOptionHeader opt;
    opt.initFrom(this);
    m_size_hint = style()->sizeFromContents(QStyle::CT_HeaderSection, &opt, QSize(), this);

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
    settings.beginGroup("Simple");
    m_font.fromString(settings.value("header_font", qApp->font().toString()).toString());

    QFont pl_font;
    pl_font.fromString(settings.value("pl_font", qApp->font().toString()).toString());
    m_pl_padding = QFontMetrics(pl_font).width("9")/2;

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
    bool rtl = (layoutDirection() == Qt::RightToLeft);

    int sx = 5;
    if(m_number_width)
        sx += m_number_width + 2 * m_pl_padding;

    if(m_model->count() == 1)
    {
        m_rects << (rtl ? QRect(5, 0, width() - sx - 5, height()) : QRect(sx, 0, width() - sx - 5, height()));
        m_names << m_model->name(0);
        return;
    }

    for(int i = 0; i < m_model->count(); ++i)
    {
        if(rtl)
            m_rects << QRect(width() - sx - m_model->size(i), 0, m_model->size(i), height());
        else
            m_rects << QRect(sx, 0, m_model->size(i), height());
        m_names << m_model->name(i);
        sx += m_model->size(i);
    }
    update();
}

int PlayListHeader::requiredHeight() const
{
    return m_size_hint.height();
}

void PlayListHeader::showSortIndicator(int column, bool reverted)
{
    if(m_sorting_column == column && m_reverted == reverted)
        return;

    m_sorting_column = column;
    m_reverted = reverted;
    updateColumns();
}

void PlayListHeader::hideSortIndicator()
{
    if(m_sorting_column != -1)
    {
        m_sorting_column = -1;
        updateColumns();
    }
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

void PlayListHeader::restoreSize()
{
    if(m_pressed_column < 0)
         return;

    m_model->resize(m_pressed_column, 100);
}

void PlayListHeader::mousePressEvent(QMouseEvent *e)
{
    bool rtl = layoutDirection() == Qt::RightToLeft;

    if(e->button() == Qt::LeftButton)
    {
        m_pressed_column = findColumn(e->pos());
        if(m_pressed_column >= 0)
        {
            m_pressed_pos = e->pos();
            m_mouse_pos = e->pos();

            if(rtl)
            {
                if(e->pos().x() < m_rects[m_pressed_column].x() + m_metrics->width("9"))
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
            else
            {
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
}

void PlayListHeader::mouseReleaseEvent(QMouseEvent *)
{
    if(m_task == SORT)
    {
        PlayListManager::instance()->selectedPlayList()->sortByColumn(m_pressed_column);
    }
    m_task = NO_TASK;
    update();
}

void PlayListHeader::mouseMoveEvent(QMouseEvent *e)
{
    bool rtl = layoutDirection() == Qt::RightToLeft;

    if(m_task == SORT)
    {
        m_task = MOVE;
    }

    if(m_task == RESIZE && m_model->count() > 1)
    {
        if(rtl)
            m_model->resize(m_pressed_column, m_old_size - e->pos().x() + m_pressed_pos.x());
        else
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

            if(rtl)
            {
                if((x_delta > m_rects.at(i).width()/2 && m_pressed_column > i) ||
                        (x_delta < m_rects.at(i).width()/2 && m_pressed_column < i))
                {
                    dest = i;
                    break;
                }
            }
            else
            {
                if((x_delta > m_rects.at(i).width()/2 && m_pressed_column < i) ||
                        (x_delta < m_rects.at(i).width()/2 && m_pressed_column > i))
                {
                    dest = i;
                    break;
                }
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
        if(rtl)
        {
            if(column >= 0 && e->pos().x() < m_rects[column].x() + m_metrics->width("9"))
                setCursor(Qt::SplitHCursor);
            else
                setCursor(Qt::ArrowCursor);
        }
        else
        {
            if(column >= 0 && e->pos().x() > m_rects[column].right() - m_metrics->width("9"))
                setCursor(Qt::SplitHCursor);
            else
                setCursor(Qt::ArrowCursor);
        }
    }
}

void PlayListHeader::resizeEvent(QResizeEvent *e)
{
    if(m_model->count() == 1)
    {
        updateColumns();
        return;
    }

    int delta = e->size().width() - e->oldSize().width();
    int index = m_model->autoResizeColumn();

    if(layoutDirection() == Qt::RightToLeft)
       updateColumns();

    if(e->oldSize().width() <= 10)
        return;

    if(index >= 0)
    {
        m_model->resize(index, m_model->size(index) + delta);
        updateColumns();
        return;
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

    {
        QStyleOption opt;
        opt.initFrom(this);
        opt.state |= QStyle::State_Horizontal;
        opt.rect = QRect(0,0,m_rects.first().x(), height());
        style()->drawControl(QStyle::CE_HeaderEmptyArea, &opt, &painter, this);
        opt.rect = QRect(m_rects.last().right(), 0,  width() - m_rects.last().right(), height());
        style()->drawControl(QStyle::CE_HeaderEmptyArea, &opt, &painter, this);
    }

    for(int i = 0; i < m_rects.count(); ++i)
    {
        QStyleOptionHeader opt;
        initStyleOption(&opt);
        opt.rect = m_rects[i];
        opt.text = m_names[i];
        opt.section = i;
        opt.state |= QStyle::State_Active;
        if(i == 0)
            opt.position = QStyleOptionHeader::Beginning;
        else if(i < m_rects.count() - 1)
            opt.position = QStyleOptionHeader::Middle;
        else if(i == m_rects.count() - 1)
            opt.position = QStyleOptionHeader::End;

        if(i == m_sorting_column)
            opt.sortIndicator = m_reverted ? QStyleOptionHeader::SortUp : QStyleOptionHeader::SortDown;

        style()->drawControl(QStyle::CE_Header, &opt, &painter, this);
    }

    if(m_names.count() == 1)
        return;

    if(m_task == MOVE)
    {
        QStyleOptionHeader opt;
        initStyleOption(&opt);
        opt.rect = m_rects[m_pressed_column];
        opt.text = m_names[m_pressed_column];
        opt.section = m_pressed_column;
        painter.setOpacity(0.75);
        opt.rect.moveTo(m_mouse_pos.x() - m_press_offset, opt.rect.y());
        style()->drawControl(QStyle::CE_Header, &opt, &painter, this);
    }
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

void PlayListHeader::initStyleOption(QStyleOptionHeader *opt)
{
    opt->initFrom(this);
    opt->state = QStyle::State_None | QStyle::State_Raised | QStyle::State_Horizontal | QStyle::State_Enabled;
    opt->orientation = Qt::Horizontal;
    opt->iconAlignment = Qt::AlignVCenter;
    opt->textAlignment = Qt::AlignLeft | Qt::AlignVCenter;
}
