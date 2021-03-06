/***************************************************************************
 *   Copyright (C) 2006-2010 by Ilya Kotov                                 *
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
#include <QPixmap>
#include <QResizeEvent>
#include <QPainter>
#include <QFont>
#include <QFontMetrics>
#include <QSettings>
#include <QMenu>
#include <QUrl>
#include <QApplication>
#include <QHelpEvent>
#include <QTimer>
#include <QScrollBar>
#include <qmmpui/playlistitem.h>
#include <qmmpui/playlistmodel.h>
#include <qmmpui/mediaplayer.h>
#include <qmmpui/playlistmanager.h>
#include "listwidget.h"

#define INVALID_ROW -1

ListWidget::ListWidget(PlayListModel *model, QWidget *parent): QWidget(parent)
{
    m_update = false;
    //m_skin = Skin::instance();
    //m_popupWidget = 0;
    m_menu = new QMenu(this);
    m_scroll_direction = NONE;
    m_prev_y = 0;
    m_anchor_row = INVALID_ROW;
    m_player = MediaPlayer::instance();
    connect (m_player, SIGNAL(repeatableChanged(bool)), SLOT(updateList()));
    m_first = 0;
    m_rows = 0;
    m_scroll = false;
    m_select_on_release = false;
    readSettings();
    //connect(m_skin, SIGNAL(skinChanged()), this, SLOT(updateSkin()));
    setAcceptDrops(true);
    setMouseTracking(true);
    m_timer = new QTimer(this);
    m_timer->setInterval(50);
    connect(m_timer, SIGNAL(timeout()), SLOT(autoscroll()));
    m_model = model;
    connect (m_model, SIGNAL(currentChanged()), SLOT(recenterCurrent()));
    connect (m_model, SIGNAL(listChanged()), SLOT(updateList()));
    m_scrollBar = new QScrollBar(Qt::Vertical, this);
    connect(m_scrollBar, SIGNAL(valueChanged (int)), SLOT(scroll(int)));
}


ListWidget::~ListWidget()
{}

void ListWidget::readSettings()
{
    m_font = QApplication::font();
    m_show_protocol = true;
    m_show_number = true;
    //bool show_popup = settings.value("PlayList/show_popup", false).toBool();

    if (m_update)
    {
        delete m_metrics;
        m_metrics = new QFontMetrics(m_font);
        m_rows = (height() - 10) / m_metrics->ascent ();
        updateList();
        /*if(m_popupWidget)
        {
            m_popupWidget->deleteLater();
            m_popupWidget = 0;
        }*/
    }
    else
    {
        m_update = true;
        m_metrics = new QFontMetrics(m_font);
    }
    /*if(show_popup)
        m_popupWidget = new PlayListPopup::PopupWidget(this);*/
    m_normal = palette().color(QPalette::Text);
    m_current = palette().color(QPalette::Text);
    m_highlighted = palette().color(QPalette::HighlightedText);
    m_normal_bg = palette().color(QPalette::Base);
    m_selected_bg = palette().color(QPalette::Highlight);
}

