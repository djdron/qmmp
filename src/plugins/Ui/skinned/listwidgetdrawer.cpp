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

#include <QSettings>
#include <QPainter>
#include <QApplication>
#include <qmmp/qmmp.h>
#include <qmmpui/qmmpuisettings.h>
#include "skin.h"
#include "listwidgetdrawer.h"

#define PADDING 3

ListWidgetDrawer::ListWidgetDrawer()
{
    m_skin = Skin::instance();
    m_update = false;
    m_show_anchor = false;
    m_show_number = false;
    m_align_numbres = false;
    m_row_height = 0;
    m_number_width = 0;
    readSettings();
    loadColors();
}

ListWidgetDrawer::~ListWidgetDrawer()
{
    if(m_metrics)
        delete m_metrics;
    if(m_extra_metrics)
        delete m_extra_metrics;
}

void ListWidgetDrawer::readSettings()
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("Skinned");
    m_show_anchor = settings.value("pl_show_anchor", false).toBool();
    m_show_number = settings.value ("pl_show_numbers", true).toBool();
    m_align_numbres = settings.value ("pl_align_numbers", false).toBool();
    m_font.fromString(settings.value("pl_font", QApplication::font().toString()).toString());
    m_extra_font = m_font;
    m_extra_font.setPointSize(m_font.pointSize() - 1);
    if (m_update)
    {
        delete m_metrics;
        delete m_extra_metrics;
    }
    m_update = true;
    m_metrics = new QFontMetrics(m_font);
    m_extra_metrics = new QFontMetrics(m_extra_font);
    m_row_height = m_metrics->lineSpacing() + 1;
}

void ListWidgetDrawer::loadColors()
{
    m_normal.setNamedColor(m_skin->getPLValue("normal"));
    m_current.setNamedColor(m_skin->getPLValue("current"));
    m_normal_bg.setNamedColor(m_skin->getPLValue("normalbg"));
    m_selected_bg.setNamedColor(m_skin->getPLValue("selectedbg"));
}

int ListWidgetDrawer::rowHeight() const
{
    return m_row_height;
}

int ListWidgetDrawer::numberWidth() const
{
    return m_number_width;
}

void ListWidgetDrawer::calculateNumberWidth(int count)
{
    //song numbers width
    if(m_show_number && m_align_numbres && count)
    {
        m_number_width = m_metrics->width("9") * QString::number(count).size();
    }
    else
        m_number_width = 0;
}

void ListWidgetDrawer::prepareRow(ListWidgetRow *row)
{
    if(row->flags & ListWidgetRow::GROUP)
    {
        row->titles[0] = m_metrics->elidedText (row->titles[0], Qt::ElideRight,
                                            row->rect.width() - m_number_width - 12 - 70);
        return;
    }

    if(m_show_number && !m_align_numbres)
        row->titles[0].prepend(QString("%1").arg(row->number)+". ");

    row->x[ListWidgetRow::NUMBER] = row->x[ListWidgetRow::TITLE] = row->rect.left() + PADDING;

    if(m_number_width)
    {
        QString number = QString("%1").arg(row->number);

        row->x[ListWidgetRow::NUMBER] += m_number_width - m_metrics->width(number);
        row->x[ListWidgetRow::TITLE] += m_number_width + m_metrics->width("9");
    }

    row->x[ListWidgetRow::LENGTH] = row->rect.right() - m_metrics->width(row->length) - PADDING;
    row->x[ListWidgetRow::EXTRA_STRING] = row->x[ListWidgetRow::LENGTH];

    int extra_string_width = row->extraString.isEmpty() ? 0 : m_extra_metrics->width(row->extraString);
    if(extra_string_width)
    {
        extra_string_width += row->length.isEmpty() ? 0 : m_extra_metrics->width("9")/2;
        row->x[ListWidgetRow::EXTRA_STRING] = row->x[ListWidgetRow::LENGTH] - extra_string_width;
    }

    //elide title
    int visible_width = row->x[ListWidgetRow::EXTRA_STRING] - row->x[ListWidgetRow::TITLE] -
            m_metrics->width("9");

    if( row->titles.count() == 1)
    {
        row->titles[0] = m_metrics->elidedText (row->titles[0], Qt::ElideRight, visible_width);
        return;
    }

    int offset = 0;
    for(int i = 0; i < row->titles.count(); ++i)
    {
        int width = qMin(QmmpUiSettings::instance()->columnManager()->size(i) - m_metrics->width(9),
                         visible_width - offset);

        if(width <= 0)
            break;
        row->titles[i] = m_metrics->elidedText (row->titles[i], Qt::ElideRight, width);
        offset += QmmpUiSettings::instance()->columnManager()->size(i);
    }
    //row->titles[0] = m_metrics->elidedText (row->titles[0], Qt::ElideRight, title_width);
}

