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
#include <QPainter>
#include <QSettings>
#include <QMouseEvent>
#include <QTimer>
#include <qmmp/qmmp.h>

#include "skin.h"
#include "timeindicator.h"

TimeIndicator::TimeIndicator (QWidget *parent)
        : PixmapWidget (parent)
{
    m_skin = Skin::instance();
    m_pixmap = QPixmap (65 * m_skin->ratio(),13 * m_skin->ratio());
    m_elapsed = true;
    m_time = m_songDuration = 0;
    readSettings();
    m_needToShowTime = false;
    updateSkin();
    reset();
    connect(m_skin, SIGNAL(skinChanged()), this, SLOT(updateSkin()));
    m_timer = new QTimer(this);
    m_timer->setInterval(125);
    m_timer->setSingleShot (true);
    connect(m_timer, SIGNAL(timeout()),SLOT(reset()));
}

void TimeIndicator::setTime (int t)
{
    m_time = t;
    m_pixmap.fill (Qt::transparent);
    int r = m_skin->ratio();
    QPainter paint (&m_pixmap);

    if (!m_elapsed)
    {
        t = m_songDuration - t;
        paint.drawPixmap(r*2,0,m_skin->getNumber(10));
    }
    if (t < 0)
        t = 0;

    if(t > 3600)
        t /= 60;

    paint.drawPixmap(r*13,0,m_skin->getNumber(t/600%10));
    paint.drawPixmap(r*26,0,m_skin->getNumber(t/60%10));
    paint.drawPixmap(r*43,0,m_skin->getNumber(t%60/10));
    paint.drawPixmap(r*56,0,m_skin->getNumber(t%60%10));

    setPixmap (m_pixmap);

}

void TimeIndicator::reset()
{
    m_pixmap.fill (Qt::transparent);
    QPainter paint (&m_pixmap);
    setPixmap (m_pixmap );
}

void TimeIndicator::mousePressEvent(QMouseEvent* e)
{
    if (m_needToShowTime && e->button() & Qt::LeftButton)
    {
        m_elapsed = m_elapsed ? false : true;
        setTime(m_time);
    }
    PixmapWidget::mousePressEvent(e);
}

void TimeIndicator::setSongDuration(int d)
{
    m_songDuration = d;
}

TimeIndicator::~TimeIndicator()
{
    writeSettings();
}


void TimeIndicator::updateSkin()
{
    m_pixmap = QPixmap (65 * m_skin->ratio(),13 * m_skin->ratio());
    if (m_needToShowTime)
        setTime(m_time);
}

void TimeIndicator::readSettings()
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("Display");
    m_elapsed  = settings.value("Elapsed",true).toBool();
    settings.endGroup();
}

void TimeIndicator::writeSettings()
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("Display");
    settings.setValue("Elapsed",m_elapsed);
    settings.endGroup();
}

void TimeIndicator::setNeedToShowTime(bool need)
{
    m_needToShowTime = need;
    if (!need)
        m_timer->start();
    else
        m_timer->stop();
}

void TimeIndicator::mouseMoveEvent(QMouseEvent *)
{}

void TimeIndicator::mouseReleaseEvent(QMouseEvent *)
{}