void ListWidget::paintEvent(QPaintEvent *)
{
    QPainter m_painter(this);
    //m_painter.setPen(Qt::white);
    m_painter.setFont(m_font);
    m_painter.setBrush(QBrush(m_normal_bg));
    int x = width() - m_scrollBar->sizeHint().width();

    m_painter.drawRect(-1,-1, x + 1, height()+1);

    for (int i=0; i<m_titles.size(); ++i )
    {
        if(i % 2 == 0)
        {
            m_painter.setBrush(QBrush(palette().color(QPalette::AlternateBase)));
            m_painter.setPen(palette().color(QPalette::AlternateBase));
            m_painter.drawRect (6, 15+(i-1)*m_metrics->height () + 2, x - 10, m_metrics->height());
        }
        else
        {
            m_painter.setPen(m_normal_bg);
            m_painter.setBrush(QBrush(m_normal_bg));
            m_painter.drawRect (6, 15+(i-1)*m_metrics->height () + 2, x - 10, m_metrics->height());
        }

        if (m_model->isSelected(i + m_first))
        {
            m_painter.setBrush(palette().highlight ());
            m_painter.setPen(m_selected_bg);
            m_painter.drawRect (6, 15+(i-1)*m_metrics->height () + 2, x - 10, m_metrics->height());
        }

        if (m_model->currentRow() == i + m_first)
        {
            m_font.setBold(true);
            m_painter.setFont(m_font);
            m_font.setBold(false);
            m_painter.setPen(m_current);
        }
        if (m_model->isSelected(i + m_first))
            m_painter.setPen(m_highlighted);
        else
            m_painter.setPen(m_normal);


        m_painter.drawText(10,14+i*m_metrics->height(),m_titles.at(i));

        if (m_model->isQueued(m_model->item(i + m_first)) ||
                (m_player->isRepeatable() && (m_model->currentRow() == m_first + i)) || m_show_protocol)
        {
            QString extra_string = getExtraString(m_first + i);

            int old_size = m_font.pointSize();
            m_font.setPointSize(old_size - 1 );
            m_painter.setFont(m_font);

            m_painter.drawText(x - 10 - m_metrics->width(extra_string) - m_metrics->width(m_times.at(i)),
                               14+i*m_metrics->height (), extra_string);
            m_font.setPointSize(old_size);
            m_painter.setFont(m_font);
            m_painter.setBrush(QBrush(m_normal_bg));
        }

        m_painter.drawText(x - 7 - m_metrics->width(m_times.at(i)),
                           14+i*m_metrics->height (), m_times.at(i));
    }
}

void ListWidget::mouseDoubleClickEvent (QMouseEvent *e)
{
    int y = e->y();
    int row = rowAt(y);
    if (INVALID_ROW != row)
    {
        m_model->setCurrent(row);
        MediaPlayer::instance()->playListManager()->selectPlayList(m_model);
        MediaPlayer::instance()->playListManager()->activatePlayList(m_model);
        MediaPlayer::instance()->stop();
        MediaPlayer::instance()->play();
        emit selectionChanged();
        update();
    }
}


void ListWidget::mousePressEvent(QMouseEvent *e)
{
    /*if(m_popupWidget)
        m_popupWidget->hide();*/
    m_scroll = true;
    int y = e->y();
    int row = rowAt(y);

    if (INVALID_ROW != row && m_model->count() > row)
    {
        if (!(Qt::ControlModifier & e->modifiers () ||
                Qt::ShiftModifier & e->modifiers () ||
                m_model->isSelected(row)))
            m_model->clearSelection();

        if (m_model->isSelected(row) && (e->modifiers() == Qt::NoModifier))
            m_select_on_release = true;

        //qWarning("m_prev_clicked_row: %d",m_prev_clicked_row);

        m_pressed_row = row;
        if ((Qt::ShiftModifier & e->modifiers()))
        {

            if (m_pressed_row > m_anchor_row)
            {
                m_model->clearSelection();
                for (int j = m_anchor_row;j <= m_pressed_row;j++)
                {
                    m_model->setSelected(j, true);
                }
            }
            else
            {
                m_model->clearSelection();
                for (int j = m_anchor_row;j >= m_pressed_row;j--)
                {
                    m_model->setSelected(j, true);
                }
            }
        }
        else
        {
            if (!m_model->isSelected(row) || (Qt::ControlModifier & e->modifiers()))
                m_model->setSelected(row, !m_model->isSelected(row));
        }

        if (m_model->getSelection(m_pressed_row).count() == 1)
            m_anchor_row = m_pressed_row;
        //qWarning("m_anchor_row: %d",m_anchor_row);

        update();
    }
    QWidget::mousePressEvent(e);
}

void ListWidget::resizeEvent(QResizeEvent *e)
{
    m_scrollBar->setGeometry(width() - m_scrollBar->sizeHint().width(), 0,
                             m_scrollBar->sizeHint().width(), height());
    
    m_rows = (e->size().height() - 10) / m_metrics->height ();

    //m_scroll = true;
    recenterCurrent();
    updateList();
    QWidget::resizeEvent(e);
}

