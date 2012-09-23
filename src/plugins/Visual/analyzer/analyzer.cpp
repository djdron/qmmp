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
#include "analyzer.h"

#define VISUAL_NODE_SIZE 512 //samples
#define VISUAL_BUFFER_SIZE (5*VISUAL_NODE_SIZE)

Analyzer::Analyzer (QWidget *parent) : Visual (parent), m_fps (20)
{
    m_intern_vis_data = 0;
    m_peaks = 0;
    m_x_scale = 0;
    m_buffer_at = 0;
    m_rows = 0;
    m_cols = 0;
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    restoreGeometry(settings.value("Analyzer/geometry").toByteArray());
    //setAttribute(Qt::WA_TranslucentBackground);
    setMinimumSize(2*300-30,105);
    m_timer = new QTimer (this);
    connect(m_timer, SIGNAL (timeout()), this, SLOT (timeout()));
    m_left_buffer = new short[VISUAL_BUFFER_SIZE];
    m_right_buffer = new short[VISUAL_BUFFER_SIZE];

    clear();
    setWindowTitle (tr("Qmmp Analyzer"));

    double peaks_speed[] = { 0.05, 0.1, 0.2, 0.4, 0.8 };
    double analyzer_speed[] = { 1.2, 1.8, 2.2, 2.4, 2.8 };
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
    //m_bgColor.setAlpha(0);
    m_peakColor.setNamedColor(settings.value("Analyzer/peak_color", "Cyan").toString());
    m_cell_size = settings.value("Analyzer/cells_size", QSize(15, 6)).toSize();

    QAction *fullScreenAction = new QAction(this);
    fullScreenAction->setShortcut(tr("F"));
    connect(fullScreenAction, SIGNAL(triggered()), SLOT(toggleFullScreen()));
    addAction(fullScreenAction);
}

Analyzer::~Analyzer()
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

void Analyzer::clear()
{
    m_buffer_at = 0;
    m_rows = 0;
    m_cols = 0;
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

    int rows = (height() - 2) / m_cell_size.height();
    int cols = (width() - 2) / m_cell_size.width() / 2;

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
        m_peaks = new double[m_cols * 2];
        m_intern_vis_data = new double[m_cols * 2];
        m_x_scale = new int[m_cols + 1];

        for(int i = 0; i < m_cols * 2; ++i)
        {
            m_peaks[i] = 0;
            m_intern_vis_data[i] = 0;
        }
        for(int i = 0; i < m_cols + 1; ++i)
            m_x_scale[i] = pow(pow(255.0, 1.0 / m_cols), i);
    }

    short dest_l[256];
    short dest_r[256];
    short yl, yr;
    int j, k, magnitude_l, magnitude_r;

    calc_freq (dest_l, left);
    calc_freq (dest_r, right);

    double y_scale = (double) 1.25 * m_rows / log(256);

    for (int i = 0; i < m_cols; i++)
    {
        j = m_cols * 2 - i - 1; //mirror index
        yl = yr = 0;
        magnitude_l = magnitude_r = 0;

        if(m_x_scale[i] == m_x_scale[i + 1])
        {
            yl = dest_l[i];
            yr = dest_r[i];
        }
        for (k = m_x_scale[i]; k < m_x_scale[i + 1]; k++)
        {
            yl = qMax(dest_l[k], yl);
            yr = qMax(dest_r[k], yr);
        }

        yl >>= 7; //256
        yr >>= 7;

        if (yl)
        {
            magnitude_l = int(log (yl) * y_scale);
            magnitude_l = qBound(0, magnitude_l, m_rows);
        }
        if (yr)
        {
            magnitude_r = int(log (yr) * y_scale);
            magnitude_r = qBound(0, magnitude_r, m_rows);
        }

        m_intern_vis_data[i] -= m_analyzer_falloff * m_rows / 15;
        m_intern_vis_data[i] = magnitude_l > m_intern_vis_data[i] ? magnitude_l : m_intern_vis_data[i];

        m_intern_vis_data[j] -= m_analyzer_falloff * m_rows / 15;
        m_intern_vis_data[j] = magnitude_r > m_intern_vis_data[j] ? magnitude_r : m_intern_vis_data[j];

        if (m_show_peaks)
        {
            m_peaks[i] -= m_peaks_falloff * m_rows / 15;
            m_peaks[i] = magnitude_l > m_peaks[i] ? magnitude_l : m_peaks[i];

            m_peaks[j] -= m_peaks_falloff * m_rows / 15;
            m_peaks[j] = magnitude_r > m_peaks[j] ? magnitude_r : m_peaks[j];
        }
    }
}

void Analyzer::draw (QPainter *p)
{
    QBrush brush(Qt::SolidPattern);
    int x = 0;
    int rdx = qMax(0, width() - 2 * m_cell_size.width() * m_cols);

    for (int j = 0; j < m_cols * 2; ++j)
    {
        x = j * m_cell_size.width() + 1;
        if(j >= m_cols)
            x += rdx; //correct right part position

        for (int i = 0; i <= m_intern_vis_data[j]; ++i)
        {
            if (i <= m_rows/3)
                brush.setColor(m_color1);
            else if (i > m_rows/3 && i <= 2 * m_rows / 3)
                brush.setColor(m_color2);
            else
                brush.setColor(m_color3);

            p->fillRect (x, height() - i * m_cell_size.height() + 1,
                         m_cell_size.width() - 2, m_cell_size.height() - 2, brush);
        }

        if (m_show_peaks)
        {
            p->fillRect (x, height() - int(m_peaks[j])*m_cell_size.height() + 1,
                         m_cell_size.width() - 2, m_cell_size.height() - 2, m_peakColor);
        }
    }
}

void Analyzer::toggleFullScreen()
{
    setWindowState(windowState() ^Qt::WindowFullScreen);
}
