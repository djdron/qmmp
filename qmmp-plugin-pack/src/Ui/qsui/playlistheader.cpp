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
#include <qmmpui/playlistmanager.h>
#include <qmmpui/playlistheadermodel.h>
#include <qmmpui/playlistmanager.h>
#include "playlistheader.h"

#define INITAL_SIZE 150
#define INITAL_MIN_SIZE 30

PlayListHeader::PlayListHeader(QWidget *parent) :
    QWidget(parent)
{
    setMouseTracking(true);

    m_pl_padding = 0;
    m_number_width = 0;
    m_sorting_column = -1;
    m_reverted = false;
    m_metrics = 0;
    m_task = NO_TASK;

    m_model = PlayListManager::instance()->headerModel();
    m_menu = new QMenu(this);
    m_menu->addAction(QIcon::fromTheme("list-add"), tr("Add Column"), this, SLOT(addColumn()));
    m_menu->addAction(QIcon::fromTheme("configure"), tr("Edit Column"), this, SLOT(editColumn()));
    m_autoResize = m_menu->addAction(tr("Auto-resize"), this, SLOT(setAutoResize(bool)));
    m_autoResize->setCheckable(true);
    m_menu->addAction(tr("Restore Size"), this, SLOT(restoreSize()));
    m_menu->addSeparator();
    m_menu->addAction(QIcon::fromTheme("list-remove"), tr("Remove Column"), this, SLOT(removeColumn()));

    readSettings();

    connect(m_model, SIGNAL(columnAdded(int)), SLOT(onColumnAdded(int)));
    connect(m_model, SIGNAL(columnRemoved(int)), SLOT(updateColumns()));
    connect(m_model, SIGNAL(columnMoved(int,int)), SLOT(onColumnMoved(int,int)));
    connect(m_model, SIGNAL(columnChanged(int)), SLOT(updateColumns()));
}

PlayListHeader::~PlayListHeader()
{
    if (m_metrics)
        delete m_metrics;
    m_metrics = 0;
}

void PlayListHeader::readSettings()
{
    if (m_metrics)
    {
        delete m_metrics;
        m_metrics = 0;
    }

    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("Simple");

    QFont header_font = qApp->font("QAbstractItemView");
    if(!settings.value("use_system_fonts", true).toBool())
    {
        header_font.fromString(settings.value("pl_header_font", header_font.toString()).toString());
    }
    m_metrics = new QFontMetrics(header_font);
    setFont(header_font);

    QStyleOptionHeader opt;
    opt.initFrom(this);
    m_size_hint = style()->sizeFromContents(QStyle::CT_HeaderSection, &opt, QSize(), this);

    QFont pl_font;
    pl_font.fromString(settings.value("pl_font", qApp->font().toString()).toString());
    m_pl_padding = QFontMetrics(pl_font).width("9")/2;

    if(!m_model->isSettingsLoaded()) //do not load settings several times
    {
        m_model->restoreSettings(&settings);
        QList<QVariant> sizes = settings.value("pl_column_sizes").toList();
        int autoResizeColumn = settings.value("pl_autoresize_column", -1).toInt();
        for(int i = 0; i < m_model->count(); ++i)
        {
            m_model->setData(i, SIZE, INITAL_SIZE);
            m_model->setData(i, MIN_SIZE, INITAL_MIN_SIZE);

            if(i < sizes.count())
                m_model->setData(i, SIZE, sizes.at(i).toInt());
            if(i == autoResizeColumn)
                m_model->setData(i, AUTO_RESIZE, true);
        }
    }

    if(isVisible())
        updateColumns();

    settings.endGroup();

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
    if(!isVisible())
        return;

    m_model->setData(0, MIN_SIZE, INITAL_MIN_SIZE + (m_number_width ? (m_number_width + 2 * m_pl_padding) : 0));
    for(int i = 1; i < m_model->count(); ++i) //restore mimimal size for other columns
        m_model->setData(i, MIN_SIZE, 30);

    int s = qMax(size(0), minSize(0));
    m_model->setData(0, SIZE, s);

    bool rtl = (layoutDirection() == Qt::RightToLeft);

    int sx = 5;
    if(m_number_width)
        sx += m_number_width + 2 * m_pl_padding;

    if(m_model->count() == 1)
    {
        m_model->setData(0, RECT, rtl ? QRect(5, 0, width() - sx - 5, height()) : QRect(sx, 0, width() - sx - 5, height()));
        m_model->setData(0, NAME, m_model->name(0));
        return;
    }

    for(int i = 0; i < m_model->count(); ++i)
    {
        int size = m_model->data(i, SIZE).toInt();

        //add number width to the first column
        if(i == 0 && m_number_width)
            size -= m_number_width + 2 * m_pl_padding;

        if(rtl)
            m_model->setData(i, RECT, QRect(width() - sx - size, 0, size, height()));
        else
            m_model->setData(i, RECT, QRect(sx, 0, size, height()));
        m_model->setData(i, NAME, m_model->name(i));
        sx += size;
    }
    update();
}

