/***************************************************************************
 *   Copyright (C) 2008 by Ilya Kotov                                      *
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

#include <qmmp/constants.h>
#include <qmmp/buffer.h>
#include <qmmp/output.h>
#include <qmmp/recycler.h>
#include <qmmp/fileinfo.h>
#include <qmmp/decoderfactory.h>

#include <QObject>
#include <QBasicTimer>
#include <QFile>
#include <QApplication>
#include <QAction>
#include <QKeyEvent>
#include <QMenu>

#include "decoder_phonon.h"


VideoWindow::VideoWindow(DecoderPhonon *decoder, QWidget *parent) :
        Phonon::VideoWidget(parent)
{
    m_decoder = decoder;
    //create menu
    m_menu = new QMenu (this);
    //actions
    m_fullScreenAction = new QAction(tr("Full Screen"), this);
    m_fullScreenAction->setShortcut(tr("Alt+Return"));
    m_fullScreenAction->setCheckable(TRUE);
    m_fullScreenAction->setChecked(FALSE);
    m_fullScreenAction->setShortcutContext(Qt::WindowShortcut);
    connect(m_fullScreenAction, SIGNAL(triggered(bool)), SLOT(changeScreen(bool)));
    m_menu->addAction(m_fullScreenAction);
    addAction(m_fullScreenAction);
    setMouseTracking (FALSE);
}

void VideoWindow::mouseDoubleClickEvent(QMouseEvent *e)
{
    changeScreen(!isFullScreen());
    Phonon::VideoWidget::mouseDoubleClickEvent(e);
}

void VideoWindow::closeEvent (QCloseEvent *event)
{
    if (event->spontaneous())
        m_decoder->stop();
    Phonon::VideoWidget::closeEvent(event);
}

void VideoWindow::mousePressEvent(QMouseEvent *event)
{
    switch ((int) event->button ())
    {
    case Qt::LeftButton:
        m_pos = event->pos();
        break;
    case Qt::RightButton:
        m_menu->exec(event->globalPos());
        break;
    }
    Phonon::VideoWidget::mousePressEvent(event);
}

void VideoWindow::mouseReleaseEvent(QMouseEvent *event)
{
    Phonon::VideoWidget::mouseReleaseEvent(event);
}

void VideoWindow::mouseMoveEvent(QMouseEvent *event)
{
    QPoint npos = (event->globalPos() - m_pos + (frameGeometry ().topLeft() - geometry().topLeft()));
    move(npos);
    Phonon::VideoWidget::mouseMoveEvent(event);
}


void VideoWindow::changeScreen(bool fullScreen)
{
    m_fullScreenAction->setChecked(fullScreen);
    setFullScreen(fullScreen);
    if (!fullScreen)
        resize(sizeHint());
}


DecoderPhonon::DecoderPhonon(QObject *parent, DecoderFactory *d, const QString &url)
        : Decoder(parent, d)
{
    m_mediaObject = 0;
    m_audioOutput = 0;
    m_videoWidget = 0;
    m_url = url;
}

DecoderPhonon::~DecoderPhonon()
{
    qDebug("DecoderPhonon::~DecoderPhonon");
    m_videoWidget->hide();
    delete m_videoWidget;
}

bool DecoderPhonon::initialize()
{
    m_videoWidget = new VideoWindow(this);//Phonon::VideoWidget();
    m_videoWidget->setAspectRatio(Phonon::VideoWidget::AspectRatioAuto);
    m_videoWidget->resize(300,200);
    m_videoWidget->setWindowTitle("Phonon Plugin");
    m_mediaObject = new Phonon::MediaObject(this);
    m_audioOutput = new Phonon::AudioOutput(this);
    m_audioOutputPath = Phonon::createPath(m_mediaObject, m_audioOutput);
    Phonon::createPath(m_mediaObject, m_videoWidget);
    connect(m_mediaObject, SIGNAL(stateChanged(Phonon::State, Phonon::State)), SLOT(updateState(Phonon::State, Phonon::State)));
    connect(m_mediaObject, SIGNAL(tick (qint64)), SLOT(updateTime(qint64)));
    connect(m_mediaObject, SIGNAL(finished ()), SLOT(finish ()));
    m_mediaObject->setTickInterval(1000);
    m_mediaObject->clearQueue();
    m_mediaObject->enqueue(Phonon::MediaSource(m_url));
    m_stop = FALSE;
    return TRUE;
}

qint64 DecoderPhonon::lengthInSeconds()
{
    return m_mediaObject->totalTime () / 1000;
}

void DecoderPhonon::seek(qint64 pos)
{
    m_mediaObject->seek(pos * 1000);
}

void DecoderPhonon::stop()
{
    m_mediaObject->stop();
    m_videoWidget->close();
    while (m_mediaObject->state() == Phonon::PlayingState)
    {
        qApp->processEvents();
        usleep(100);
    }
    StateHandler::instance()->dispatch(Qmmp::Stopped);
}

void DecoderPhonon::pause()
{
    if (m_mediaObject->state() == Phonon::PausedState)
        m_mediaObject->play();
    else if (m_mediaObject->state() == Phonon::PlayingState)
        m_mediaObject->pause();
}

void DecoderPhonon::setEQ(double bands[10], double preamp)
{
}

void DecoderPhonon::setEQEnabled(bool on)
{
}

void DecoderPhonon::run()
{
    if (!m_stop)
        m_mediaObject->play();
    else
    {
        //m_mediaObject.stop();
        while (m_mediaObject->state() == Phonon::PlayingState)
        {
            qApp->processEvents();
            usleep(500);
        }
    }
}

void DecoderPhonon::updateState(Phonon::State newstate, Phonon::State)
{
    switch ((int) newstate)
    {
    case Phonon::PlayingState:
        StateHandler::instance()->dispatch(Qmmp::Playing);
        m_videoWidget->resize(m_videoWidget->sizeHint());
        m_videoWidget->show();
        break;
    case Phonon::PausedState:
        StateHandler::instance()->dispatch(Qmmp::Paused);
        break;
    case Phonon::StoppedState:
        //StateHandler::instance()->dispatch(Qmmp::Stopped);
        //m_videoWidget->close();
        qApp->processEvents();
        break;
    }
}

void DecoderPhonon::updateTime(qint64 time)
{
    StateHandler::instance()->dispatch(time / 1000, m_mediaObject->totalTime () / 1000, 0, 0, 0, 0);
}

