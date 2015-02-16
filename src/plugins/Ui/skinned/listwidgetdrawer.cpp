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
#include "skin.h"
#include "listwidgetdrawer.h"

ListWidgetDrawer::ListWidgetDrawer()
{
    m_skin = Skin::instance();
    m_update = false;
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
}

void ListWidgetDrawer::loadColors()
{
    m_normal.setNamedColor(m_skin->getPLValue("normal"));
    m_current.setNamedColor(m_skin->getPLValue("current"));
    m_normal_bg.setNamedColor(m_skin->getPLValue("normalbg"));
    m_selected_bg.setNamedColor(m_skin->getPLValue("selectedbg"));
}

void ListWidgetDrawer::fillBackground(QPainter *painter, int width, int height)
{
    painter->setBrush(m_normal_bg);
    painter->setPen(m_normal_bg);
    painter->drawRect(0,0,width,height);
}

void  ListWidgetDrawer::drawBackground(QPainter *painter, ListWidgetRow *row)
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

void ListWidgetDrawer::drawSeparator(QPainter *painter, int m_number_width, ListWidgetRow *row, bool rtl)
{
    int sx = row->rect.x() + 50;

    painter->setPen(m_normal);

    if(m_number_width)
        sx += m_number_width + m_metrics->width("9");
    if(rtl)
        sx = row->rect.right() - sx - m_metrics->width(row->title);

    painter->drawText(sx, row->textY, row->title);

    int sy = row->textY - m_metrics->lineSpacing()/2 + 2;

    if(rtl)
    {
        painter->drawLine(10, sy, sx - 5, sy);
        painter->drawLine(sx + m_metrics->width(row->title) + 5, sy,
                         sx + m_metrics->width(row->title) + 35, sy);
    }
    else
    {
        painter->drawLine(sx - 45, sy, sx - 5, sy);
        painter->drawLine(sx + m_metrics->width(row->title) + 5, sy,
                         row->rect.width(), sy);
    }
}

void ListWidgetDrawer::drawTrack(QPainter *painter, int m_number_width, ListWidgetRow *row, bool rtl)
{
    int sx = 0;

    painter->setPen(row->flags & ListWidgetRow::CURRENT ? m_current : m_normal);

    if(m_number_width)
    {
        QString number = QString("%1").arg(row->number);
        sx = 10 + m_number_width - m_metrics->width(number);
        if(rtl)
            sx = row->rect.right() - sx - m_metrics->width(number);

        painter->drawText(sx, row->textY, number);

        sx = 10 + m_number_width + m_metrics->width("9");
        if(rtl)
            sx = row->rect.right() - sx - m_metrics->width(row->title);
    }
    else
    {
        sx = rtl ? row->rect.right() - 10 - m_metrics->width(row->title) : 10;
    }

    painter->drawText(sx, row->textY, row->title);

    QString extra_string = row->extraString;

    if(!extra_string.isEmpty())
    {
        painter->setFont(m_extra_font);

        if(row->length.isEmpty())
        {
            sx = rtl ? 7 : row->rect.right() - 7 - m_extra_metrics->width(extra_string);
            painter->drawText(sx, row->textY, extra_string);
        }
        else
        {
            sx = row->rect.right() - 10 - m_extra_metrics->width(extra_string) - m_metrics->width(row->length);
            if(rtl)
                sx = row->rect.right() - sx - m_extra_metrics->width(extra_string);
            painter->drawText(sx, row->textY, extra_string);
        }
        painter->setFont(m_font);
    }
    sx = rtl ? 9 : row->rect.right() - 7 - m_metrics->width(row->length);
    painter->drawText(sx, row->textY, row->length);
}

void ListWidgetDrawer::drawDropLine(QPainter *painter, int row_number, int width)
{
    painter->setPen(m_current);
    painter->drawLine (5, row_number * (m_metrics->lineSpacing() + 1),
                       width - 5 , row_number * (m_metrics->lineSpacing() + 1));
}

void ListWidgetDrawer::drawVerticalLine(QPainter *painter, int m_number_width, int row_count, int width, bool rtl)
{
    painter->setPen(m_normal);
    int sx = rtl ? width - 10 - m_number_width - m_metrics->width("9")/2 - 1 :
                   10 + m_number_width + m_metrics->width("9")/2 - 1;
    painter->drawLine(sx, 2, sx, row_count * (1 + m_metrics->lineSpacing()) - 2 - m_metrics->descent());
}
