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
#include <QDir>
#include <QKeyEvent>

#include "projectmwidget.h"

ProjectMWidget::ProjectMWidget(QWidget *parent)
        : QGLWidget(parent)
{
    setMouseTracking(TRUE);
    m_projectM = 0;
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()),SLOT(updateGL ()));
    timer->start(0);
}


ProjectMWidget::~ProjectMWidget()
{
    if (m_projectM)
        delete m_projectM;
    m_projectM = 0;
}

QProjectM *ProjectMWidget::projectM()
{
    return m_projectM;
}

void ProjectMWidget::initializeGL()
{
    glShadeModel(GL_SMOOTH);
    glClearColor(0,0,0,0);
    // Setup our viewport
    glViewport(0, 0, width(), height());
    // Change to the projection matrix and set our viewing volume.
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glDrawBuffer(GL_BACK);
    glReadBuffer(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POINT_SMOOTH);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glLineStipple(2, 0xAAAA);
    if (!m_projectM)
    {
        m_projectM = new QProjectM("/usr/share/projectM/config.inp");


        QString file(m_projectM->settings().presetURL.c_str());

        foreach (QFileInfo info, QDir(file).entryInfoList())
        {
            if (info.fileName().toLower().endsWith(".prjm") || info.fileName().toLower().endsWith(".milk"))
                m_projectM->addPresetURL ( info.absoluteFilePath().toStdString(), info.fileName().toStdString(), 1);
        }
    }
}

void ProjectMWidget::resizeGL (int w, int h)
{
    if (m_projectM)
    {
        m_projectM->projectM_resetGL(w, h);
        initializeGL();
    }
}

void ProjectMWidget::paintGL()
{
    if (m_projectM)
        m_projectM->renderFrame();
}
