/***************************************************************************
 *   Copyright (C) 2006-2008 by Ilya Kotov                                 *
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
#include <QTimer>
#include <QSettings>
#include <QAction>
#include <QMenu>
#include <QMouseEvent>
#include <QSettings>
#include <qmmp/qmmp.h>

#include "skin.h"
#include "textscroller.h"

#define SCROLL_SEP "*** "

TextScroller *TextScroller::pointer = 0;

TextScroller *TextScroller::getPointer()
{
    return pointer;
}


TextScroller::TextScroller ( QWidget *parent )
        : QWidget ( parent )
{
    pointer = this;
    m_skin = Skin::getPointer();
    m_pixmap = QPixmap ( 150,15 );
    resize(150,15);
    x = 0;
    m_progress = -1;
    m_text = "Qt-based Multimedia Player (Qmmp " + Qmmp::strVersion() + ")";
    m_update = FALSE;
    readSettings();
    m_timer = new QTimer ( this );
    connect (m_timer, SIGNAL (timeout()), SLOT (addOffset()));
    m_timer->setInterval(50);
    m_timer->start();
    updateSkin();
    connect(m_skin, SIGNAL(skinChanged()), this, SLOT(updateSkin()));
    m_autoscroll = TRUE;
    m_menu = new QMenu(this);
    QAction *autoscrollAction = new QAction(tr("Autoscroll Songname"), m_menu);
    autoscrollAction->setCheckable (TRUE);
    connect(autoscrollAction, SIGNAL(toggled(bool)), SLOT(setAutoscroll(bool)));
    m_menu->addAction(autoscrollAction);
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    autoscrollAction->setChecked(settings.value("TextScroller/autoscroll", TRUE).toBool());
    setAutoscroll(autoscrollAction->isChecked());
}


TextScroller::~TextScroller()
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.setValue("TextScroller/autoscroll", m_autoscroll);
}

void TextScroller::addOffset()
{
    x--;
    if ( 154 + x < - m_metrics->width (m_scrollText) - 15 +1)
    {
        x=-154;
    }
    update();
}

void TextScroller::setText(const QString& text)
{
    m_progress = -1;
    if (isVisible() && m_autoscroll)
        m_timer->start();
    if (m_text != text)
    {
        m_text = text;
        m_scrollText = "*** " + text;
        x = m_autoscroll ? -50 : -150;
    }
    update();
}

void TextScroller::updateSkin()
{
    m_color.setNamedColor(m_skin->getPLValue("mbfg"));
}


void TextScroller::setAutoscroll(bool enabled)
{
    m_autoscroll = enabled;
    x = -150;
    if (m_autoscroll)
    {
        if (isVisible())
            m_timer->start();
    }
    else
    {
        m_timer->stop();
        update();
    }
}

void TextScroller::readSettings()
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    QString fontname = settings.value("MainWindow/Font").toString();
    if(!fontname.isEmpty())
        m_font.fromString(fontname);

    if (m_update)
    {
        delete m_metrics;
        m_metrics = new QFontMetrics(m_font);
    }
    else
    {
        m_update = TRUE;
        m_metrics = new QFontMetrics(m_font);
    }
}

void TextScroller::setProgress(int progress)
{
    m_timer->stop();
    m_progress = progress;
    update();
}

void TextScroller::hideEvent (QHideEvent *)
{
    m_timer->stop();
}

void TextScroller::showEvent (QShowEvent *)
{
    if (m_autoscroll)
        m_timer->start();
}

void TextScroller::paintEvent (QPaintEvent *)
{
    QPainter paint (this);
    paint.setPen(m_color);
    paint.setFont(m_font);
    if (m_progress < 0)
    {
        if (m_autoscroll)
        {
            paint.drawText (154 + x + m_metrics->width (m_scrollText) + 15,12, m_scrollText);
            paint.drawText (154 + x,12, m_scrollText);
        }
        else
            paint.drawText (154 + x,12, m_text);
    }
    else
        paint.drawText (4,12, tr("Buffering:") + QString(" %1\%").arg(m_progress));
}

void TextScroller::mousePressEvent (QMouseEvent *e)
{
    if (e->button() == Qt::RightButton)
        m_menu->exec(e->globalPos());
    else
        QWidget::mousePressEvent(e);
}

