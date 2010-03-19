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
#include <qmmp/qmmp.h>
#include <math.h>
#include <stdlib.h>
#include "skin.h"
#include "fft.h"
#include "inlines.h"
#include "mainvisual.h"


MainVisual *MainVisual::m_instance = 0;

MainVisual *MainVisual::instance()
{
    if (!m_instance)
        qFatal ("MainVisual: this object is not created!");
    return m_instance;
}

MainVisual::MainVisual (QWidget *parent)
        : Visual (parent), m_vis (0), m_playing (false)
{
    m_skin = Skin::instance();
    m_ratio = m_skin->ratio();
    connect(m_skin, SIGNAL(skinChanged()), this, SLOT(updateSettings()));
    m_timer = new QTimer (this);
    connect(m_timer, SIGNAL (timeout()), this, SLOT (timeout()));
    m_nodes.clear();
    createMenu();
    readSettings();
    m_instance = this;
}

MainVisual::~MainVisual()
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    if (m_vis)
    {
        settings.setValue("Visualization/type",m_vis->name());
        delete m_vis;
        m_vis = 0;
    }
    else
        settings.setValue("Visualization/type", "None");
    settings.setValue("Visualization/rate", 1000/m_timer->interval());
    while (!m_nodes.isEmpty())
        delete m_nodes.takeFirst();
    m_instance = 0;
}

void MainVisual::setVisual (VisualBase *newvis)
{
    m_timer->stop();
    if (m_vis)
        delete m_vis;
    m_vis = newvis;
    if (m_vis)
        m_timer->start();
    else
    {
        m_pixmap.fill (Qt::transparent);
        update();
    }
}

void MainVisual::clear()
{
    while (!m_nodes.isEmpty())
        delete m_nodes.takeFirst();
    if (m_vis)
        m_vis->clear();
    m_pixmap = m_bg;
    update();
}

void MainVisual::add (unsigned char *data, qint64 size, int chan)
{
    if (!m_timer->isActive () || !m_vis)
        return;

    short *l = 0, *r = 0;
    qint64 samples = size/chan >> 1;
    int frames = samples/512;
    for (int i = 0; i < frames; ++i)
    {
        l = new short[512];
        r = 0;
        if (chan == 2)
        {
            r = new short[512];
            stereo16_from_stereopcm16 (l, r, (short *) (data + i*4*512), 512);
        }
        else if (chan == 1)
            mono16_from_monopcm16 (l, (short *) (data + i*2*512), 512);
        else
        {
            r = new short[512];
            stereo16_from_multichannel(l, r, (short *) (data + i*2*chan*512), 512, chan);
        }
        m_nodes.append (new VisualNode (l, r, 512));
    }
}

void MainVisual::timeout()
{
    VisualNode *node = 0;
    mutex()->lock ();

    while(m_nodes.size() > 5)
    {
        delete m_nodes.takeFirst();
    }

    if(!m_nodes.isEmpty())
        node = m_nodes.takeFirst();

    mutex()->unlock();

    if (m_vis && node)
    {
        m_vis->process (node);
        m_pixmap = m_bg;
        QPainter p(&m_pixmap);
        m_vis->draw (&p);
        delete node;
    }
    update();
}

void MainVisual::paintEvent (QPaintEvent *)
{
    QPainter painter (this);
    painter.drawPixmap (0,0, m_pixmap);
}

void MainVisual::hideEvent (QHideEvent *)
{
    m_timer->stop();
}

void MainVisual::showEvent (QShowEvent *)
{
    if (m_vis)
        m_timer->start();
}

void MainVisual::mousePressEvent (QMouseEvent *e)
{
    if (e->button() == Qt::RightButton)
        m_menu->exec(e->globalPos());
    else
    {
        m_pixmap = m_bg;
        if (!m_vis)
            setVisual(new mainvisual::Analyzer);
        else if (m_vis->name() == "Analyzer")
            setVisual(new mainvisual::Scope);
        else if (m_vis->name() == "Scope")
            setVisual(0);

        QString str = "Off";
        if (m_vis)
            str = m_vis->name();
        foreach(QAction *act, m_visModeGroup->actions ())
        if (str == act->data().toString())
        {
            act->setChecked(true);
            break;
        }
    }
}

