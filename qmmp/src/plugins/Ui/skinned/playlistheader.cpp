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
#include <QHash>
#include <qmmp/qmmp.h>
#include <qmmpui/playlistmanager.h>
#include <qmmpui/playlistheadermodel.h>
#include "skin.h"
#include "playlistheader.h"

#define INITAL_SIZE 150
#define MAX_COLUMNS 7
#define MIN_SIZE 30

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
    m_pl_padding = 0;
    m_number_width = 0;
    m_sorting_column = -1;
    m_reverted = false;
    m_task = NO_TASK;
    m_model = PlayListManager::instance()->headerModel();
    m_skin = Skin::instance();
    m_menu = new QMenu(this);
    m_menu->addAction(QIcon::fromTheme("list-add"), tr("Add Column"), this, SLOT(addColumn()));
    m_menu->addAction(QIcon::fromTheme("configure"), tr("Edit Column"), this, SLOT(editColumn()));
    m_trackStateAction = m_menu->addAction(tr("Show Queue/Protocol"), this, SLOT(showTrackState(bool)));
    m_trackStateAction->setCheckable(true);
    m_autoResizeAction = m_menu->addAction(tr("Auto-resize"), this, SLOT(setAutoResize(bool)));
    m_autoResizeAction->setCheckable(true);
    m_menu->addAction(tr("Restore Size"), this, SLOT(restoreSize()));
    m_menu->addSeparator();
    m_menu->addAction(QIcon::fromTheme("list-remove"), tr("Remove Column"), this, SLOT(removeColumn()));

    connect(m_skin, SIGNAL(skinChanged()), this, SLOT(updateSkin()));
    connect(m_model, SIGNAL(columnAdded(int)), SLOT(onColumnAdded(int)));
    connect(m_model, SIGNAL(columnRemoved(int)), SLOT(updateColumns()));
    connect(m_model, SIGNAL(columnMoved(int,int)), SLOT(updateColumns()));
    connect(m_model, SIGNAL(columnChanged(int)), SLOT(updateColumns()));
    loadColors();
    readSettings();
}

PlayListHeader::~PlayListHeader()
{
    if (m_metrics)
        delete m_metrics;
    m_metrics = 0;
    writeSettings();
}

void PlayListHeader::readSettings()
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("Skinned");
    m_font.fromString(settings.value("pl_header_font", qApp->font().toString()).toString());

    if (m_metrics)
    {
        delete m_metrics;
        m_metrics = 0;
    }

    m_metrics = new QFontMetrics(m_font);
    m_padding = m_metrics->width("9")/2;

    QFont pl_font;
    pl_font.fromString(settings.value("pl_font", qApp->font().toString()).toString());
    m_pl_padding = QFontMetrics(pl_font).width("9")/2;

    if(!m_model->isSettingsLoaded())
    {
        m_model->restoreSettings(&settings);
        QList<QVariant> sizes = settings.value("pl_column_sizes").toList();
        int autoResizeColumn = settings.value("pl_autoresize_column", -1).toInt();
        int trackStateColumn = settings.value("pl_track_state_column", -1).toInt();
        for(int i = 0; i < m_model->count(); ++i)
        {
            m_model->setData(i, SIZE, INITAL_SIZE);

            if(i < sizes.count())
                m_model->setData(i, SIZE, sizes.at(i).toInt());
            if(i == autoResizeColumn)
                m_model->setData(i, AUTO_RESIZE, true);
            if(i == trackStateColumn)
                m_model->setData(i,TRACK_STATE, true);
        }
    }

    settings.endGroup();

    updateColumns();
}

void PlayListHeader::setNumberWidth(int width)
{
    if(width != m_number_width)
    {
        m_number_width = width;
        if(m_model->count() == 1)
            updateColumns();
    }
}

