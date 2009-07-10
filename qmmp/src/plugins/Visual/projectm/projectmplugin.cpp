/***************************************************************************
 *   Copyright (C) 2009 by Ilya Kotov                                      *
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
#include <math.h>
#include <stdlib.h>
#include <locale.h>
#include <libprojectM/projectM.hpp>

#include <qmmp/buffer.h>
#include <qmmp/output.h>
#include "projectmwidget.h"
#include "projectmplugin.h"

ProjectMPlugin::ProjectMPlugin (QWidget *parent)
        : Visual (parent)
{
    setlocale(LC_NUMERIC, "C"); //fixes none-english locales problem
    setWindowTitle(tr("ProjectM"));
    m_projectMWidget = new ProjectMWidget(this);
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(m_projectMWidget);
    layout->setContentsMargins(0,0,0,0);
    setLayout(layout);
    resize(300,300); //TODO save/load geometry
}

ProjectMPlugin::~ProjectMPlugin()
{}

void ProjectMPlugin::clear()
{
    update();
}

void ProjectMPlugin::add ( Buffer *b, unsigned long, int, int)
{ //TODO 8 bit support
    if (m_projectMWidget->projectMInstance())
        m_projectMWidget->projectMInstance()->pcm()->addPCM16Data((short *)b->data, b->nbytes/4);
}
