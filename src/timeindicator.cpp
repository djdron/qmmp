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
#include <QPainter>
#include <QSettings>
#include <QMouseEvent>

#include "skin.h"
#include "timeindicator.h"

TimeIndicator::TimeIndicator ( QWidget *parent )
        : PixmapWidget ( parent )
{
    m_skin = Skin::getPointer();
    m_pixmap = QPixmap ( 65,13 );
    m_elapsed = true;
    m_time = m_songDuration = 0;
    readSettings();
    m_needToShowTime = false;
    updateSkin();
    connect(m_skin, SIGNAL(skinChanged()), this, SLOT(updateSkin()));
}

void TimeIndicator::setTime ( int t )
{
    m_time = t;
    m_pixmap.fill ( Qt::transparent );
    QPainter paint ( &m_pixmap );

    if (!m_elapsed)
    {
        t = m_songDuration - t;
        paint.drawPixmap(QPoint(2,0),m_skin->getNumber( 10 ));
    }
    if(t < 0)
        t = 0;

    paint.drawPixmap(QPoint(13,0),m_skin->getNumber( t/600%10 ));
    paint.drawPixmap(QPoint(26,0),m_skin->getNumber( t/60%10 ));
    paint.drawPixmap(QPoint(43,0),m_skin->getNumber( t%60/10 ));
    paint.drawPixmap(QPoint(56,0),m_skin->getNumber( t%60%10 ));

    setPixmap ( m_pixmap );

}

void TimeIndicator::reset()
{
    m_pixmap.fill ( Qt::transparent );
    QPainter paint ( &m_pixmap );
    setPixmap ( m_pixmap );
}

void TimeIndicator::mousePressEvent(QMouseEvent* e )
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
    if (m_needToShowTime)
        setTime(m_time);
}

void TimeIndicator::readSettings()
{
    QSettings settings(QDir::homePath()+"/.qmmp/qmmprc", QSettings::IniFormat);
    settings.beginGroup("Display");
    m_elapsed  = settings.value("Elapsed",true).toBool();
    settings.endGroup();
}


void TimeIndicator::writeSettings()
{
    QSettings settings(QDir::homePath()+"/.qmmp/qmmprc", QSettings::IniFormat);
    settings.beginGroup("Display");
    settings.setValue("Elapsed",m_elapsed);
    settings.endGroup();
}


void TimeIndicator::setNeedToShowTime(bool need)
{
    m_needToShowTime = need;
    if (!need) reset();
}

void TimeIndicator::mouseMoveEvent(QMouseEvent *)
{}

void TimeIndicator::mouseReleaseEvent(QMouseEvent *)
{}