void PlayListHeader::updateColumns()
{
    bool rtl = (layoutDirection() == Qt::RightToLeft);

    int sx = 5;


    if(m_model->count() == 1)
    {
        if(m_number_width)
            sx += m_number_width + 2 * m_pl_padding;

        m_model->setData(0, RECT, rtl ? QRect(5, 0, width() - sx - 5, height()) : QRect(sx, 0, width() - sx - 5, height()));
        QRect rect = m_model->data(0, RECT).toRect();
        if(m_sorting_column == 0)
        {
            m_model->setData(0, NAME, m_metrics->elidedText(m_model->name(0), Qt::ElideRight,
                                                            rect.width() - 2 * m_padding - m_arrow_up.width() - 4));
        }
        else
        {
            m_model->setData(0, NAME, m_metrics->elidedText(m_model->name(0), Qt::ElideRight,
                                                            rect.width() - 2 * m_padding));
        }
        return;
    }

    for(int i = 0; i < m_model->count(); ++i)
    {
        int size = m_model->data(i, SIZE).toInt();

        if(rtl)
            m_model->setData(i, RECT, QRect(width() - sx - size, 0, size, height()));
        else
            m_model->setData(i, RECT, QRect(sx, 0, size, height()));
        if(i == m_sorting_column)
        {
            m_model->setData(i, NAME, m_metrics->elidedText(m_model->name(i), Qt::ElideRight,
                                                            size - 2 * m_padding - m_arrow_up.width() - 4));
        }
        else
        {
            m_model->setData(i, NAME, m_metrics->elidedText(m_model->name(i), Qt::ElideRight,
                                                            size - 2 * m_padding));
        }

        sx += size;
    }
    update();
}

int PlayListHeader::requiredHeight() const
{
    return m_metrics->lineSpacing() + 1;
}

QList<int> PlayListHeader::sizes() const
{
    QList<int> sizeList;
    for(int i = 0; i < m_model->count(); ++i)
        sizeList.append(m_model->data(i, SIZE).toInt());
    return sizeList;
}

