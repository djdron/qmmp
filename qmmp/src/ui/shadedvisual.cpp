/***************************************************************************
 *   Copyright (C) 2007-2009 by Ilya Kotov                                 *
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
#include <math.h>
#include "skin.h"
#include "mainvisual.h"
#include "inlines.h"
#include "shadedvisual.h"

ShadedVisual::ShadedVisual(QWidget *parent) : Visual(parent)
{    
    m_skin = Skin::instance();
    m_ratio = m_skin->ratio();
    resize(m_ratio*38,m_ratio*5);
    m_pixmap = QPixmap (m_ratio*38,m_ratio*5);
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL (timeout()), this, SLOT (timeout()));
    connect(m_skin, SIGNAL(skinChanged()), this, SLOT(updateSkin()));
    m_timer->setInterval(50);
    m_timer->start();
    clear();
}

ShadedVisual::~ShadedVisual()
{}

void ShadedVisual::add(unsigned char *data, qint64 size, int chan)
{
    if (!m_timer->isActive ())
        return;
    long len = size, cnt;
    short *l = 0, *r = 0;

    len /= chan;
    len /= 2;
    if ( len > 512 )
        len = 512;
    cnt = len;

    if (chan == 2)
    {
        l = new short[len];
        r = new short[len];
        stereo16_from_stereopcm16 ( l, r, (short *) data, cnt);
    }
    else if (chan == 1)
    {
        l = new short[len];
        mono16_from_monopcm16 (l, (short *) data, cnt);
    }
    else
        len = 0;

    if (len)
        m_nodes.append (new VisualNode (l, r, len));
}

void ShadedVisual::clear()
{
    while (!m_nodes.isEmpty())
        m_nodes.removeFirst();
    m_l = 0;
    m_r = 0;
    m_pixmap.fill(m_skin->getVisColor(0));
    update();
}

void ShadedVisual::timeout()
{
    VisualNode *node = 0;
    m_pixmap.fill(m_skin->getVisColor(0));

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

    if (!node)
        return;
    process (node);
    delete node;
    QPainter p(&m_pixmap);
    draw (&p);
    update();
}

void ShadedVisual::process (VisualNode *node)
{
    if (!node)
        return;

    int step = (node->length << 8)/74;
    int pos = 0;
    int l = 0;
    int r = 0;
    int j_l = 0, j_r = 0;

    for (int i = 0; i < 75; ++i)
    {
        pos += step;

        if (node->left)
        {
            j_l = abs((node->left[pos >> 8] >> 12));

            if (j_l > 15)
                j_l = 15;
            l = qMax(l, j_l);
        }
        if (node->right)
        {
            j_r = abs((node->right[pos >> 8] >> 12));
            if (j_r > 15)
                j_r = 15;
            r = qMax(r, j_r);
        }
    }
    m_l -= 0.5;
    m_l = m_l > l ? m_l : l;
    m_r -= 0.5;
    m_r = m_r > r ? m_r : r;
}

void ShadedVisual::draw (QPainter *p)
{
    for (int i = 0; i < m_l; ++i)
    {
        p->fillRect (i*3*m_ratio, 0, 3*m_ratio, 2*m_ratio, QBrush(m_skin->getVisColor (17-i)));
    }
    for (int i = 0; i < m_r; ++i)
    {
        p->fillRect (i*3*m_ratio, 3*m_ratio, 3*m_ratio, 2*m_ratio, QBrush(m_skin->getVisColor (17-i)));
    }
}

void ShadedVisual::paintEvent (QPaintEvent *)
{
    QPainter painter (this);
    painter.drawPixmap (0,0,m_pixmap);
}

void ShadedVisual::hideEvent (QHideEvent *)
{
    m_timer->stop();
}

void ShadedVisual::showEvent (QShowEvent *)
{
    m_timer->start();
}

void ShadedVisual::updateSkin()
{
    m_ratio = m_skin->ratio();
    resize(m_ratio*38,m_ratio*5);
    m_pixmap = QPixmap (m_ratio*38,m_ratio*5);
    clear();
}
