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
#include <QHBoxLayout>

#include <qmmp/buffer.h>
#include <qmmp/constants.h>
#include <qmmp/output.h>
#include <math.h>
#include <stdlib.h>
#include <locale.h>
#include <projectM.hpp>

#include "projectmwidget.h"
#include "analyzer.h"


Analyzer::Analyzer (QWidget *parent)
        : Visual (parent), m_fps ( 20 )
{
    setlocale(LC_NUMERIC, "C");
    m_projectMWidget = new ProjectMWidget(this);
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(m_projectMWidget);
    layout->setContentsMargins(0,0,0,0);
    setLayout(layout);
    resize(300,300);
}

Analyzer::~Analyzer()
{
    while (!m_nodes.isEmpty())
        m_nodes.removeFirst();
    //m_window->deleteLater();
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
    if (m_projectMWidget->projectM())
        m_projectMWidget->projectM()->pcm()->addPCM16Data((short *)b->data, b->nbytes/4);
}

void Analyzer::keyReleaseEvent (QKeyEvent *e)
{

    projectMKeycode pkey;
    bool ignore = FALSE;
    switch (e->key())
    {
    case Qt::Key_F:
    {
        setWindowState (windowState() ^Qt::WindowFullScreen);
        return;
    }
    case Qt::Key_M:
        pkey =  PROJECTM_K_M;
        break;
    case Qt::Key_F4:
        pkey =  PROJECTM_K_F4;
        break;
    case Qt::Key_F3:
        pkey =  PROJECTM_K_F3;
        break;
    case Qt::Key_F2:
        pkey =  PROJECTM_K_F2;
        break;
    case Qt::Key_F1:
        pkey =  PROJECTM_K_F1;
        break;
    case Qt::Key_R:
        pkey =  PROJECTM_K_r;
        break;
    case Qt::Key_L:
        pkey =  PROJECTM_K_l;
        ignore = true;
        break;
    case Qt::Key_N:
        pkey =  PROJECTM_K_n;
        break;
    case Qt::Key_P:
        pkey =  PROJECTM_K_p;
        break;
    case Qt::Key_F5:
        pkey =  PROJECTM_K_F5;
        break;
    default:
        e->ignore();
        return;
    }
    projectMModifier modifier;

    m_projectMWidget->projectM()->key_handler(PROJECTM_KEYDOWN, pkey, modifier);
    if (ignore)
        e->ignore();
}
