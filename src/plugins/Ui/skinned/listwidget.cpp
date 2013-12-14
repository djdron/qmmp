/***************************************************************************
 *   Copyright (C) 2006-2013 by Ilya Kotov                                 *
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
#include <qmmpui/playlistitem.h>
#include <qmmpui/playlistmodel.h>
#include <qmmpui/qmmpuisettings.h>
#include "listwidget.h"
#include "skin.h"
#include "popupwidget.h"
#include "playlist.h"

#define INVALID_INDEX -1

ListWidget::ListWidget(QWidget *parent)
        : QWidget(parent)
{
    m_update = false;
    m_skin = Skin::instance();
    m_popupWidget = 0;
    m_metrics = 0;
    m_extra_metrics = 0;
    m_drop_index = INVALID_INDEX;
    loadColors();
    m_menu = new QMenu(this);
    m_scroll_direction = NONE;
    m_prev_y = 0;
    m_anchor_index = INVALID_INDEX;
    m_pressed_index = INVALID_INDEX;
    m_ui_settings = QmmpUiSettings::instance();
    connect (m_ui_settings, SIGNAL(repeatableTrackChanged(bool)), SLOT(updateList()));
    m_first = 0;
    m_row_count = 0;
    m_scroll = false;
    m_select_on_release = false;
    readSettings();
    connect(m_skin, SIGNAL(skinChanged()), this, SLOT(updateSkin()));
    setAcceptDrops(true);
    setMouseTracking(true);
    m_timer = new QTimer(this);
    m_timer->setInterval(50);
    connect(m_timer, SIGNAL(timeout()), SLOT(autoscroll()));
}

ListWidget::~ListWidget()
{
    if(m_metrics)
        delete m_metrics;
    if(m_extra_metrics)
        delete m_extra_metrics;
    qDeleteAll(m_rows);
    m_rows.clear();
}

void ListWidget::readSettings()
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("Skinned");
    m_font.fromString(settings.value("pl_font", QApplication::font().toString()).toString());
    m_extra_font = m_font;
    m_extra_font.setPointSize(m_font.pointSize() - 1);
    m_show_protocol = settings.value ("pl_show_protocol", false).toBool();
    m_show_number = settings.value ("pl_show_numbers", true).toBool();
    m_align_numbres = settings.value ("pl_align_numbers", false).toBool();
    m_show_anchor = settings.value("pl_show_anchor", false).toBool();
    bool show_popup = settings.value("pl_show_popup", false).toBool();

    if (m_update)
    {
        delete m_metrics;
        delete m_extra_metrics;
        m_metrics = new QFontMetrics(m_font);
        m_extra_metrics = new QFontMetrics(m_extra_font);
        m_row_count = height() / (m_metrics->lineSpacing() + 2);
        updateList();
        if(m_popupWidget)
        {
            m_popupWidget->deleteLater();
            m_popupWidget = 0;
        }
    }
    else
    {
        m_update = true;
        m_metrics = new QFontMetrics(m_font);
        m_extra_metrics = new QFontMetrics(m_extra_font);
    }
    if(show_popup)
        m_popupWidget = new PlayListPopup::PopupWidget(this);
}

int ListWidget::visibleRows() const
{
    return m_row_count;
}

int ListWidget::firstVisibleIndex() const
{
    return m_first;
}

int ListWidget::anchorIndex() const
{
    return m_anchor_index;
}

void ListWidget::setAnchorIndex(int index)
{
    m_anchor_index = index;
    update();
}

QMenu *ListWidget::menu()
{
    return m_menu;
}

PlayListModel *ListWidget::model()
{
    Q_ASSERT(m_model);
    return m_model;
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
    QPainter painter(this);
    painter.setFont(m_font);
    painter.setBrush(QBrush(m_normal_bg));
    painter.drawRect(-1,-1,width()+1,height()+1);
#if QT_VERSION >= 0x040700
    painter.setLayoutDirection(Qt::LayoutDirectionAuto);
#endif
    bool rtl = (layoutDirection() == Qt::RightToLeft);
    int sx = 0, sy = 0;

    for (int i = 0; i < m_rows.size(); ++i )
    {
        sy = (i + 1) * (2 + m_metrics->lineSpacing()) - 2 - m_metrics->descent();

        if (m_show_anchor && i == m_anchor_index - m_first)
        {
            painter.setBrush(m_rows[i]->selected ? m_selected_bg : m_normal_bg);
            painter.setPen(m_normal);
            painter.drawRect (6, i * (m_metrics->lineSpacing() + 2),
                              width() - 10, m_metrics->lineSpacing() + 1);
        }
        else
        {
            if (m_rows[i]->selected)
            {
                painter.setBrush(QBrush(m_selected_bg));
                painter.setPen(m_selected_bg);
                painter.drawRect (6, i * (m_metrics->lineSpacing() + 2),
                                  width() - 10, m_metrics->lineSpacing() + 1);
            }
        }

        if(m_rows[i]->separator)
        {
            painter.setPen(m_normal);
            sx = 45;

            if(m_number_width)
                sx += m_number_width + m_metrics->width("9");
            if(rtl)
                sx = width() - sx - m_metrics->width(m_rows[i]->title) - 5;

            painter.drawText(sx, sy, m_rows[i]->title);

            sy -= (m_metrics->lineSpacing()/2 - 2);

            if(rtl)
            {
                painter.drawLine(10, sy, sx - 5, sy);
                painter.drawLine(sx + m_metrics->width(m_rows[i]->title) + 5, sy,
                                 sx + m_metrics->width(m_rows[i]->title) + 35, sy);
            }
            else
            {
                painter.drawLine(sx - 35, sy, sx - 5, sy);
                painter.drawLine(sx + m_metrics->width(m_rows[i]->title) + 5, sy,
                                 width() - 10, sy);
            }
            continue;
        }

        if (m_model->currentIndex() == m_first + i)
            painter.setPen(m_current);
        else
            painter.setPen(m_normal);  //243,58


        if(m_number_width)
        {
            QString number = QString("%1").arg(m_rows[i]->number);
            sx = 10 + m_number_width - m_metrics->width(number);
            if(rtl)
                sx = width() - sx - m_metrics->width(number);

            painter.drawText(sx, sy, number);

            sx = 10 + m_number_width + m_metrics->width("9");
            if(rtl)
                sx = width() - sx - m_metrics->width(m_rows[i]->title);
        }
        else
        {
            sx = rtl ? width() - 10 - m_metrics->width(m_rows[i]->title) : 10;
        }

        painter.drawText(sx, sy, m_rows[i]->title);

        QString extra_string = m_rows[i]->extraString;

        if(!extra_string.isEmpty())
        {
            painter.setFont(m_extra_font);

            if(m_rows[i]->length.isEmpty())
            {
                sx = rtl ? 7 : width() - 7 - m_extra_metrics->width(extra_string);
                painter.drawText(sx, sy, extra_string);
            }
            else
            {
                sx = width() - 10 - m_extra_metrics->width(extra_string) - m_metrics->width(m_rows[i]->length);
                if(rtl)
                    sx = width() - sx - m_extra_metrics->width(extra_string);
                painter.drawText(sx, sy, extra_string);
            }
            painter.setFont(m_font);
        }
        sx = rtl ? 9 : width() - 7 - m_metrics->width(m_rows[i]->length);
        painter.drawText(sx, sy, m_rows[i]->length);
    }
    //draw drop line
    if(m_drop_index != INVALID_INDEX)
    {
        painter.setPen(m_current);
        painter.drawLine (6, (m_drop_index - m_first) * (m_metrics->lineSpacing() + 2),
                          width() - 4 , (m_drop_index - m_first) * (m_metrics->lineSpacing() + 2));
    }
    //draw line
    if(m_number_width)
    {
        painter.setPen(m_normal);
        sx = rtl ? width() - 10 - m_number_width - m_metrics->width("9")/2 - 1 :
                   10 + m_number_width + m_metrics->width("9")/2 - 1;
        painter.drawLine(sx, 2, sx, sy);
    }
}

void ListWidget::mouseDoubleClickEvent (QMouseEvent *e)
{
    int y = e->y();
    int index = indexAt(y);
    if (INVALID_INDEX != index)
    {
        m_model->setCurrent(index);
        emit selectionChanged();
        update();
    }
}

void ListWidget::mousePressEvent(QMouseEvent *e)
{
    if(m_popupWidget)
        m_popupWidget->hide();
    m_scroll = true;
    int y = e->y();
    int index = indexAt(y);

    if (INVALID_INDEX != index && m_model->count() > index)
    {
        m_pressed_index = index;
        if(e->button() == Qt::RightButton)
        {
            if(!m_model->isSelected(index))
            {
                m_model->clearSelection();
                m_model->setSelected(index, true);
            }
            m_anchor_index = m_pressed_index;
            if(m_model->isGroup(index) && m_model->selectedTracks().isEmpty())
            {
                PlayListGroup *group = m_model->group(index);
                m_model->setSelected(group->tracks());
            }
            QWidget::mousePressEvent(e);
            return;
        }

        if (m_model->isSelected(index) && (e->modifiers() == Qt::NoModifier))
        {
            m_select_on_release = true;
            QWidget::mousePressEvent(e);
            return;
        }

        if ((Qt::ShiftModifier & e->modifiers()))
        {
            bool select = true;
            if (m_pressed_index > m_anchor_index)
            {
                for (int j = m_anchor_index;j <= m_pressed_index;j++)
                {
                    m_model->setSelected(j, select);
                }
            }
            else
            {
                for (int j = m_anchor_index;j >= m_pressed_index;j--)
                {
                    m_model->setSelected(j, select);
                }
            }
            m_anchor_index = m_pressed_index;
        }
        else //ShiftModifier released
        {
            if ((Qt::ControlModifier & e->modifiers()))
            {
                m_model->setSelected(index, !m_model->isSelected(index));
            }
            else //ControlModifier released
            {
                m_model->clearSelection();
                m_model->setSelected(index, true);
            }
            m_anchor_index = m_pressed_index;
        }
        update();
    }
    QWidget::mousePressEvent(e);
}

void ListWidget::resizeEvent(QResizeEvent *e)
{
    m_row_count = e->size().height() / (m_metrics->lineSpacing() + 2);
    m_scroll = true;
    updateList();
    QWidget::resizeEvent(e);
}

void ListWidget::wheelEvent (QWheelEvent *e)
{
    if (m_model->count() <= m_row_count)
        return;
    if ((m_first == 0 && e->delta() > 0) ||
            ((m_first == m_model->count() - m_row_count) && e->delta() < 0))
        return;
    m_first -= e->delta()/40;  //40*3 TODO: add step to config
    if (m_first < 0)
        m_first = 0;

    if (m_first > m_model->count() - m_row_count)
        m_first = m_model->count() - m_row_count;

    m_scroll = false;
    updateList();
}

bool ListWidget::event (QEvent *e)
{
    if(m_popupWidget)
    {
        if(e->type() == QEvent::ToolTip)
        {
            QHelpEvent *helpEvent = (QHelpEvent *) e;
            int index = indexAt(helpEvent->y());
            if(index < 0 || !m_model->isTrack(index))
            {
                m_popupWidget->deactivate();
                return QWidget::event(e);
            }
            e->accept();
            m_popupWidget->prepare(m_model->track(index), helpEvent->globalPos());
            return true;
        }
        else if(e->type() == QEvent::Leave)
            m_popupWidget->deactivate();
    }
    return QWidget::event(e);
}

void ListWidget::updateList()
{
    if (m_model->count() < (m_row_count+m_first+1) && m_row_count< m_model->count())
    {
        m_first = m_model->count() - m_row_count;
    }
    if (m_model->count() < m_row_count + 1)
    {
        m_first = 0;
        emit positionChanged(0,0);
    }
    else
        emit positionChanged(m_first, m_model->count() - m_row_count);
    if (m_model->count() <= m_first)
    {
        m_first = 0;
        emit positionChanged(0, qMax(0, m_model->count() - m_row_count));
    }
    //song numbers width
    if(m_show_number && m_align_numbres && m_model->count())
    {
        m_number_width = m_metrics->width("9") * QString::number(m_model->trackCount()).size();
    }
    else
        m_number_width = 0;

    QList<PlayListItem *> items = m_model->mid(m_first, m_row_count);

    while(m_rows.count() < qMin(m_row_count, items.count()))
        m_rows << new ListWidgetRow;
    while(m_rows.count() > qMin(m_row_count, items.count()))
        delete m_rows.takeFirst();

    for(int i = 0; i < items.count(); ++i)
    {
        ListWidgetRow *row = m_rows[i];
        row->title = items[i]->formattedTitle();
        row->selected = items[i]->isSelected();
        if(items[i]->isGroup())
        {
            row->separator = true;
            row->number = 0;
            row->length.clear();
            row->title = m_metrics->elidedText (row->title, Qt::ElideRight,
                            width() - m_number_width - 22 - 70);
        }
        else
        {
            row->separator = false;
            row->number = m_model->numberOfTrack(m_first+i) + 1;
            row->length = items[i]->formattedLength();
            if(m_show_number && !m_align_numbres)
                row->title.prepend(QString("%1").arg(row->number)+". ");
            row->extraString = getExtraString(m_first + i);
            //elide titles
            int extra_string_width = row->extraString.isEmpty() ? 0 : m_metrics->width(row->extraString);
            if(m_number_width)
                extra_string_width += m_number_width + m_metrics->width("9");
            row->title = m_metrics->elidedText (row->title, Qt::ElideRight,
                            width() -  m_metrics->width(row->length) - 22 - extra_string_width);
        }
    }
    m_scroll = false;
    update();
}

void ListWidget::autoscroll()
{
    SimpleSelection sel = m_model->getSelection(m_pressed_index);
    if ((sel.m_top == 0 && m_scroll_direction == TOP && sel.count() > 1) ||
        (sel.m_bottom == m_model->count() - 1 && m_scroll_direction == DOWN && sel.count() > 1))
        return;

    if(m_scroll_direction == DOWN)
    {
        int row = m_first + m_row_count;
        (m_first + m_row_count < m_model->count()) ? m_first ++ : m_first;
        m_model->moveItems(m_pressed_index,row);
        m_pressed_index = row;
    }
    else if(m_scroll_direction == TOP && m_first > 0)
    {
        m_first --;
        m_model->moveItems(m_pressed_index, m_first);
        m_pressed_index = m_first;
    }
}

void ListWidget::setModel(PlayListModel *selected, PlayListModel *previous)
{
    if(previous)
        disconnect(previous, 0, this, 0); //disconnect previous model
    qApp->processEvents();
    m_model = selected;
    m_first = 0;
    m_scroll = false;
    recenterCurrent();
    updateList();
    connect (m_model, SIGNAL(currentChanged()), SLOT(recenterCurrent()));
    connect (m_model, SIGNAL(listChanged()), SLOT(updateList()));
}

void ListWidget::scroll(int sc)
{
    if (m_model->count() <= m_row_count)
        return;
    m_first = sc; //*(m_model->count() - m_rows)/99;
    m_scroll = true;
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

        int index = indexAt(event->pos().y());
        if(index == INVALID_INDEX)
        {
            index = qMin(m_first + m_row_count, m_model->count());
        }
        m_model->insert(index, list_urls);
    }
    m_drop_index = INVALID_INDEX;
}

void ListWidget::dragLeaveEvent(QDragLeaveEvent *)
{
    m_drop_index = INVALID_INDEX;
    update();
}

void ListWidget::dragMoveEvent(QDragMoveEvent *event)
{
    int index = indexAt(event->pos().y());
    if(index == INVALID_INDEX)
        index = qMin(m_first + m_row_count, m_model->count());
    if(index != m_drop_index)
    {
        m_drop_index = index;
        update();
    }
}

const QString ListWidget::getExtraString(int i)
{
    QString extra_string;

    PlayListTrack *track = m_model->track(i);
    if(!track)
        return extra_string;

    if (m_show_protocol && track->url().contains("://"))
        extra_string = "[" + track->url().split("://").at(0) + "]";

    if (m_model->isQueued(track))
    {
        int index = m_model->queuedIndex(track);
        extra_string += "|"+QString::number(index + 1)+"|";
    }

    if(m_model->currentIndex() == i && m_ui_settings->isRepeatableTrack())
        extra_string += "|R|";
    else if(m_model->isStopAfter(track))
        extra_string += "|S|";

    extra_string = extra_string.trimmed(); //remove white space
    if(!extra_string.isEmpty())
        extra_string.prepend(" ");
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

        int index = indexAt(e->y());

        if (INVALID_INDEX != index)
        {
            SimpleSelection sel = m_model->getSelection(m_pressed_index);
            if(sel.count() > 1 && m_scroll_direction == TOP)
            {
                if(sel.m_top == 0 || sel.m_top == m_first)
                    return;
            }
            else if(sel.count() > 1 && m_scroll_direction == DOWN)
            {
                if(sel.m_bottom == m_model->count() - 1 || sel.m_bottom == m_first + m_row_count)
                    return;
            }
            m_model->moveItems(m_pressed_index,index);

            m_prev_y = e->y();
            m_scroll = false;
            m_pressed_index = index;
            m_anchor_index = index;
        }
    }
    else if(m_popupWidget)
    {
        int index = indexAt(e->y());
        if(index < 0 || !m_model->isTrack(index) || m_popupWidget->url() != m_model->track(index)->url())
            m_popupWidget->deactivate();
    }
}

void ListWidget::mouseReleaseEvent(QMouseEvent *e)
{
    if (m_select_on_release)
    {
        m_model->clearSelection();
        m_model->setSelected(m_pressed_index,true);
        m_anchor_index = m_pressed_index;
        m_select_on_release = false;
    }
    m_pressed_index = INVALID_INDEX;
    m_scroll_direction = NONE;
    m_timer->stop();
    m_scroll = false;
    QWidget::mouseReleaseEvent(e);
}

int ListWidget::indexAt(int y) const
{
    for (int i = 0; i < qMin(m_row_count, m_model->count() - m_first); ++i)
    {
        if ((y >= i * (m_metrics->lineSpacing() + 2)) && (y <= (i+1) * (m_metrics->lineSpacing() + 2)))
            return m_first + i;
    }
    return INVALID_INDEX;
}

void ListWidget::contextMenuEvent(QContextMenuEvent * event)
{
    if (menu())
        menu()->exec(event->globalPos());
}

void ListWidget::recenterCurrent()
{
    if (!m_scroll && m_row_count)
    {
        if (m_first + m_row_count < m_model->currentIndex() + 1)
            m_first = qMin(m_model->count() - m_row_count,
                           m_model->currentIndex() - m_row_count/2);
        else if (m_first > m_model->currentIndex())
            m_first = qMax (m_model->currentIndex() - m_row_count/2, 0);
    }
}
