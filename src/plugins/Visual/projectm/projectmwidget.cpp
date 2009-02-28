/***************************************************************************
 *   Copyright (C) 2009 by Ilya Kotov                                      *
 *   forkotov02@hotmail.ru                                                 *
 *                                                                         *
 *   Copyright (C) 2007 by  projectM team                                  *
 *                                                                         *
 *   Carmelo Piccione and carmelo.piccione+projectM@gmail.com              *
 *   carmelo.piccione+projectM@gmail.com                                   *
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
#include <QMenu>
#include <libprojectM/projectM.hpp>
#include <qmmp/soundcore.h>
#include <qmmp/qmmp.h>
#include "projectmwidget.h"

#ifndef PROJECTM_CONFIG
#define PROJECTM_CONFIG "/usr/share/projectM/config.inp"
#endif

ProjectMWidget::ProjectMWidget(QWidget *parent)
        : QGLWidget(parent)
{
    setMouseTracking(TRUE);
    m_projectM = 0;
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()),SLOT(updateGL ()));
    timer->start(0);
    m_menu = new QMenu(this);
    connect(SoundCore::instance(), SIGNAL(metaDataChanged()), SLOT(updateTitle()));
}


ProjectMWidget::~ProjectMWidget()
{
    if (m_projectM)
        delete m_projectM;
    m_projectM = 0;
}

projectM *ProjectMWidget::projectMInstance()
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
        m_projectM = new projectM(PROJECTM_CONFIG, projectM::FLAG_DISABLE_PLAYLIST_LOAD);
        QString presetPath (m_projectM->settings().presetURL.c_str());
        QDir presetDir(presetPath);
        presetDir.setFilter(QDir::Files);
        QStringList filters;
        filters << "*.prjm" << "*.milk";
        QFileInfoList l = presetDir.entryInfoList(filters);
        foreach (QFileInfo info, l)
        {
            m_projectM->addPresetURL (info.absoluteFilePath().toStdString(), info.fileName().toStdString(), 1);
        }
        createActions();
        updateTitle();
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

void ProjectMWidget::mousePressEvent (QMouseEvent *event)
{
    if (event->button () == Qt::RightButton)
        m_menu->exec(event->globalPos());
}

void ProjectMWidget::createActions()
{
    m_menu->addAction(tr("&Help"), this, SLOT(showHelp()), tr("F1"));
    m_menu->addAction(tr("&Show Song Title"), this, SLOT(showTitle()), tr("F2"));
    m_menu->addAction(tr("&Show Preset Name"), this, SLOT(showPresetName()), tr("F3"));
    m_menu->addSeparator();
    m_menu->addAction(tr("&Next Preset"), this, SLOT(nextPreset()), tr("N"));
    m_menu->addAction(tr("&Previous Preset"), this, SLOT(previousPreset()), tr("P"));
    m_menu->addAction(tr("&Random Preset"), this, SLOT(randomPreset()), tr("R"));
    m_menu->addAction(tr("&Lock Preset"), this, SLOT(lockPreset()), tr("L"));
    m_menu->addSeparator();
    m_menu->addAction(tr("&Fullscreen"), this, SLOT(fullScreen()), tr("F"));
    m_menu->addSeparator();
    parentWidget()->addActions(m_menu->actions());
}

void ProjectMWidget::showHelp()
{
    m_projectM->key_handler(PROJECTM_KEYDOWN, PROJECTM_K_F1, PROJECTM_KMOD_LSHIFT);
}

void ProjectMWidget::showPresetName()
{
    m_projectM->key_handler(PROJECTM_KEYDOWN, PROJECTM_K_F3, PROJECTM_KMOD_LSHIFT);
}

void ProjectMWidget::showTitle()
{
    m_projectM->key_handler(PROJECTM_KEYDOWN, PROJECTM_K_F2, PROJECTM_KMOD_LSHIFT);
}

void ProjectMWidget::nextPreset()
{
    m_projectM->key_handler(PROJECTM_KEYDOWN, PROJECTM_K_n, PROJECTM_KMOD_LSHIFT);
}

void ProjectMWidget::previousPreset()
{
    m_projectM->key_handler(PROJECTM_KEYDOWN, PROJECTM_K_p, PROJECTM_KMOD_LSHIFT);
}

void ProjectMWidget::randomPreset()
{
    m_projectM->key_handler(PROJECTM_KEYDOWN, PROJECTM_K_r, PROJECTM_KMOD_LSHIFT);
}

void ProjectMWidget::lockPreset()
{
    m_projectM->key_handler(PROJECTM_KEYDOWN, PROJECTM_K_l, PROJECTM_KMOD_LSHIFT);
}

void ProjectMWidget::fullScreen()
{
    parentWidget()->setWindowState (parentWidget()->windowState() ^Qt::WindowFullScreen);
}

void ProjectMWidget::updateTitle()
{
    std::string artist = SoundCore::instance()->metaData(Qmmp::ARTIST).toLocal8Bit().constData();
    std::string title = SoundCore::instance()->metaData(Qmmp::TITLE).toLocal8Bit().constData();
    m_projectM->projectM_setTitle(artist + " - " + title);
}