void ListWidget::wheelEvent (QWheelEvent *e)
{
    if (m_model->count() <= m_rows)
        return;
    if ((m_first == 0 && e->delta() > 0) ||
            ((m_first == m_model->count() - m_rows) && e->delta() < 0))
        return;
    m_first -= e->delta()/40;  //40*3 TODO: add step to config
    if (m_first < 0)
        m_first = 0;

    if (m_first > m_model->count() - m_rows)
        m_first = m_model->count() - m_rows;

    m_scroll = false;
    updateList();
}

bool ListWidget::event (QEvent *e)
{
    /*if(m_popupWidget)
    {
        if(e->type() == QEvent::ToolTip)
        {
            QHelpEvent *helpEvent = (QHelpEvent *) e;
            int row = rowAt(helpEvent->y());
            if(row < 0)
            {
                m_popupWidget->deactivate();
                return QWidget::event(e);
            }
            e->accept();
            m_popupWidget->prepare(m_model->item(row), helpEvent->globalPos());
            return true;
        }
        else if(e->type() == QEvent::Leave)
            m_popupWidget->deactivate();
    }*/
    return QWidget::event(e);
}

void ListWidget::updateList()
{
    if (m_model->count() < (m_rows+m_first+1) && m_rows< m_model->count())
    {
        m_first = m_model->count() - m_rows;
    }
    if (m_model->count() < m_rows + 1)
    {
        m_first = 0;
        m_scrollBar->setMaximum(0);
        m_scrollBar->setValue(0);
        emit positionChanged(0,0);
    }
    else
    {
        m_scrollBar->setMaximum(m_model->count() - m_rows);
        m_scrollBar->setValue(m_first);
        emit positionChanged(m_first, m_model->count() - m_rows);
    }
    if (m_model->count() <= m_first)
    {
        m_first = 0;
        m_scrollBar->setMaximum(qMax(0, m_model->count() - m_rows));
        m_scrollBar->setValue(0);
        emit positionChanged(0, qMax(0, m_model->count() - m_rows));
    }

    m_titles = m_model->getTitles(m_first, m_rows);
    m_times  = m_model->getTimes(m_first, m_rows);
    m_scroll = false;
    //add numbers
    for (int i = 0; i < m_titles.size() && m_show_number; ++i)
    {
        QString title = m_titles.at(i);
        m_titles.replace(i, title.prepend(QString("%1").arg(m_first+i+1)+". "));

    }
    //elide title
    QString extra_string;
    int x = width() - m_scrollBar->width();
    for (int i=0; i<m_titles.size(); ++i)
    {
        extra_string = getExtraString(m_first + i);
        int extra_string_space = extra_string.isEmpty() ? 0 : m_metrics->width(extra_string);
        m_titles.replace(i, m_metrics->elidedText (m_titles.at(i), Qt::ElideRight,
                            x -  m_metrics->width(m_times.at(i)) - 22 - extra_string_space));
    }
    update();
}

void ListWidget::autoscroll()
{
    SimpleSelection sel = m_model->getSelection(m_pressed_row);
    if ((sel.m_top == 0 && m_scroll_direction == TOP && sel.count() > 1) ||
        (sel.m_bottom == m_model->count() - 1 && m_scroll_direction == DOWN && sel.count() > 1))
        return;

    if(m_scroll_direction == DOWN)
    {
        int row = m_first + m_rows;
        (m_first + m_rows < m_model->count()) ? m_first ++ : m_first;
        m_model->moveItems(m_pressed_row,row);
        m_pressed_row = row;
    }
    else if(m_scroll_direction == TOP && m_first > 0)
    {
        m_first --;
        m_model->moveItems(m_pressed_row, m_first);
        m_pressed_row = m_first;
    }
}

void ListWidget::scroll(int sc)
{
    if (m_model->count() <= m_rows)
        return;
    m_first = sc;
    m_scroll = true;
    updateList();
}

void ListWidget::updateSkin()
{
    //loadColors();
    update();
}

void ListWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("text/uri-list"))
        event->acceptProposedAction();
}