void MainVisual::drawBackGround()
{
    m_bg = QPixmap (76 * m_ratio, 16 * m_ratio);
    if (m_transparentAction->isChecked())
    {
        m_bg.fill (Qt::transparent);
        return;
    }
    QPainter painter(&m_bg);
    for (int x = 0; x < 76 * m_ratio; x += 2)
    {
        painter.setPen(m_skin->getVisColor(0));
        painter.drawLine(x + 1, 0, x + 1, 16 *m_ratio);
        for (int y = 0; y < 16 *m_ratio; y += 2)
        {
            painter.setPen(m_skin->getVisColor(0));
            painter.drawPoint(x,y);
            painter.setPen(m_skin->getVisColor(1));
            painter.drawPoint(x,y + 1);
        }
    }
}

void MainVisual::updateSettings()
{
    m_ratio = m_skin->ratio();
    resize(76 * m_ratio, 16 * m_ratio);
    m_pixmap = QPixmap (76 * m_ratio, 16 * m_ratio);
    drawBackGround();
    m_pixmap = m_bg;
    update();
    QAction *act = m_fpsGroup->checkedAction ();
    if (act)
        m_timer->setInterval (1000/act->data().toInt());
    else
        m_timer->setInterval (40);

    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    act = m_peaksFalloffGroup->checkedAction ();
    if (act)
        settings.setValue("Visualization/peaks_falloff", act->data().toInt());
    else
        settings.setValue("Visualization/peaks_falloff", 3);

    act = m_analyzerFalloffGroup->checkedAction ();
    if (act)
        settings.setValue("Visualization/analyzer_falloff", act->data().toInt());
    else
        settings.setValue("Visualization/analyzer_falloff", 3);

    settings.setValue("Visualization/show_peaks", m_peaksAction->isChecked());

    act = m_analyzerModeGroup->checkedAction();
    if (act)
        settings.setValue("Visualization/analyzer_mode", act->data().toInt());
    else
        settings.setValue("Visualization/analyzer_mode", 0);

    act = m_analyzerTypeGroup->checkedAction();
    if (act)
        settings.setValue("Visualization/analyzer_type", act->data().toInt());
    else
        settings.setValue("Visualization/analyzer_type", 1);

    settings.setValue("Visualization/transparent_bg", m_transparentAction->isChecked());

    act = m_visModeGroup->checkedAction ();
    QString visName;
    if (act)
        visName = act->data().toString();
    else
        visName == "Off";

    if (visName == "Analyzer")
        setVisual(new mainvisual::Analyzer);
    else if (visName == "Scope")
        setVisual(new mainvisual::Scope);
    else
        setVisual(0);

}

