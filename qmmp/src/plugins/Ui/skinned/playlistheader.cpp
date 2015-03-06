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
#include <QFont>
#include <QFontMetrics>
#include <QSettings>
#include <QApplication>
#include <QMouseEvent>
#include <QMenu>
#include <QLineEdit>
#include <QInputDialog>
#include <qmmp/qmmp.h>
#include <qmmpui/qmmpuisettings.h>
#include <qmmpui/playlistmodel.h>
#include <qmmpui/columnmanager.h>
#include "skin.h"
#include "playlistheader.h"

PlayListHeader::PlayListHeader(QWidget *parent) :
    QWidget(parent)
{
    m_scrollable = false;
    m_metrics = 0;
    m_model = 0;
    m_show_number = false;
    m_align_numbres = false;
    m_number_width = 0;
    m_manager = QmmpUiSettings::instance()->columnManager();
    m_skin = Skin::instance();
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
    m_font.fromString(settings.value("pl_font", QApplication::font().toString()).toString());

    if (m_metrics)
    {
        delete m_metrics;
        m_metrics = 0;
    }
    m_metrics = new QFontMetrics(m_font);
    resize(width(), m_metrics->height () +1);
    m_show_number = settings.value ("pl_show_numbers", true).toBool();
    m_align_numbres = settings.value ("pl_align_numbers", false).toBool();
    settings.endGroup();
    updateList(PlayListModel::STRUCTURE);
}

void PlayListHeader::setModel(PlayListModel *selected, PlayListModel *previous)
{
    if(previous)
    {
        disconnect(previous, 0, this, 0); //disconnect previous model
    }
    connect (selected, SIGNAL(listChanged(int)), SLOT(updateList(int)));
    m_model = selected;
    updateList(PlayListModel::STRUCTURE);
}

void PlayListHeader::updateList(int flags)
{
    qDebug("1");
    if(flags & PlayListModel::STRUCTURE)
    {
        qDebug("2");
        //song numbers width
        if(m_show_number && m_align_numbres && m_model && m_model->count() > 0)
        {
            qDebug("3");
            m_number_width = m_metrics->width("9") * QString::number(m_model->count()).size();
        }
        else
            m_number_width = 0;
        qDebug("4");
    }
    qDebug("++%d++", m_number_width);
    update();
}

void PlayListHeader::updateSkin()
{
    loadColors();
    //drawButtons();
}

void PlayListHeader::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setFont(m_font);
    painter.setBrush(m_normal_bg);
    painter.drawRect(-1,-1,width()+1,height()+1);




    int sx = 5 + 3 + m_number_width + m_metrics->width("9");


    painter.setBrush(m_normal);
    painter.setPen(m_normal);
    painter.drawRect(5,-1,width()-10,height()+1);

    painter.setPen(m_normal_bg);

    //painter.drawLine(0, height()-1, width(), height()-1);

    if(m_number_width)
    painter.drawLine(5 + 3 + m_number_width + m_metrics->width("9")/2 - 1, 0, 5 + 3 + m_number_width + m_metrics->width("9")/2 - 1, height());

    for(int i = 0; i < m_manager->count(); ++i)
    {
        painter.drawText(sx + m_manager->size(i) / 2 - m_metrics->width(m_manager->name(i))/2, m_metrics->ascent(), m_manager->name(i));
        sx += m_manager->size(i);
        //if(i < m_manager->count() - 1)
        painter.drawLine(sx - m_metrics->width("9")/2, 0, sx - m_metrics->width("9")/2, height());
    }
}

void PlayListHeader::loadColors()
{
    m_normal.setNamedColor(m_skin->getPLValue("normal"));
    m_current.setNamedColor(m_skin->getPLValue("current"));
    m_normal_bg.setNamedColor(m_skin->getPLValue("normalbg"));
    m_selected_bg.setNamedColor(m_skin->getPLValue("selectedbg"));
}
