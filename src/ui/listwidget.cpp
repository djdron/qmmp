/***************************************************************************
 *   Copyright (C) 2006-2009 by Ilya Kotov                                 *
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
#include <qmmpui/playlistitem.h>
#include <qmmpui/playlistmodel.h>
#include <qmmpui/mediaplayer.h>
#include "textscroller.h"
#include "listwidget.h"
#include "skin.h"
#include "playlist.h"

#define INVALID_ROW -1

ListWidget::ListWidget(QWidget *parent)
        : QWidget(parent)
{
    m_update = FALSE;
    m_skin = Skin::instance();
    loadColors();
    m_menu = new QMenu(this);
    m_scroll_direction = NONE;
    m_prev_y = 0;
    m_anchor_row = INVALID_ROW;
    m_player = MediaPlayer::instance();
    connect (m_player, SIGNAL(repeatableChanged(bool)), SLOT(updateList()));
    m_first = 0;
    m_rows = 0;
    m_scroll = FALSE;
    m_select_on_release = FALSE;
    readSettings();
    connect(m_skin, SIGNAL(skinChanged()), this, SLOT(updateSkin()));
    setAcceptDrops(true);
}


ListWidget::~ListWidget()
{}

void ListWidget::readSettings()
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    m_font.fromString(settings.value("PlayList/Font", QApplication::font().toString()).toString());
    m_show_protocol = settings.value ("PlayList/show_protocol", FALSE).toBool();
    m_show_number = settings.value ("PlayList/show_numbers", TRUE).toBool();

    if (m_update)
    {
        delete m_metrics;
        m_metrics = new QFontMetrics(m_font);
        m_rows = (height() - 10) / m_metrics->ascent ();
        updateList();
    }
    else
    {
        m_update = TRUE;
        m_metrics = new QFontMetrics(m_font);
    }
}

void ListWidget::loadColors()
{
    m_normal.setNamedColor(m_skin->getPLValue("normal"));
    m_current.setNamedColor(m_skin->getPLValue("current"));
    m_normal_bg.setNamedColor(m_skin->getPLValue("normalbg"));
    m_selected_bg.setNamedColor(m_skin->getPLValue("selectedbg"));
}

void ListWidget::paintEvent(QPaintEvent *)
{

    QPainter m_painter(this);
    //m_painter.setPen(Qt::white);
    m_painter.setFont(m_font);
    m_painter.setBrush(QBrush(m_normal_bg));
    m_painter.drawRect(-1,-1,width()+1,height()+1);

    for (int i=0; i<m_titles.size(); ++i )
    {
        if (m_model->isSelected(i + m_first))
        {
            m_painter.setBrush(QBrush(m_selected_bg));
            m_painter.setPen(m_selected_bg);
            m_painter.drawRect ( 6, 15+(i-1)*m_metrics->ascent(),
                                 width() - 10, m_metrics->ascent());
        }

        if (m_model->currentRow() == i + m_first)
            m_painter.setPen(m_current);
        else
            m_painter.setPen(m_normal);  //243,58

        m_painter.drawText(10,14+i*m_metrics->ascent(),m_titles.at(i));

        if (m_model->isQueued(m_model->item(i + m_first)) ||
                (m_player->isRepeatable() && (m_model->currentRow() == m_first + i)) || m_show_protocol)
        {
            QString extra_string = getExtraString(m_first + i);

            int old_size = m_font.pointSize();
            m_font.setPointSize(old_size - 1 );
            m_painter.setFont(m_font);

            m_painter.drawText(width() - 10 - m_metrics->width(extra_string) - m_metrics->width(m_times.at(i)),
                               14+i*m_metrics->ascent (), extra_string);
            m_font.setPointSize(old_size);
            m_painter.setFont(m_font);
            m_painter.setBrush(QBrush(m_normal_bg));
        }

        m_painter.drawText(width() - 7 - m_metrics->width(m_times.at(i)),
                           14+i*m_metrics->ascent (), m_times.at(i));
    }

}

void ListWidget::mouseDoubleClickEvent (QMouseEvent *e)
{
    int y = e->y();
    int row = rowAt(y);
    if (INVALID_ROW != row)
    {
        m_model->setCurrent(row);
        emit selectionChanged();
        update();
    }
}


void ListWidget::mousePressEvent(QMouseEvent *e)
{
    m_scroll = TRUE;
    int y = e->y();
    int row = rowAt(y);

    if (INVALID_ROW != row && m_model->count() > row)
    {
        if (!(Qt::ControlModifier & e->modifiers () ||
                Qt::ShiftModifier & e->modifiers () ||
                m_model->isSelected(row)))
            m_model->clearSelection();

        if (m_model->isSelected(row) && (e->modifiers() == Qt::NoModifier))
            m_select_on_release = TRUE;

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
    m_rows = (e->size().height() - 10) / m_metrics->ascent ();

    m_scroll = TRUE;

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

    m_scroll = FALSE;
    updateList();
}

bool ListWidget::event (QEvent *e)
{
    if(e->type() == QEvent::ToolTip)
    {
        e->accept();
        QHelpEvent *helpEvent = (QHelpEvent *) e;
        int row = rowAt(helpEvent->y());
        qDebug("==== %d", row);
        return TRUE;
    }
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
        emit positionChanged(0,0);
    }
    else
    {
        //int pos = m_first*99/(m_model->count() - m_rows);
        //emit positionChanged(pos);
        emit positionChanged(m_first, m_model->count() - m_rows);
    }
    if (m_model->count() <= m_first)
    {
        m_first = 0;
        emit positionChanged(0, qMax(0, m_model->count() - m_rows));
    }

    m_titles = m_model->getTitles(m_first, m_rows);
    m_times  = m_model->getTimes(m_first, m_rows);
    m_scroll = FALSE;
    //add numbers
    for (int i = 0; i < m_titles.size() && m_show_number; ++i)
    {
        QString title = m_titles.at(i);
        m_titles.replace(i, title.prepend(QString("%1").arg(m_first+i+1)+". "));

    }
    //elide title
    QString extra_string;
    for (int i=0; i<m_titles.size(); ++i)
    {
        extra_string = getExtraString(m_first + i);
        int extra_string_space = extra_string.isEmpty() ? 0 : m_metrics->width(extra_string);
        m_titles.replace(i, m_metrics->elidedText (m_titles.at(i), Qt::ElideRight,
                            width() -  m_metrics->width(m_times.at(i)) - 22 - extra_string_space));
    }
    update();
}

void ListWidget::setModel(PlayListModel *selected, PlayListModel *previous)
{
    if(previous)
        disconnect(previous, 0, this, 0); //disconnect previous model
    m_model = selected;
    connect (m_model, SIGNAL(listChanged()), SLOT(updateList()));
    connect (m_model, SIGNAL(currentChanged()), SLOT(recenterCurrent()));
    updateList();
}

void ListWidget::scroll(int sc)
{
    if (m_model->count() <= m_rows)
        return;
    m_first = sc; //*(m_model->count() - m_rows)/99;
    m_scroll = TRUE;
    updateList();
}

void ListWidget::updateSkin()
{
    loadColors();
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
        m_model->addDirectory(info.absoluteFilePath());
    }
    else
    {
        m_model->addFile(info.absoluteFilePath());
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
    m_scroll = true;
    if (m_prev_y > e->y())
        m_scroll_direction = TOP;
    else if (m_prev_y < e->y())
        m_scroll_direction = DOWN;
    else
        m_scroll_direction = NONE;

    int row = rowAt(e->y());

    if (INVALID_ROW != row)
    {
        SimpleSelection sel = m_model->getSelection(m_pressed_row);
        if (((sel.m_top == 0 && m_scroll_direction == TOP) && sel.count() > 1) ||
                (sel.m_bottom == m_model->count() - 1 && m_scroll_direction == DOWN && sel.count() > 1)
           )
            return;

        if (row + 1 == m_first + m_rows && m_scroll_direction == DOWN)
            (m_first + m_rows < m_model->count() ) ? m_first ++ : m_first;
        else if (row == m_first && m_scroll_direction == TOP)
            (m_first > 0)  ? m_first -- : 0;

        m_model->moveItems(m_pressed_row,row);
        m_prev_y = e->y();
        m_scroll = false;
        m_pressed_row = row;
    }
}

void ListWidget::mouseReleaseEvent(QMouseEvent *e)
{
    if (FALSE != m_select_on_release)
    {
        m_model->clearSelection();
        m_model->setSelected(m_pressed_row,true);
        //if(e->modifiers() != Qt::ShiftModifier)
        m_anchor_row = m_pressed_row;
        m_select_on_release = FALSE;
    }
    m_pressed_row = INVALID_ROW;
    m_scroll_direction = NONE;
    QWidget::mouseReleaseEvent(e);
}

int ListWidget::rowAt(int y) const
{
    if (y <= 14 && y >= 2 && m_model->count())
        return m_first;

    for (int i = 0; i < qMin(m_rows, m_model->count() - m_first); ++i )
    {
        if ((y >= 14 + i * m_metrics->ascent ()) && (y <= 14 + (i+1) * m_metrics->ascent()))
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
    if (!m_scroll)
    {
        if (m_first + m_rows < m_model->currentRow() + 1)
            m_first = qMin(m_model->count() - m_rows,
                           m_model->currentRow() - m_rows/2 + 1);
        else if (m_first > m_model->currentRow())
            m_first = qMax (m_model->currentRow() - m_rows/2 + 1, 0);
    }
}