void MainVisual::createMenu()
{
    m_menu = new QMenu (this);
    connect(m_menu, SIGNAL(triggered (QAction *)),SLOT(updateSettings()));
    QMenu *visMode = m_menu->addMenu(tr("Visualization Mode"));
    m_visModeGroup = new QActionGroup(this);
    m_visModeGroup->setExclusive(true);
    m_visModeGroup->addAction(tr("Analyzer"))->setData("Analyzer");
    m_visModeGroup->addAction(tr("Scope"))->setData("Scope");
    m_visModeGroup->addAction(tr("Off"))->setData("None");
    foreach(QAction *act, m_visModeGroup->actions ())
    {
        act->setCheckable(true);
        visMode->addAction(act);
    }

    QMenu *analyzerMode = m_menu->addMenu(tr("Analyzer Mode"));
    m_analyzerModeGroup = new QActionGroup(this);
    m_analyzerTypeGroup = new QActionGroup(this);
    m_analyzerModeGroup->addAction(tr("Normal"))->setData(0);
    m_analyzerModeGroup->addAction(tr("Fire"))->setData(1);
    m_analyzerModeGroup->addAction(tr("Vertical Lines"))->setData(2);
    m_analyzerTypeGroup->addAction(tr("Lines"))->setData(0);
    m_analyzerTypeGroup->addAction(tr("Bars"))->setData(1);
    foreach(QAction *act, m_analyzerModeGroup->actions ())
    {
        act->setCheckable(true);
        analyzerMode->addAction(act);
    }
    analyzerMode->addSeparator ();
    foreach(QAction *act, m_analyzerTypeGroup->actions ())
    {
        act->setCheckable(true);
        analyzerMode->addAction(act);
    }
    analyzerMode->addSeparator ();
    m_peaksAction = analyzerMode->addAction(tr("Peaks"));
    m_peaksAction->setCheckable(true);


    QMenu *refreshRate = m_menu->addMenu(tr("Refresh Rate"));
    m_fpsGroup = new QActionGroup(this);
    m_fpsGroup->setExclusive(true);
    m_fpsGroup->addAction(tr("50 fps"))->setData(50);
    m_fpsGroup->addAction(tr("25 fps"))->setData(25);
    m_fpsGroup->addAction(tr("10 fps"))->setData(10);
    m_fpsGroup->addAction(tr("5 fps"))->setData(5);
    foreach(QAction *act, m_fpsGroup->actions ())
    {
        act->setCheckable(true);
        refreshRate->addAction(act);
    }

    QMenu *analyzerFalloff = m_menu->addMenu(tr("Analyzer Falloff"));
    m_analyzerFalloffGroup = new QActionGroup(this);
    m_analyzerFalloffGroup->setExclusive(true);
    m_analyzerFalloffGroup->addAction(tr("Slowest"))->setData(1);
    m_analyzerFalloffGroup->addAction(tr("Slow"))->setData(2);
    m_analyzerFalloffGroup->addAction(tr("Medium"))->setData(3);
    m_analyzerFalloffGroup->addAction(tr("Fast"))->setData(4);
    m_analyzerFalloffGroup->addAction(tr("Fastest"))->setData(5);
    foreach(QAction *act, m_analyzerFalloffGroup->actions ())
    {
        act->setCheckable(true);
        analyzerFalloff->addAction(act);
    }

    QMenu *peaksFalloff = m_menu->addMenu(tr("Peaks Falloff"));
    m_peaksFalloffGroup = new QActionGroup(this);
    m_peaksFalloffGroup->setExclusive(true);
    m_peaksFalloffGroup->addAction(tr("Slowest"))->setData(1);
    m_peaksFalloffGroup->addAction(tr("Slow"))->setData(2);
    m_peaksFalloffGroup->addAction(tr("Medium"))->setData(3);
    m_peaksFalloffGroup->addAction(tr("Fast"))->setData(4);
    m_peaksFalloffGroup->addAction(tr("Fastest"))->setData(5);
    foreach(QAction *act, m_peaksFalloffGroup->actions ())
    {
        act->setCheckable(true);
        peaksFalloff->addAction(act);
    }
    QMenu *background = m_menu->addMenu(tr("Background"));
    m_transparentAction = background->addAction(tr("Transparent"));
    m_transparentAction->setCheckable(true);
    update();
}


