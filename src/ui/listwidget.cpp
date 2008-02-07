/***************************************************************************
 *   Copyright (C) 2006 by Ilya Kotov                                      *
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

#include "mediafile.h"
#include "textscroller.h"
#include "listwidget.h"
#include "skin.h"
#include "playlistmodel.h"
#include "playlist.h"

#define INVALID_ROW -1

ListWidget::ListWidget(QWidget *parent)
        : QWidget(parent)
{
    m_update = FALSE;
    m_skin = Skin::getPointer();
    loadColors();
    setWindowFlags(Qt::FramelessWindowHint);
    m_menu = new QMenu(this);
    m_scroll_direction = NONE;
    m_prev_y = 0;
    m_anchor_row = INVALID_ROW;

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
    QSettings settings(QDir::homePath()+"/.qmmp/qmmprc", QSettings::IniFormat);
    QString fontname = settings.value("PlayList/Font","").toString();
    if (fontname.isEmpty ())
        fontname = QFont("Helvetica [Cronyx]", 10).toString();
    m_font.fromString(fontname);

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

        if (m_model->isQueued(m_model->item(i + m_first)))
        {
            QString queue_string = "|" +
                                   QString::number(1 + m_model->queuedIndex(m_model->item(m_first + i))) + "|";

            int old_size = m_font.pointSize();
            m_font.setPointSize(old_size - 1 );
            m_painter.setFont(m_font);

            m_painter.drawText(width() - 10 - m_metrics->width(queue_string) - m_metrics->width(m_times.at(i)), 12+i*m_metrics->ascent (), queue_string);

            m_font.setPointSize(old_size);
            m_painter.setFont(m_font);


            m_painter.setBrush(QBrush(Qt::transparent));
            //m_painter.drawRect(width() - 10 - m_metrics->width(queue_string) - m_metrics->width(m_times.at(i)),
            //              /*14+*/i*m_metrics->ascent () + 3,10,12);
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
                //int upper_selected = m_model->firstSelectedUpper(m_anchor_row);
                //if (INVALID_ROW != upper_selected)
                //{
                /*for (int j = upper_selected;j < m_anchor_row;j++)
                {
                 m_model->setSelected(j, false);
                }*/
                m_model->clearSelection();
                for (int j = m_anchor_row;j <= m_pressed_row;j++)
                {
                    m_model->setSelected(j, true);
                }
                //}
            }
            else
            {
                m_model->clearSelection();
                for (int j = m_anchor_row;j >= m_pressed_row;j--)
                {
                    m_model->setSelected(j, true);
                }
            }

            /*
                   int upper_selected = m_model->firstSelectedUpper(row);
                   int lower_selected = m_model->firstSelectedLower(row);
                   if (INVALID_ROW != upper_selected)
                   {
                       for (int j = upper_selected;j <= row;j++)
                       {
                           m_model->setSelected(j, true);
                       }
                   }
                   else if (INVALID_ROW != lower_selected)
                   {
                       for (int j = row;j <= lower_selected;j++)
                       {
                           m_model->setSelected(j, true);
                       }
                   }
                   else
                       m_model->setSelected(row, true);
            */
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

    m_titles = m_model->getTitles(m_first, m_rows );
    m_times  = m_model->getTimes(m_first, m_rows );
    m_scroll = FALSE;
    //add numbers
    for (int i = 0; i < m_titles.size(); ++i)
    {
        QString title = m_titles.at(i);
        m_titles.replace(i, title.prepend(QString("%1").arg(m_first+i+1)+". "));

    }
    if (m_model->currentItem())
    {
        TextScroller::getPointer()->setText("***  "+m_model->currentItem()->title());
        parentWidget()->parentWidget()->setWindowTitle(m_model->currentItem()->title());
    }
    cut();
    update();
}

void ListWidget::setModel(PlayListModel *model)
{
    m_model = model;
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

void ListWidget::cut()
{
    bool cut;
    for (int i=0; i<m_titles.size(); ++i )
    {
        QString name;
        cut = FALSE;

        int queue_number_space = 0;
        if (m_model->isQueued(m_model->item(i + m_first)))
        {
            int index = m_model->queuedIndex(m_model->item(m_first + i));
            QString queue_string = "|"+QString::number(index)+"|";
            queue_number_space = m_metrics->width(queue_string);
        }
        while ( m_metrics->width(m_titles.at(i)) > (this->width() - 54 - queue_number_space))
        {
            cut = TRUE;
            name = m_titles.at(i);
            m_titles.replace(i, name.left(name.length()-1) );
        }
        if (cut)
        {
            m_titles.replace(i, name.left(name.length()-3).trimmed()+"...");

        }
    }
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
    }
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
        if ((sel.m_top == 0 && m_scroll_direction == TOP) && sel.count() > 1 ||
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

int ListWidget::rowAt( int y) const
{
    for (int i = 0; i < qMin(m_rows, m_model->count() - m_first); ++i )
    {
        if ((y >= 2+i*m_metrics->ascent())&&(y < 2+(i+1)*m_metrics->ascent()))
            return m_first + i;
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