void ListWidgetDrawer::fillBackground(QPainter *painter, int width, int height)
{
    painter->setBrush(m_normal_bg);
    painter->setPen(m_normal_bg);
    painter->drawRect(0,0,width,height);
}

void ListWidgetDrawer::drawBackground(QPainter *painter, ListWidgetRow *row)
{
    painter->setBrush((row->flags & ListWidgetRow::SELECTED) ? m_selected_bg : m_normal_bg);

    if(m_show_anchor && (row->flags & ListWidgetRow::ANCHOR))
    {
        painter->setPen(m_normal);
    }
    else if(row->flags & ListWidgetRow::SELECTED)
    {
        painter->setPen(m_selected_bg);
    }
    else
    {
        painter->setPen(m_normal_bg);
    }
    painter->drawRect(row->rect);
}

void ListWidgetDrawer::drawSeparator(QPainter *painter, ListWidgetRow *row, bool rtl)
{
    int sx = row->rect.x() + 50;
    int sy =  row->rect.y() + m_metrics->overlinePos() - 1;

    painter->setFont(m_font);
    painter->setPen(m_normal);

    if(m_number_width)
        sx += m_number_width + m_metrics->width("9");
    if(rtl)
        sx = row->rect.right() - sx - m_metrics->width(row->titles[0]);

    painter->drawText(sx, sy, row->titles[0]);

    sy = sy - m_metrics->lineSpacing()/2 + 2;

    if(rtl)
    {
        painter->drawLine(10, sy, sx - 5, sy);
        painter->drawLine(sx + m_metrics->width(row->titles[0]) + 5, sy,
                          sx + m_metrics->width(row->titles[0]) + 35, sy);
    }
    else
    {
        painter->drawLine(sx - 45, sy, sx - 5, sy);
        painter->drawLine(sx + m_metrics->width(row->titles[0]) + 5, sy,
                          row->rect.width(), sy);
    }
}

void ListWidgetDrawer::drawTrack(QPainter *painter, ListWidgetRow *row)
{
    int sy = row->rect.y() + m_metrics->overlinePos() - 1;

    painter->setFont(m_font);
    painter->setPen(row->flags & ListWidgetRow::CURRENT ? m_current : m_normal);

    if(m_number_width)
    {
        QString number = QString("%1").arg(row->number);
        painter->drawText(row->x[ListWidgetRow::NUMBER], sy, number);
    }

    int offset = 0;
    for(int i = 0; i < QmmpUiSettings::instance()->columnManager()->count(); i++)
    {
        if(row->x[ListWidgetRow::TITLE] + offset >= row->x[ListWidgetRow::EXTRA_STRING] - m_metrics->width("9"))
            break;

        painter->drawText(row->x[ListWidgetRow::TITLE] + offset, sy, row->titles[i]);
        offset += QmmpUiSettings::instance()->columnManager()->size(i);


        if(QmmpUiSettings::instance()->columnManager()->count() > 1 &&
                row->x[ListWidgetRow::TITLE] + offset < row->x[ListWidgetRow::EXTRA_STRING] - m_metrics->width("9"))
        {
            painter->drawLine(row->x[ListWidgetRow::TITLE] + offset - m_metrics->width("9")/2, row->rect.top(),
                    row->x[ListWidgetRow::TITLE] + offset - m_metrics->width("9")/2, row->rect.bottom() + 1);
        }
    }

    QString extra_string = row->extraString;

    if(!extra_string.isEmpty())
    {
        painter->setFont(m_extra_font);
        painter->drawText(row->x[ListWidgetRow::EXTRA_STRING], sy, extra_string);
        painter->setFont(m_font);
    }
    if(!row->length.isEmpty())
        painter->drawText(row->x[ListWidgetRow::LENGTH], sy, row->length);
}

void ListWidgetDrawer::drawDropLine(QPainter *painter, int row_number, int width)
{
    painter->setPen(m_current);
    painter->drawLine (5, row_number * m_row_height,
                       width - 5 , row_number * m_row_height);
}

void ListWidgetDrawer::drawVerticalLine(QPainter *painter, int row_count, int width, bool rtl)
{
    if(m_number_width)
    {
        painter->setPen(m_normal);
        int sx = rtl ? width - 10 - m_number_width - m_metrics->width("9")/2 - 1 :
                       5 + PADDING + m_number_width + m_metrics->width("9")/2 - 1;
        painter->drawLine(sx, 2, sx, row_count * (1 + m_metrics->lineSpacing()) - m_metrics->descent());
    }
}