void MainVisual::readSettings()
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);

    QString name = settings.value("Visualization/type","Analyzer").toString();
    m_visModeGroup->actions ()[0]->setChecked(true);
    foreach(QAction *act, m_visModeGroup->actions ())
    if (name == act->data().toString())
        act->setChecked(true);

    m_peaksAction->setChecked(
        settings.value("Visualization/show_peaks", true).toBool());

    int fps = settings.value("Visualization/rate", 25).toInt();
    m_fpsGroup->actions ()[1]->setChecked(true);
    foreach(QAction *act, m_fpsGroup->actions ())
    if (fps == act->data().toInt())
        act->setChecked(true);

    int mode = settings.value("Visualization/analyzer_mode", 0).toInt();
    m_analyzerModeGroup->actions ()[0]->setChecked(true);
    foreach(QAction *act, m_analyzerModeGroup->actions ())
    if (mode == act->data().toInt())
        act->setChecked(true);

    int type = settings.value("Visualization/analyzer_type", 1).toInt();
    m_analyzerTypeGroup->actions ()[1]->setChecked(true);
    foreach(QAction *act, m_analyzerTypeGroup->actions ())
    if (type == act->data().toInt())
        act->setChecked(true);

    int speed = settings.value("Visualization/peaks_falloff", 3).toInt();
    m_peaksFalloffGroup->actions ()[2]->setChecked(true);
    foreach(QAction *act, m_peaksFalloffGroup->actions ())
    if (speed == act->data().toInt())
        act->setChecked(true);

    speed = settings.value("Visualization/analyzer_falloff", 3).toInt();
    m_analyzerFalloffGroup->actions ()[2]->setChecked(true);
    foreach(QAction *act, m_analyzerFalloffGroup->actions ())
    if (speed == act->data().toInt())
        act->setChecked(true);

    m_transparentAction->setChecked(
        settings.value("Visualization/transparent_bg", false).toBool());

    updateSettings();
}

using namespace mainvisual;

Analyzer::Analyzer()
        : m_analyzerBarWidth (4), m_fps (20)
{
    clear();
    m_skin = Skin::instance();
    m_size = QSize(76*m_skin->ratio(), 16*m_skin->ratio());

    double peaks_speed[] = { 0.05, 0.1, 0.2, 0.4, 0.8 };
    double analyzer_speed[] = { 1.2, 1.8, 2.2, 2.8, 2.4 };

    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    m_peaks_falloff =
        peaks_speed[settings.value("Visualization/peaks_falloff", 3).toInt()-1];
    m_analyzer_falloff =
        analyzer_speed[settings.value("Visualization/analyzer_falloff", 3).toInt()-1];
    m_show_peaks = settings.value("Visualization/show_peaks", true).toBool();

    m_lines = settings.value("Visualization/analyzer_type", 1).toInt() == 0;
    m_mode = settings.value("Visualization/analyzer_mode", 0).toInt();
}

Analyzer::~Analyzer()
{}

void Analyzer::clear()
{
    for (int i = 0; i< 75; ++i)
    {
        m_intern_vis_data[i] = 0;
        m_peaks[i] = 0;
    }
}

bool Analyzer::process (VisualNode *node)
{
    static fft_state *state = 0;
    if (!state)
        state = fft_init();
    short dest[256];

    const int xscale_long[] =
    {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
        19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34,
        35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51,
        52, 53, 54, 55, 56, 57, 58, 61, 66, 71, 76, 81, 87, 93, 100, 107,
        114, 122, 131, 140, 150, 161, 172, 184, 255
    };

    const int xscale_short[] =
    {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 11, 15, 20, 27,
        36, 47, 62, 82, 107, 141, 184, 255
    };

    if (node)
    {
        //i = node->length;
        calc_freq (dest, node->left);
    }
    else
        return false;
    const double y_scale = 3.60673760222;   /* 20.0 / log(256) */
    int max = m_lines ? 75 : 19, y, j;

    for (int i = 0; i < max; i++)
    {
        if (m_lines)
            for (j = xscale_long[i], y = 0; j < xscale_long[i + 1]; j++)
            {
                if (dest[j] > y)
                    y = dest[j];
            }
        else
            for (j = xscale_short[i], y = 0; j < xscale_short[i + 1]; j++)
            {
                if (dest[j] > y)
                    y = dest[j];
            }
        y >>= 7;
        int magnitude = 0;
        if (y != 0)
        {
            magnitude = int(log (y) * y_scale);
            if (magnitude > 15)
                magnitude = 15;
            if (magnitude < 0)
                magnitude = 0;
        }

        m_intern_vis_data[i] -= m_analyzer_falloff;
        m_intern_vis_data[i] = magnitude > m_intern_vis_data[i]
                               ? magnitude : m_intern_vis_data[i];
        if (m_show_peaks)
        {
            m_peaks[i] -= m_peaks_falloff;
            m_peaks[i] = magnitude > m_peaks[i]
                         ? magnitude : m_peaks[i];
        }
    }
    return true;
}