void ListWidget::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasUrls())
    {
        QList<QUrl> list_urls = event->mimeData()->urls();
        event->acceptProposedAction();
        QApplication::restoreOverrideCursor();

        foreach(QUrl u,list_urls)
        {
            QString add_string = u.toString(QUrl::RemoveScheme);
            if (!add_string.isEmpty())
                processFileInfo(QFileInfo(add_string));
        }
    }
}

void ListWidget::processFileInfo(const QFileInfo& info)
{
    if (info.isDir())
    {
        m_model->add(info.absoluteFilePath());
    }
    else
    {
        m_model->add(info.absoluteFilePath());
        m_model->loadPlaylist(info.absoluteFilePath());
    }
}

const QString ListWidget::getExtraString(int i)
{
    QString extra_string;

    if (m_show_protocol && m_model->item(i)->url().contains("://"))
        extra_string = "[" + m_model->item(i)->url().split("://").at(0) + "]";

    if (m_player->isRepeatable() && (m_model->currentRow() == i))
        extra_string += " |R|";

    if (m_model->isQueued(m_model->item(i)))
    {
        int index = m_model->queuedIndex(m_model->item(i));
        extra_string += " |"+QString::number(index + 1)+"|";
    }
    extra_string = extra_string.trimmed(); //remove white space
    return extra_string;
}

void ListWidget::mouseMoveEvent(QMouseEvent *e)
{
    if(e->buttons() == Qt::LeftButton)
    {
        m_scroll = true;
        if (m_prev_y > e->y())
            m_scroll_direction = TOP;
        else if (m_prev_y < e->y())
            m_scroll_direction = DOWN;
        else
            m_scroll_direction = NONE;

        if(e->y() < 0 || e->y() > height())
        {
            if(!m_timer->isActive())
                m_timer->start();
            return;
        }
        m_timer->stop();

        int row = rowAt(e->y());

        if (INVALID_ROW != row)
        {
            SimpleSelection sel = m_model->getSelection(m_pressed_row);
            if(sel.count() > 1 && m_scroll_direction == TOP)
            {
                if(sel.m_top == 0 || sel.m_top == m_first)
                    return;
            }
            else if(sel.count() > 1 && m_scroll_direction == DOWN)
            {
                if(sel.m_bottom == m_model->count() - 1 || sel.m_bottom == m_first + m_rows)
                    return;
            }
            m_model->moveItems(m_pressed_row,row);

            m_prev_y = e->y();
            m_scroll = false;
            m_pressed_row = row;
        }
    }
    /*else if(m_popupWidget)
    {
        int row = rowAt(e->y());
        if(row < 0 || m_popupWidget->item() != m_model->item(row))
            m_popupWidget->deactivate();
    }*/
}

void ListWidget::mouseReleaseEvent(QMouseEvent *e)
{
    if (false != m_select_on_release)
    {
        m_model->clearSelection();
        m_model->setSelected(m_pressed_row,true);
        //if(e->modifiers() != Qt::ShiftModifier)
        m_anchor_row = m_pressed_row;
        m_select_on_release = false;
    }
    m_pressed_row = INVALID_ROW;
    m_scroll_direction = NONE;
    m_timer->stop();
    m_scroll = false;
    QWidget::mouseReleaseEvent(e);
}

int ListWidget::rowAt(int y) const
{
    if (y <= 14 && y >= 2 && m_model->count())
        return m_first;

    for (int i = 0; i < qMin(m_rows, m_model->count() - m_first) - 1; ++i)
    {
        if ((y >= 14 + i * m_metrics->height ()) && (y <= 14 + (i+1) * m_metrics->height()))
            return m_first + i + 1;
    }
    return INVALID_ROW;
}

void ListWidget::contextMenuEvent(QContextMenuEvent * event)
{
    if (menu())
        menu()->exec(event->globalPos());
}

void ListWidget::recenterCurrent()
{
    //qDebug("%d", m_rows);
    if (!m_scroll)
    {
        if (m_first + m_rows < m_model->currentRow() + 1)
            m_first = qMin(m_model->count() - m_rows,
                           m_model->currentRow() - m_rows/2 + 1);
        else if (m_first > m_model->currentRow())
            m_first = qMax (m_model->currentRow() - m_rows/2 + 1, 0);
    }
}
