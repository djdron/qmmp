/***************************************************************************
 *   Copyright (C) 2009-2010 by Ilya Kotov                                 *
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
#include <qmmpui/playlistmanager.h>
#include "skin.h"
#include "playlistselector.h"

PlayListSelector::PlayListSelector(PlayListManager *manager, QWidget *parent) : QWidget(parent)
{
    m_update = FALSE;
    m_scrollable = FALSE;
    m_left_pressed = FALSE;
    m_right_pressed = FALSE;
    m_offset = 0;
    m_offset_max = 0;
    m_skin = Skin::instance();
    m_pl_manager = manager;
    connect(m_pl_manager, SIGNAL(playListsChanged()), SLOT(updateTabs()));
    connect(m_skin, SIGNAL(skinChanged()), this, SLOT(updateSkin()));    
    loadColors();
    readSettings();
    updateTabs();
    m_menu = new QMenu(this);
    m_menu->addAction(tr("&Load"), parent, SIGNAL (loadPlaylist()));
    m_menu->addAction(tr("&Save As..."), parent, SIGNAL (savePlaylist()));
    m_menu->addSeparator();
    m_menu->addAction(tr("Rename"),this, SLOT (renamePlaylist()));
    m_menu->addAction(tr("&Delete"),parent, SLOT (deletePlaylist()));
}

PlayListSelector::~PlayListSelector()
{}

void PlayListSelector::readSettings()
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    m_font.fromString(settings.value("PlayList/Font", QApplication::font().toString()).toString());
    if (m_update)
    {
        delete m_metrics;
        m_metrics = new QFontMetrics(m_font);
        updateTabs();
    }
    else
    {
        m_update = TRUE;
    }
    m_metrics = new QFontMetrics(m_font);

    resize(width(), m_metrics->height () +1);
    drawButtons();
}

void PlayListSelector::updateTabs()
{
    m_rects.clear();
    QRect rect;
    foreach(QString text, m_pl_manager->playListNames())
    {
        if(m_rects.isEmpty())
            rect.setX(9);
        else
            rect.setX(m_rects.last().right() + m_metrics->width(" | "));
        rect.setY(0);
        rect.setWidth(m_metrics->width(text));
        rect.setHeight(m_metrics->ascent ());
        m_rects.append(rect);
    }
    updateScrollers();
    update();
}

void PlayListSelector::updateSkin()
{
    loadColors();
    drawButtons();
    updateTabs();
}

void PlayListSelector::renamePlaylist()
{
    bool ok = FALSE;
    QString name = QInputDialog::getText (this,
                                          tr("Rename Playlist"), tr("Playlist name:"),
                                          QLineEdit::Normal,
                                          m_pl_manager->selectedPlayList()->name(), &ok);
    if(ok)
        m_pl_manager->selectedPlayList()->setName(name);
}

void PlayListSelector::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setFont(m_font);
    painter.setBrush(QBrush(m_normal_bg));
    painter.drawRect(-1,-1,width()+1,height()+1);
    QStringList names = m_pl_manager->playListNames();
    int current = m_pl_manager->indexOf(m_pl_manager->currentPlayList());
    int selected = m_pl_manager->indexOf(m_pl_manager->selectedPlayList());
    painter.setBrush(QBrush(m_selected_bg));
    painter.setPen(m_selected_bg);
    painter.drawRect(m_rects.at(selected).x()- 3 - m_offset, 0,
                     m_rects.at(selected).width()+4, height()-1);

    for (int i = 0; i < m_rects.size(); ++i)
    {
        if(i == current)
             painter.setPen(m_current);
        else
             painter.setPen(m_normal);
        painter.drawText(m_rects[i].x() - m_offset, m_metrics->ascent(), names.at(i));
        if(i < m_rects.size() - 1)
        {
            painter.setPen(m_normal);
            painter.drawText(m_rects[i].x() + m_rects[i].width() - m_offset, m_metrics->ascent(), " | ");
        }
    }
    if(m_scrollable)
    {
        painter.drawPixmap(width()-40, 0, m_pixmap);
        painter.setBrush(QBrush(m_normal_bg));
        painter.setPen(m_normal_bg);
        painter.drawRect(0,0,6,height());
    }
}

void PlayListSelector::mousePressEvent (QMouseEvent *e)
{
    if(m_scrollable && e->x() > width() - 20)
    {
        m_offset += m_rects.at(lastVisible()).right() - m_offset - width() + 42;
        m_offset = qMin(m_offset, m_offset_max);
        m_right_pressed = TRUE;
        drawButtons();
        update();
        return;
    }
    if(m_scrollable && (width() - 40 < e->x()) && (e->x() <= width() - 20))
    {
        m_offset -=  11 - m_rects.at(firstVisible()).x() + m_offset;
        m_offset = qMax(0, m_offset);
        m_left_pressed = TRUE;
        drawButtons();
        update();
        return;
    }

    QPoint pp = e->pos();
    pp.rx() += m_offset;
    for(int i = 0; i < m_rects.count(); ++i)
    {
        if(m_rects.at(i).contains(pp))
        {
            m_pl_manager->selectPlayList(i);
            break;
        }
    }
    update();
    if(e->button() == Qt::RightButton)
        m_menu->exec(e->globalPos());
}

void PlayListSelector::mouseReleaseEvent (QMouseEvent *e)
{
    m_left_pressed = FALSE;
    m_right_pressed = FALSE;
    drawButtons();
    update();
    QWidget::mouseReleaseEvent(e);
}

void PlayListSelector::mouseDoubleClickEvent (QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton && !(m_scrollable && (e->x() > width() - 40)))
        renamePlaylist();
    else
        QWidget::mouseDoubleClickEvent(e);
}

void PlayListSelector::resizeEvent (QResizeEvent *)
{
    updateScrollers();
}

void PlayListSelector::loadColors()
{
    m_normal.setNamedColor(m_skin->getPLValue("normal"));
    m_current.setNamedColor(m_skin->getPLValue("current"));
    m_normal_bg.setNamedColor(m_skin->getPLValue("normalbg"));
    m_selected_bg.setNamedColor(m_skin->getPLValue("selectedbg"));
}

void PlayListSelector::drawButtons()
{
    m_pixmap = QPixmap(40, height());
    m_pixmap.fill(m_normal_bg);
    QPainter painter(&m_pixmap);
    painter.setPen(m_left_pressed ? m_current : m_normal);
    painter.setBrush(QBrush(m_left_pressed ? m_current : m_normal));
    QPoint points[3] = {
        QPoint(m_pixmap.width() - 25, height()/2 - 5),
        QPoint(m_pixmap.width() - 35, height()/2-1),
        QPoint(m_pixmap.width() - 25, height()/2 + 3),
    };
    painter.drawPolygon(points, 3);
    painter.setPen(m_right_pressed ? m_current : m_normal);
    painter.setBrush(QBrush(m_right_pressed ? m_current : m_normal));
    QPoint points2[3] = {
        QPoint(m_pixmap.width() - 20, height()/2 - 5),
        QPoint(m_pixmap.width() - 10, height()/2-1),
        QPoint(m_pixmap.width() - 20, height()/2 + 3),
    };
    painter.drawPolygon(points2, 3);
}

void PlayListSelector::updateScrollers()
{
    m_scrollable = m_rects.last().right() > width();
    if(m_scrollable)
    {
        m_offset_max = m_rects.last().right() - width() + 42;
        m_offset = qMin(m_offset, m_offset_max);
    }
    else
    {
         m_offset = 0;
         m_offset_max = 0;
    }
}

int PlayListSelector::firstVisible()
{
    for(int i = 0; i < m_rects.size(); ++i)
    {
        if(m_rects.at(i).right() - m_offset + m_metrics->width(" - ") + 2 >= 9)
            return i;
    }
    return 0;
}

int PlayListSelector::lastVisible()
{
    for(int i = m_rects.size() - 1; i >= 0; --i)
    {
        if(m_rects.at(i).x() - m_offset -  m_metrics->width(" - ") - 2 <= width() - 40)
            return i;
    }
    return m_rects.count() - 1;
}