void Analyzer::draw (QPainter *p)
{
    int r = m_skin->ratio();
    if (m_lines)
        for (int j = 0; j < 75; ++j)
        {
            for (int i = 0; i <= m_intern_vis_data[j]; ++i)
            {
                if (m_mode == 0)
                    p->setPen (m_skin->getVisColor (18-i));
                else if (m_mode == 1)
                    p->setPen (m_skin->getVisColor (3+(int(m_intern_vis_data[j])-i)));
                else
                    p->setPen (m_skin->getVisColor (18-int(m_intern_vis_data[j])));
                p->drawPoint (j*r, m_size.height() - r*i);
                if(r == 2)
                    p->drawPoint (j*r+1, m_size.height() - r*i);
            }
            p->setPen (m_skin->getVisColor (23));
            if (m_show_peaks)
            {
                p->drawPoint (j*r, m_size.height() - r*m_peaks[j]);
                if(r == 2)
                    p->drawPoint (j*r+1, m_size.height() - r*m_peaks[j]);
            }
        }
    else
        for (int j = 0; j < 19; ++j)
        {
            for (int i = 0; i <= m_intern_vis_data[j]; ++i)
            {
                if (m_mode == 0)
                    p->setPen (m_skin->getVisColor (18-i));
                else if (m_mode == 1)
                    p->setPen (m_skin->getVisColor (3+(int(m_intern_vis_data[j])-i)));
                else
                    p->setPen (m_skin->getVisColor (18-int(m_intern_vis_data[j])));

                p->drawLine (j*4*r,m_size.height()-r*i, (j*4+2)*r,m_size.height()-r*i);
                if(r == 2)
                    p->drawLine (j*4*r, m_size.height()-r*i +1, (j*4+2)*r,m_size.height()-r*i+1);
            }
            p->setPen (m_skin->getVisColor (23));
            if (m_show_peaks)
            {
                p->drawLine (j*4*r,m_size.height()-r*m_peaks[j],
                             (j*4+2)*r,m_size.height()-r*m_peaks[j]);
                if(r == 2)
                    p->drawLine (j*4*r,m_size.height()-r*m_peaks[j]+1,
                             (j*4+2)*r,m_size.height()-r*m_peaks[j]+1);
            }
        }
}

Scope::Scope()
{
    clear();
    m_skin = Skin::instance();
    m_ratio = m_skin->ratio();
}

void Scope::clear()
{
    for (int i = 0; i< 76; ++i)
        m_intern_vis_data[i] = 5;
}

Scope::~Scope()
{}

bool Scope::process(VisualNode *node)
{
    if (!node)
        return false;

    int step = (node->length << 8)/76;
    int pos = 0;

    for (int i = 0; i < 76; ++i)
    {
        pos += step;
        m_intern_vis_data[i] = (node->left[pos >> 8] >> 12);

        if (m_intern_vis_data[i] > 4)
            m_intern_vis_data[i] = 4;
        else if (m_intern_vis_data[i] < -4)
            m_intern_vis_data[i] = -4;
    }
    return true;
}

void Scope::draw(QPainter *p)
{
    for (int i = 0; i<75; ++i)
    {
        int h1 = 8 - m_intern_vis_data[i];
        int h2 = 8 - m_intern_vis_data[i+1];
        if (h1 > h2)
            qSwap(h1, h2);
        p->setPen (m_skin->getVisColor(18 + qAbs(8 - h2)));
        p->drawLine(i*m_ratio, h1*m_ratio, (i+1)*m_ratio, h2*m_ratio);
    }
    for (int i = 0; i< 76; ++i)
        m_intern_vis_data[i] = 0;
}