int PlayListHeader::trackStateColumn() const
{
    for(int i = 0; i < m_model->count(); ++i)
    {
        if(m_model->data(i, TRACK_STATE).toBool())
        {
            return i;
        }
    }
    return -1;
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

void PlayListHeader::updateSkin()
{
    loadColors();
    update();
}

void PlayListHeader::addColumn()
{
    int column = findColumn(m_pressed_pos);

    if(column < 0)
    {
        QRect firstRect = m_model->data(0, RECT).toRect();
        QRect lastRect = m_model->data(m_model->count() - 1, RECT).toRect();

        if(m_pressed_pos.x() > lastRect.right())
            column = m_model->count();
        else if(m_pressed_pos.x() < firstRect.x())
            column = 0;
    }

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

   if(yes)
   {
       for(int i = 0; i < m_model->count(); ++i)
           m_model->setData(i, AUTO_RESIZE, false);
   }

   m_model->setData(m_pressed_column, AUTO_RESIZE, yes);
}

void PlayListHeader::showTrackState(bool yes)
{
    if(m_pressed_column < 0)
         return;

    if(yes)
    {
        for(int i = 0; i < m_model->count(); ++i)
            m_model->setData(i, TRACK_STATE, false);
    }

    m_model->setData(m_pressed_column, TRACK_STATE, yes);
    PlayListManager::instance()->selectedPlayList()->updateMetaData();
}

void PlayListHeader::restoreSize()
{
    if(m_pressed_column < 0)
        return;

    m_model->setData(m_pressed_column, SIZE, INITAL_SIZE);
    adjustColumns();
    updateColumns();
    PlayListManager::instance()->selectedPlayList()->updateMetaData();
}

void PlayListHeader::onColumnAdded(int index)
{
    m_model->setData(index, SIZE, INITAL_SIZE);
    adjustColumns();
    updateColumns();
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
                if(e->pos().x() < m_model->data(m_pressed_column, RECT).toRect().x() + m_metrics->width("9"))
                {
                    m_old_size = size(m_pressed_column);
                    m_task = RESIZE;
                }
                else
                {
                    m_press_offset = e->pos().x() - m_model->data(m_pressed_column, RECT).toRect().x();
                    m_task = SORT;
                }
            }
            else
            {
                if(e->pos().x() > m_model->data(m_pressed_column, RECT).toRect().right() - m_metrics->width("9"))
                {
                    m_old_size = size(m_pressed_column);
                    m_task = RESIZE;
                }
                else
                {
                    m_press_offset = e->pos().x() - m_model->data(m_pressed_column, RECT).toRect().x();
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
        PlayListManager::instance()->selectedPlayList()->sortByColumn(m_pressed_column);

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
            setSize(m_pressed_column, m_old_size - e->pos().x() + m_pressed_pos.x());
        else
            setSize(m_pressed_column, m_old_size + e->pos().x() - m_pressed_pos.x());
        setSize(m_pressed_column, qMax(size(m_pressed_column), MIN_SIZE));

        adjustColumns();
        updateColumns();
        PlayListManager::instance()->selectedPlayList()->updateMetaData();
    }
    else if(m_task == MOVE)
    {
        m_mouse_pos = e->pos();

        int dest = -1;
        for(int i = 0; i < m_model->count(); ++i)
        {
            QRect rect = m_model->data(i, RECT).toRect();
            int x_delta = m_mouse_pos.x() - rect.x();
            if(x_delta < 0 || x_delta > rect.width())
                continue;

            if(rtl)
            {
                if((x_delta > rect.width()/2 && m_pressed_column > i) ||
                        (x_delta < rect.width()/2 && m_pressed_column < i))
                {
                    dest = i;
                    break;
                }
            }
            else
            {
                if((x_delta > rect.width()/2 && m_pressed_column < i) ||
                        (x_delta < rect.width()/2 && m_pressed_column > i))
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
            if(column >= 0 && e->pos().x() < m_model->data(column, RECT).toRect().x() + m_metrics->width("9"))
                setCursor(Qt::SplitHCursor);
            else
                setCursor(Qt::ArrowCursor);
        }
        else
        {
            if(column >= 0 && e->pos().x() > m_model->data(column, RECT).toRect().right() - m_metrics->width("9"))
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
    int index = -1;
    for(int i = 0; i < m_model->count(); ++i)
    {
        if(m_model->data(i, AUTO_RESIZE).toBool())
        {
            index = i;
            break;
        }
    }

    if(index >= 0 && e->oldSize().width() > 10)
    {
        setSize(index, qMax(MIN_SIZE, size(index) + delta));
        adjustColumns();
        updateColumns();
        return;
    }

    if(adjustColumns())
    {
        updateColumns();
        return;
    }

    if(layoutDirection() == Qt::RightToLeft || e->oldSize().height() != e->size().height())
    {
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
        m_trackStateAction->setChecked(m_model->data(m_pressed_column, TRACK_STATE).toBool());
        m_autoResizeAction->setChecked(m_model->data(m_pressed_column, AUTO_RESIZE).toBool());
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
    painter.drawRect(5, -1, width() - 10, height() + 1);

    painter.setPen(m_normal_bg);

    bool rtl = (layoutDirection() == Qt::RightToLeft);

    QRect rect = m_model->data(0, RECT).toRect();

    if(rtl)
    {
        if(m_number_width)
        {
            painter.drawLine(rect.right(), 0, rect.right(), height());
        }

        if(m_model->count() == 1)
        {
            painter.drawText(rect.right() - m_padding - m_metrics->width(name(0)),
                    m_metrics->ascent(), name(0));
            if(m_sorting_column == 0)
            {
                painter.drawPixmap(rect.x() + 4,
                        (height() - m_arrow_up.height()) / 2,
                        m_reverted ? m_arrow_up : m_arrow_down);
            }
            return;
        }

        for(int i = 0; i < m_model->count(); ++i)
        {
            rect = m_model->data(i, RECT).toRect();

            if(m_task == MOVE && i == m_pressed_column)
            {
                painter.setBrush(m_normal_bg);
                painter.setPen(m_current);
                painter.drawRect(rect.x(), 0, rect.width(), height() - 1);
                painter.setBrush(m_normal);
                painter.setPen(m_normal_bg);
                continue;
            }

            painter.drawText(rect.right() - m_padding - m_metrics->width(name(i)),
                             m_metrics->ascent(), name(i));

            painter.drawLine(rect.x() - 1, 0, rect.x() - 1, height() + 1);

            if(i == m_sorting_column)
            {
                painter.drawPixmap(rect.x() + 4,
                                   (height() - m_arrow_up.height()) / 2,
                                   m_reverted ? m_arrow_up : m_arrow_down);
            }
        }

        if(m_task == MOVE)
        {
            painter.setPen(m_normal);
            painter.drawRect(m_mouse_pos.x() - m_press_offset, 0,
                             m_model->data(m_pressed_column, RECT).toRect().width(), height());

            painter.setPen(m_normal_bg);
            painter.drawText(m_mouse_pos.x() - m_press_offset +
                             m_model->data(m_pressed_column, RECT).toRect().width() - m_padding -
                             m_metrics->width(m_model->data(m_pressed_column, NAME).toString()),
                             m_metrics->ascent(), m_model->data(m_pressed_column, NAME).toString());
        }
    }
    else
    {
        if(m_model->count() == 1)
        {
            if(m_number_width)
            {
                painter.drawLine(rect.x(), 0, rect.x(), height());
            }

            painter.drawText(rect.x() + m_padding, m_metrics->ascent(), name(0));
            if(m_sorting_column == 0)
            {
                painter.drawPixmap(rect.right() - m_arrow_up.width() - 4,
                        (height() - m_arrow_up.height()) / 2,
                        m_reverted ? m_arrow_up : m_arrow_down);
            }
            return;
        }

        for(int i = 0; i < m_model->count(); ++i)
        {
            rect = m_model->data(i, RECT).toRect();

            if(m_task == MOVE && i == m_pressed_column)
            {
                painter.setBrush(m_normal_bg);
                painter.setPen(m_current);
                painter.drawRect(rect.x(), 0, rect.width(), height() - 1);
                painter.setBrush(m_normal);
                painter.setPen(m_normal_bg);
                continue;
            }

            painter.drawText(rect.x() + m_padding, m_metrics->ascent(), name(i));

            painter.drawLine(rect.right()+1, 0, rect.right() + 1, height() + 1);

            if(i == m_sorting_column)
            {
                painter.drawPixmap(rect.right() - m_arrow_up.width() - 4,
                                   (height() - m_arrow_up.height()) / 2,
                                   m_reverted ? m_arrow_up : m_arrow_down);
            }
        }

        if(m_task == MOVE)
        {
            painter.setPen(m_normal);
            painter.drawRect(m_mouse_pos.x() - m_press_offset, 0,
                             m_model->data(m_pressed_column, RECT).toRect().width(), height());

            painter.setPen(m_normal_bg);
            painter.drawText(m_mouse_pos.x() - m_press_offset + m_padding,
                             m_metrics->ascent(), m_model->data(m_pressed_column, NAME).toString());
        }
    }
}

void PlayListHeader::loadColors()
{
    m_normal.setNamedColor(m_skin->getPLValue("normal"));
    m_normal_bg.setNamedColor(m_skin->getPLValue("normalbg"));
    m_current.setNamedColor(m_skin->getPLValue("current"));

    QPixmap px1(skinned_arrow_up_xpm);
    QPixmap px2(skinned_arrow_down_xpm);
    m_arrow_up = px1;
    m_arrow_down = px2;
    m_arrow_up.fill(m_normal_bg);
    m_arrow_down.fill(m_normal_bg);
    m_arrow_up.setMask(px1.createMaskFromColor(Qt::transparent));
    m_arrow_down.setMask(px2.createMaskFromColor(Qt::transparent));
}

int PlayListHeader::findColumn(QPoint pos)
{
    for(int i = 0; i < m_model->count(); ++i)
    {
        if(m_model->data(i,RECT).toRect().contains(pos))
            return i;
    }
    return -1;
}

int PlayListHeader::size(int index) const
{
    return m_model->data(index, SIZE).toInt();
}

void PlayListHeader::setSize(int index, int size)
{
    m_model->setData(index, SIZE, size);
}

const QString PlayListHeader::name(int index) const
{
    return m_model->data(index, NAME).toString();
}

bool PlayListHeader::adjustColumns()
{
    int total_size = 0;
    foreach (int s, sizes())
    {
        total_size += s;
    }

    if(total_size > width() - 10)
    {
        int delta = total_size - width() + 10;
        for(int i = m_model->count() - 1; i >= 0 && delta > 0; i--)
        {
            int dx = size(i) - qMax(MIN_SIZE, size(i) - delta);
            setSize(i, qMax(MIN_SIZE, size(i) - delta));
            delta -= dx;
        }
        updateColumns();
        return true;
    }

    return false;
}

void PlayListHeader::writeSettings()
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("Skinned");
    m_model->saveSettings(&settings);
    QList<QVariant> sizes;
    int autoResizeColumn = -1;
    int trackStateColumn = -1;
    for(int i = 0; i < m_model->count(); ++i)
    {
       sizes << m_model->data(i, SIZE).toInt();
       if(m_model->data(i, AUTO_RESIZE).toBool())
           autoResizeColumn = i;
       if(m_model->data(i, TRACK_STATE).toBool())
           trackStateColumn = i;
    }
    settings.setValue("pl_column_sizes", sizes);
    settings.setValue("pl_autoresize_column", autoResizeColumn);
    settings.setValue("pl_track_state_column", trackStateColumn);
    settings.endGroup();
}
