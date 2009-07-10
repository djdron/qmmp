/***************************************************************************
 *   Copyright (C) 2007 by Ilya Kotov                                      *
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


Analyzer::Analyzer (QWidget *parent)
        : Visual (parent), m_fps ( 20 )
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    restoreGeometry(settings.value("Analyzer/geometry").toByteArray());
    setFixedSize(2*300-30,105);
    m_pixmap = QPixmap (75,20);
    m_timer = new QTimer (this);
    connect(m_timer, SIGNAL (timeout()), this, SLOT (timeout()));
    m_nodes.clear();

    clear();
    setWindowTitle (tr("Qmmp Analyzer"));

    double peaks_speed[] = { 0.05, 0.1, 0.2, 0.4, 0.8 };
    double analyzer_speed[] = { 1.2, 1.8, 2.2, 2.8, 2.4 };
    int intervals[] = { 20 , 40 , 100 , 200 };

    m_peaks_falloff =
        peaks_speed[settings.value("Analyzer/peaks_falloff", 3).toInt()-1];
    m_analyzer_falloff =
        analyzer_speed[settings.value("Analyzer/analyzer_falloff", 3).toInt()-1];
    m_show_peaks = settings.value("Analyzer/show_peaks", TRUE).toBool();
    m_timer->setInterval(intervals[settings.value("Analyzer/refresh_rate", 2).toInt() - 1]);
    m_color1.setNamedColor(settings.value("Analyzer/color1", "Green").toString());
    m_color2.setNamedColor(settings.value("Analyzer/color2", "Yellow").toString());
    m_color3.setNamedColor(settings.value("Analyzer/color3", "Red").toString());
    m_bgColor.setNamedColor(settings.value("Analyzer/bg_color", "Black").toString());
    m_peakColor.setNamedColor(settings.value("Analyzer/peak_color", "Cyan").toString());
}

Analyzer::~Analyzer()
{
    while (!m_nodes.isEmpty())
        m_nodes.removeFirst();
}

void Analyzer::clear()
{
    while (!m_nodes.isEmpty())
        m_nodes.removeFirst();
    for ( int i = 0; i< 75; ++i )
    {
        m_intern_vis_data[i] = 0;
        m_peaks[i] = 0;
    }
    update();
}

void Analyzer::add ( Buffer *b, unsigned long w, int c, int p )
{
    if (!m_timer->isActive ())
        return;
    long len = b->nbytes, cnt;
    short *l = 0, *r = 0;

    len /= c;
    len /= ( p / 8 );
    if ( len > 512 )
        len = 512;
    cnt = len;

    if ( c == 2 )
    {
        l = new short[len];
        r = new short[len];

        if ( p == 8 )
            stereo16_from_stereopcm8 ( l, r, b->data, cnt );
        else if ( p == 16 )
            stereo16_from_stereopcm16 ( l, r, ( short * ) b->data, cnt );
    }
    else if ( c == 1 )
    {
        l = new short[len];

        if ( p == 8 )
            mono16_from_monopcm8 ( l, b->data, cnt );
        else if ( p == 16 )
            mono16_from_monopcm16 ( l, ( short * ) b->data, cnt );
    }
    else
        len = 0;

    if (len)
        m_nodes.append (new VisualNode (l, r, len, w));
}

void Analyzer::timeout()
{
    VisualNode *node = 0;

    //if ( /*playing &&*/ output())
    {
        //output()->mutex()->lock ();
        //long olat = output()->latency();
        //long owrt = output()->written();
        //output()->mutex()->unlock();

        //long synctime = owrt < olat ? 0 : owrt - olat;

        mutex()->lock ();
        VisualNode *prev = 0;
        while ((!m_nodes.isEmpty()))
        {
            node = m_nodes.takeFirst();
            /*if ( node->offset > synctime )
               break;*/

            if (prev)
                delete prev;
            prev = node;
        }
        mutex()->unlock();
        node = prev;
    }

    if (!node)
        return;
    process (node);
    delete node;
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

bool Analyzer::process (VisualNode *node)
{
    static fft_state *state = 0;
    if ( !state )
        state = fft_init();

    short dest_l[256];
    short dest_r[256];

    const int xscale_short[] =
        {
            0, 1, 2, 3, 4, 5, 6, 7, 8, 11, 15, 20, 27,
            36, 47, 62, 82, 107, 141, 184, 255
        };

    if ( node )
    {
        //i = node->length;
        calc_freq ( dest_l, node->left );
        if (node->right)
            calc_freq ( dest_r, node->right );
    }
    else
        return FALSE;
    const double y_scale = 3.60673760222;   /* 20.0 / log(256) */
    int yl,yr, j;

    for (int i = 0; i < 19; i++)
    {
        yl = yr = 0;

        for ( j = xscale_short[i];  j < xscale_short[i + 1]; j++ )
        {
            if ( dest_l[j] > yl )
                yl = dest_l[j];
            if ( dest_r[j] > yr && node->right)
                yr = dest_r[j];
        }
        yl >>= 7;
        int magnitude_l = 0;
        int magnitude_r = 0;
        if (node->right)
        {
            yr >>= 7;
        }
        if (yl)
        {
            magnitude_l = int(log (yl) * y_scale);
            if ( magnitude_l > 15 )
                magnitude_l = 15;
            if ( magnitude_l < 0 )
                magnitude_l = 0;
        }
        if (yr && node->right)
        {
            magnitude_r = int(log (yr) * y_scale);
            if ( magnitude_r > 15 )
                magnitude_r = 15;
            if ( magnitude_r < 0 )
                magnitude_r = 0;
        }

        m_intern_vis_data[i] -= m_analyzer_falloff;
        m_intern_vis_data[i] = magnitude_l > m_intern_vis_data[i]
                               ? magnitude_l : m_intern_vis_data[i];
        if (node->right)
        {
            m_intern_vis_data[37-i] -= m_analyzer_falloff;
            m_intern_vis_data[37-i] = magnitude_r > m_intern_vis_data[37-i]
                                      ? magnitude_r : m_intern_vis_data[37-i];
        }

        if (m_show_peaks)
        {
            m_peaks[i] -= m_peaks_falloff;
            m_peaks[i] = magnitude_l > m_peaks[i]
                         ? magnitude_l : m_peaks[i];
            if (node->right)
            {
                m_peaks[37-i] -= m_peaks_falloff;
                m_peaks[37-i] = magnitude_r > m_peaks[37-i]
                                ? magnitude_r : m_peaks[37-i];
            }
        }
    }
    return TRUE;
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
