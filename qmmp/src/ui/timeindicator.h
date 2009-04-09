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
#ifndef TIMEINDICATOR_H
#define TIMEINDICATOR_H

#include "pixmapwidget.h"

class QMouseEvent;
class QTimer;

class Skin;


/** Class TimeIndicator
 *  @author Vladimir Kuznetsov <vovanec@gmail.com>
 *
 *  Represents time indicator in the main display. Can show elapsed
 *  and rest time of song (mouse press on indicator changes mode)
 */
class TimeIndicator : public PixmapWidget
{
    Q_OBJECT
public:
    TimeIndicator(QWidget *parent = 0);
    ~TimeIndicator();
    void setTime ( int t );
    void setSongDuration(int);
    void setNeedToShowTime(bool);

protected:
    virtual void mousePressEvent(QMouseEvent*);
    virtual void mouseMoveEvent(QMouseEvent*);
    virtual void mouseReleaseEvent(QMouseEvent*);
    void writeSettings();
    void readSettings();

private slots:
    void updateSkin();
    void reset();

private:
    QPixmap m_pixmap;
    Skin *m_skin;
    int m_time;
    int m_songDuration;
    bool m_elapsed;
    bool m_needToShowTime;
    QTimer *m_timer;
};

#endif
