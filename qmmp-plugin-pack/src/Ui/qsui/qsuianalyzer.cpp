/***************************************************************************
 *   Copyright (C) 2007-2012 by Ilya Kotov                                 *
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
#include "qsuianalyzer.h"

#define VISUAL_NODE_SIZE 512 //samples
#define VISUAL_BUFFER_SIZE (5*VISUAL_NODE_SIZE)

QSUiAnalyzer::QSUiAnalyzer (QWidget *parent) : Visual (parent), m_fps (20)
{
    m_intern_vis_data = 0;
    m_peaks = 0;
    m_x_scale = 0;
    m_buffer_at = 0;
    m_rows = 0;
    m_cols = 0;
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    m_timer = new QTimer (this);
    connect(m_timer, SIGNAL (timeout()), this, SLOT (timeout()));
    m_left_buffer = new short[VISUAL_BUFFER_SIZE];
    m_right_buffer = new short[VISUAL_BUFFER_SIZE];

    clear();

    double peaks_speed[] = { 0.05, 0.1, 0.2, 0.4, 0.8 };
    double analyzer_speed[] = { 1.2, 1.8, 2.2, 2.8, 2.4 };
    int intervals[] = { 20 , 40 , 100 , 200 };

    settings.beginGroup("Simple");
    m_peaks_falloff = peaks_speed[settings.value("peaks_falloff", 3).toInt()-1];
    m_analyzer_falloff = analyzer_speed[settings.value("analyzer_falloff", 3).toInt()-1];
    m_show_peaks = settings.value("show_peaks", true).toBool();
    m_timer->setInterval(intervals[settings.value("refresh_rate", 2).toInt() - 1]);
    m_color1.setNamedColor(settings.value("color1", "#BECBFF").toString());
    m_color2.setNamedColor(settings.value("color2", "#BECBFF").toString());
    m_color3.setNamedColor(settings.value("color3", "#BECBFF").toString());
    m_bgColor.setNamedColor(settings.value("bg_color", "Black").toString());
    m_peakColor.setNamedColor(settings.value("peak_color", "#DDDDDD").toString());
    settings.endGroup();
    m_cell_size =  QSize(14, 8);
}

QSUiAnalyzer::~QSUiAnalyzer()
{
    delete [] m_left_buffer;
    delete [] m_right_buffer;

    if(m_peaks)
        delete [] m_peaks;
    if(m_intern_vis_data)
        delete [] m_intern_vis_data;
    if(m_x_scale)
        delete [] m_x_scale;
}

void QSUiAnalyzer::clear()
{
    m_buffer_at = 0;
    m_rows = 0;
    m_cols = 0;
    update();
}

void QSUiAnalyzer::add (unsigned char *data, qint64 size, int chan)
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

void QSUiAnalyzer::timeout()
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

void QSUiAnalyzer::paintEvent (QPaintEvent * e)
{
    QPainter painter (this);
    painter.fillRect(e->rect(),m_bgColor);
    draw(&painter);
}

void QSUiAnalyzer::hideEvent (QHideEvent *)
{
    m_timer->stop();
}

void QSUiAnalyzer::showEvent (QShowEvent *)
{
    m_timer->start();
}

void QSUiAnalyzer::process (short *left, short *right)
{
    static fft_state *state = 0;
    if (!state)
        state = fft_init();

    int rows = (height() - 2) / m_cell_size.height();
    int cols = (width() - 2) / m_cell_size.width();// / 2;

    if(m_rows != rows || m_cols != cols)
    {
        m_rows = rows;
        m_cols = cols;
        if(m_peaks)
            delete [] m_peaks;
        if(m_intern_vis_data)
            delete [] m_intern_vis_data;
        if(m_x_scale)
            delete [] m_x_scale;
        m_peaks = new double[m_cols];
        m_intern_vis_data = new double[m_cols];
        m_x_scale = new int[m_cols + 1];

        for(int i = 0; i < m_cols; ++i)
        {
            m_peaks[i] = 0;
            m_intern_vis_data[i] = 0;
        }
        for(int i = 0; i < m_cols + 1; ++i)
            m_x_scale[i] = pow(pow(255.0, 1.0 / m_cols), i);
    }

    short dest[256];

    short y;
    int k, magnitude;

    short data[256];

    for(int i = 0; i < VISUAL_NODE_SIZE; ++i)
    {
        data[i] = (left[i] >> 1) + (right[i] >> 1);
    }

    calc_freq (dest, data);

    double y_scale = (double) 1.25 * m_rows / log(256);

    for (int i = 0; i < m_cols; i++)
    {
        y = 0;
        magnitude = 0;

        if(m_x_scale[i] == m_x_scale[i + 1])
        {
            y = dest[i];
        }
        for (k = m_x_scale[i]; k < m_x_scale[i + 1]; k++)
        {
            y = qMax(dest[k], y);
        }

        y >>= 7; //256


        if (y)
        {
            magnitude = int(log (y) * y_scale);
            magnitude = qBound(0, magnitude, m_rows);
        }


        m_intern_vis_data[i] -= m_analyzer_falloff * m_rows / 15;
        m_intern_vis_data[i] = magnitude > m_intern_vis_data[i] ? magnitude : m_intern_vis_data[i];

        if (m_show_peaks)
        {
            m_peaks[i] -= m_peaks_falloff * m_rows / 15;
            m_peaks[i] = magnitude > m_peaks[i] ? magnitude : m_peaks[i];
        }
    }
}

void QSUiAnalyzer::draw (QPainter *p)
{
    QBrush brush(Qt::SolidPattern);
    int x = 0;

    for (int j = 0; j < m_cols; ++j)
    {
        x = j * m_cell_size.width() + 1;

        for (int i = 0; i <= m_intern_vis_data[j]; ++i)
        {
            if (i <= m_rows/3)
                brush.setColor(m_color1);
            else if (i > m_rows/3 && i <= 2 * m_rows / 3)
                brush.setColor(m_color2);
            else
                brush.setColor(m_color3);

            p->fillRect (x, height() - i * m_cell_size.height(),
                         m_cell_size.width() - 1, m_cell_size.height() - 4, brush);
        }

        if (m_show_peaks)
        {
            p->fillRect (x, height() - int(m_peaks[j]) * m_cell_size.height(),
                         m_cell_size.width() - 1, m_cell_size.height() - 4, m_peakColor);
        }
    }
}
