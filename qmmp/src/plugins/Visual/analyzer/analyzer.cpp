/***************************************************************************
 *   Copyright (C) 2007-2010 by Ilya Kotov                                 *
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
#include <QTimer>
#include <QSettings>
#include <QPainter>
#include <QMenu>
#include <QActionGroup>

#include <qmmp/buffer.h>
#include <qmmp/output.h>
#include <math.h>
#include <stdlib.h>

#include "fft.h"
#include "inlines.h"
#include "analyzer.h"

#define VISUAL_NODE_SIZE 512 //samples
#define VISUAL_BUFFER_SIZE (5*VISUAL_NODE_SIZE)

Analyzer::Analyzer (QWidget *parent)
        : Visual (parent), m_fps (20)
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    restoreGeometry(settings.value("Analyzer/geometry").toByteArray());
    setFixedSize(2*300-30,105);
    m_pixmap = QPixmap (75,20);
    m_timer = new QTimer (this);
    connect(m_timer, SIGNAL (timeout()), this, SLOT (timeout()));
    m_left_buffer = new short[VISUAL_BUFFER_SIZE];
    m_right_buffer = new short[VISUAL_BUFFER_SIZE];
    m_buffer_at = 0;
    clear();
    setWindowTitle (tr("Qmmp Analyzer"));

    double peaks_speed[] = { 0.05, 0.1, 0.2, 0.4, 0.8 };
    double analyzer_speed[] = { 1.2, 1.8, 2.2, 2.8, 2.4 };
    int intervals[] = { 20 , 40 , 100 , 200 };

    m_peaks_falloff =
        peaks_speed[settings.value("Analyzer/peaks_falloff", 3).toInt()-1];
    m_analyzer_falloff =
        analyzer_speed[settings.value("Analyzer/analyzer_falloff", 3).toInt()-1];
    m_show_peaks = settings.value("Analyzer/show_peaks", true).toBool();
    m_timer->setInterval(intervals[settings.value("Analyzer/refresh_rate", 2).toInt() - 1]);
    m_color1.setNamedColor(settings.value("Analyzer/color1", "Green").toString());
    m_color2.setNamedColor(settings.value("Analyzer/color2", "Yellow").toString());
    m_color3.setNamedColor(settings.value("Analyzer/color3", "Red").toString());
    m_bgColor.setNamedColor(settings.value("Analyzer/bg_color", "Black").toString());
    m_peakColor.setNamedColor(settings.value("Analyzer/peak_color", "Cyan").toString());
}

Analyzer::~Analyzer()
{
    delete [] m_left_buffer;
    delete [] m_right_buffer;
}

void Analyzer::clear()
{
    m_buffer_at = 0;
    for (int i = 0; i< 75; ++i)
    {
        m_intern_vis_data[i] = 0;
        m_peaks[i] = 0;
    }
    update();
}

void Analyzer::add (unsigned char *data, qint64 size, int chan)
{
    if (!m_timer->isActive ())
        return;

    if(VISUAL_BUFFER_SIZE == m_buffer_at)
    {
        m_buffer_at -= VISUAL_NODE_SIZE;
        memmove(m_left_buffer, m_left_buffer + VISUAL_NODE_SIZE, m_buffer_at << 1);
        memmove(m_right_buffer, m_right_buffer + VISUAL_NODE_SIZE, m_buffer_at << 1);
        return;
    }

    int frames = qMin((int)size/chan >> 1, VISUAL_BUFFER_SIZE - m_buffer_at);

    if (chan >= 2)
    {
        stereo16_from_multichannel(m_left_buffer + m_buffer_at,
                                   m_right_buffer + m_buffer_at,(short *) data, frames, chan);
    }
    else
    {
        memcpy(m_left_buffer + m_buffer_at, (short *) data, frames << 1);
        memcpy(m_right_buffer + m_buffer_at, (short *) data, frames << 1);
    }

    m_buffer_at += frames;
}

void Analyzer::timeout()
{
    mutex()->lock ();
    if(m_buffer_at < VISUAL_NODE_SIZE)
    {
        mutex()->unlock ();
        return;
    }

    process (m_left_buffer, m_right_buffer);
    m_buffer_at -= VISUAL_NODE_SIZE;
    memmove(m_left_buffer, m_left_buffer + VISUAL_NODE_SIZE, m_buffer_at << 1);
    memmove(m_right_buffer, m_right_buffer + VISUAL_NODE_SIZE, m_buffer_at << 1);
    mutex()->unlock ();
    update();
}

void Analyzer::paintEvent (QPaintEvent * e)
{
    QPainter painter (this);
    painter.fillRect(e->rect(),m_bgColor);
    draw(&painter);
}

void Analyzer::hideEvent (QHideEvent *)
{
    m_timer->stop();
}

void Analyzer::showEvent (QShowEvent *)
{
    m_timer->start();
}

void Analyzer::closeEvent (QCloseEvent *event)
{
    //save geometry
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.setValue("Analyzer/geometry", saveGeometry());
    Visual::closeEvent(event); //removes visualization before class deleting
}

void Analyzer::process (short *left, short *right)
{
    static fft_state *state = 0;
    if (!state)
        state = fft_init();

    short dest_l[256];
    short dest_r[256];

    const int xscale_short[] =
        {
            0, 1, 2, 3, 4, 5, 6, 7, 8, 11, 15, 20, 27,
            36, 47, 62, 82, 107, 141, 184, 255
        };

    calc_freq (dest_l, left);
    calc_freq (dest_r, right);

    const double y_scale = 3.60673760222;   /* 20.0 / log(256) */
    int yl,yr, j;

    for (int i = 0; i < 19; i++)
    {
        yl = yr = 0;

        for (j = xscale_short[i];  j < xscale_short[i + 1]; j++)
        {
            if (dest_l[j] > yl)
                yl = dest_l[j];
            if (dest_r[j] > yr)
                yr = dest_r[j];
        }
        yl >>= 7;
        yr >>= 7;
        int magnitude_l = 0;
        int magnitude_r = 0;

        if (yl)
        {
            magnitude_l = int(log (yl) * y_scale);
            if (magnitude_l > 15)
                magnitude_l = 15;
            if (magnitude_l < 0)
                magnitude_l = 0;
        }
        if (yr)
        {
            magnitude_r = int(log (yr) * y_scale);
            if (magnitude_r > 15)
                magnitude_r = 15;
            if (magnitude_r < 0)
                magnitude_r = 0;
        }

        m_intern_vis_data[i] -= m_analyzer_falloff;
        m_intern_vis_data[i] = magnitude_l > m_intern_vis_data[i]
                               ? magnitude_l : m_intern_vis_data[i];

        m_intern_vis_data[37-i] -= m_analyzer_falloff;
        m_intern_vis_data[37-i] = magnitude_r > m_intern_vis_data[37-i]
                                  ? magnitude_r : m_intern_vis_data[37-i];

        if (m_show_peaks)
        {
            m_peaks[i] -= m_peaks_falloff;
            m_peaks[i] = magnitude_l > m_peaks[i] ? magnitude_l : m_peaks[i];

            m_peaks[37-i] -= m_peaks_falloff;
            m_peaks[37-i] = magnitude_r > m_peaks[37-i] ? magnitude_r : m_peaks[37-i];
        }
    }
}

void Analyzer::draw (QPainter *p)
{
    QBrush brush(Qt::SolidPattern);
    for (int j = 0; j < 19; ++j)
    {
        for (int i = 0; i <= m_intern_vis_data[j]; ++i)
        {
            if (i <= 5)
                brush.setColor(m_color1);
            else if (i > 5 && i <= 10)
                brush.setColor(m_color2);
            else
                brush.setColor(m_color3);
            p->fillRect (j*15+1, height() - i*7, 12, 4, brush);
        }

        for (int i = 0; i <= m_intern_vis_data[19+j]; ++i)
        {
            if (i <= 5)
                brush.setColor(m_color1);
            else if (i > 5 && i <= 10)
                brush.setColor(m_color2);
            else
                brush.setColor(m_color3);
            p->fillRect ((j+19)*15+1, height() - i*7, 12, 4, brush);
        }

        if (m_show_peaks)
        {
            p->fillRect (j*15+1, height() - int(m_peaks[j])*7, 12, 4, m_peakColor);
            p->fillRect ((j+19)*15+1, height() - int(m_peaks[j+19])*7, 12, 4, m_peakColor);
        }
    }
}