int PlayListHeader::requiredHeight() const
{
    return m_size_hint.height();
}

QList<int> PlayListHeader::sizes() const
{
    QList<int> sizeList;
    for(int i = 0; i < m_model->count(); ++i)
        sizeList.append(m_model->data(i, SIZE).toInt());
    return sizeList;
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

void PlayListHeader::restoreSize()
{
    if(m_pressed_column < 0)
            return;

    m_model->setData(m_pressed_column, SIZE, INITAL_SIZE);
    updateColumns();
    emit resizeColumnRequest();
}

void PlayListHeader::onColumnAdded(int index)
{
    m_model->setData(index, SIZE, INITAL_SIZE);
    m_model->setData(index, MIN_SIZE, INITAL_MIN_SIZE);
    updateColumns();
}

void PlayListHeader::onColumnMoved(int from, int to)
{
    //correct geometry
    if(!isVisible())
        return;

    if(from == 0 && m_number_width)
    {
        setSize(from, size(from) + (m_number_width + 2 * m_pl_padding));
        setSize(to, size(to) - (m_number_width + 2 * m_pl_padding));
    }
    else if(to == 0 && m_number_width)
    {
        setSize(from, size(from) - (m_number_width + 2 * m_pl_padding));
        setSize(to, size(to) + (m_number_width + 2 * m_pl_padding));
    }
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
        setSize(m_pressed_column, qMax(size(m_pressed_column), minSize(m_pressed_column)));
        updateColumns();
        emit resizeColumnRequest();
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
        setSize(index, qMax(minSize(index), size(index) + delta));
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
        m_autoResize->setChecked(m_model->data(m_pressed_column, AUTO_RESIZE).toBool());
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
        opt.rect = QRect(0,0,m_model->data(0, RECT).toRect().x(), height());
        style()->drawControl(QStyle::CE_HeaderEmptyArea, &opt, &painter, this);
        opt.rect = QRect(m_model->data(m_model->count() - 1, RECT).toRect().right(), 0,
                         width() - m_model->data(m_model->count() - 1, RECT).toRect().right(), height());
        style()->drawControl(QStyle::CE_HeaderEmptyArea, &opt, &painter, this);
    }

    for(int i = 0; i < m_model->count(); ++i)
    {
        QStyleOptionHeader opt;
        initStyleOption(&opt);
        opt.rect = m_model->data(i, RECT).toRect();
        opt.text = name(i);
        opt.section = i;
        opt.state |= QStyle::State_Active;
        if(i == 0)
            opt.position = QStyleOptionHeader::Beginning;
        else if(i < m_model->count() - 1)
            opt.position = QStyleOptionHeader::Middle;
        else if(i == m_model->count() - 1)
            opt.position = QStyleOptionHeader::End;

        if(i == m_sorting_column)
            opt.sortIndicator = m_reverted ? QStyleOptionHeader::SortUp : QStyleOptionHeader::SortDown;

        style()->drawControl(QStyle::CE_Header, &opt, &painter, this);
    }

    if(m_model->count() == 1)
        return;

    if(m_task == MOVE)
    {
        QStyleOptionHeader opt;
        initStyleOption(&opt);
        opt.rect = m_model->data(m_pressed_column, RECT).toRect();
        opt.text = name(m_pressed_column);
        opt.section = m_pressed_column;
        painter.setOpacity(0.75);
        opt.rect.moveTo(m_mouse_pos.x() - m_press_offset, opt.rect.y());
        style()->drawControl(QStyle::CE_Header, &opt, &painter, this);
    }
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

int PlayListHeader::minSize(int index) const
{
    return m_model->data(index, MIN_SIZE).toInt();
}

const QString PlayListHeader::name(int index) const
{
    return m_model->data(index, NAME).toString();
}

void PlayListHeader::writeSettings()
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("Simple");
    m_model->saveSettings(&settings);
    QList<QVariant> sizes;
    int autoResizeColumn = -1;
    for(int i = 0; i < m_model->count(); ++i)
    {
        sizes << m_model->data(i, SIZE).toInt();
        if(m_model->data(i, AUTO_RESIZE).toBool())
            autoResizeColumn = i;
    }
    settings.setValue("pl_column_sizes", sizes);
    settings.setValue("pl_autoresize_column", autoResizeColumn);
    settings.endGroup();
}

void PlayListHeader::showEvent(QShowEvent *)
{
    updateColumns();
}

void PlayListHeader::hideEvent(QHideEvent *)
{
    writeSettings();
}

void PlayListHeader::initStyleOption(QStyleOptionHeader *opt)
{
    opt->initFrom(this);
    opt->state = QStyle::State_None | QStyle::State_Raised | QStyle::State_Horizontal | QStyle::State_Enabled;
    opt->orientation = Qt::Horizontal;
    opt->iconAlignment = Qt::AlignVCenter;
    opt->textAlignment = Qt::AlignLeft | Qt::AlignVCenter;
}
