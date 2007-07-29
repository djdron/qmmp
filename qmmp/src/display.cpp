/***************************************************************************
 *   Copyright (C) 2006 by Ilya Kotov                                      *
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
#include <QCoreApplication>
#include <QPainter>
#include <QPushButton>
#include <QLabel>
#include <QSettings>
#include <QMenu>

#include <output.h>
#include "skin.h"
#include "mainvisual.h"
#include "button.h"
#include "titlebar.h"
#include "positionbar.h"
#include "number.h"
#include "togglebutton.h"
#include "symboldisplay.h"
#include "textscroller.h"
#include "monostereo.h"
#include "playstatus.h"
#include "volumebar.h"
#include "balancebar.h"
#include "mainwindow.h"
#include "timeindicator.h"

#include "display.h"

MainDisplay::MainDisplay ( QWidget *parent )
        : PixmapWidget ( parent )
{
    m_skin = Skin::getPointer();
    setPixmap ( m_skin->getMain() );
    setMaximumSize ( QSize ( 275,116 ) );
    setMinimumSize ( QSize ( 275,116 ) );

    m_mw = qobject_cast<MainWindow*>(parent);

    Button *previous = new Button ( this,
                                    Skin::BT_PREVIOUS_N, Skin::BT_PREVIOUS_P );
    previous->move ( 16, 88 );
    connect ( previous,SIGNAL ( clicked() ),parent,SLOT ( previous() ) );
    Button *play = new Button ( this,
                                Skin::BT_PLAY_N, Skin::BT_PLAY_P );
    play->move ( 39, 88 );
    connect ( play,SIGNAL ( clicked() ),parent,SLOT ( play() ) );
    Button *pause = new Button ( this, Skin::BT_PAUSE_N,Skin::BT_PAUSE_P );
    pause->move ( 62, 88 );
    connect ( pause,SIGNAL ( clicked() ),parent,SLOT ( pause() ) );
    Button *stop = new Button ( this, Skin::BT_STOP_N,Skin::BT_STOP_P );
    stop->move ( 85, 88 );
    connect ( stop,SIGNAL ( clicked() ),parent,SLOT ( stop() ) );
    connect ( stop,SIGNAL ( clicked() ),this,SLOT ( hideTimeDisplay() ) );
    Button *next = new Button ( this, Skin::BT_NEXT_N,Skin::BT_NEXT_P );
    next->move ( 108, 88 );
    connect ( next,SIGNAL ( clicked() ),parent,SLOT ( next() ) );
    Button *eject = new Button ( this, Skin::BT_EJECT_N,Skin::BT_EJECT_P );
    eject->move ( 136, 89 );
    connect ( eject,SIGNAL ( clicked() ),parent,SLOT ( addFile() ) );
    connect ( m_skin, SIGNAL ( skinChanged() ), this, SLOT ( updateSkin() ) );
    posbar = new PositionBar ( this );
    posbar->move ( 16,72 );
    //connect(posbar, SIGNAL(sliderMoved(int)), SLOT(setTime(int)));
    MainVisual* vis = new MainVisual ( this,"" );
    vis->setGeometry ( 24,39,75,20 );
    vis->show();

    m_eqButton = new ToggleButton ( this,Skin::BT_EQ_ON_N,Skin::BT_EQ_ON_P,
                                    Skin::BT_EQ_OFF_N,Skin::BT_EQ_OFF_P );
    m_eqButton->move ( 219,58 );
    m_eqButton->show();
    m_plButton = new ToggleButton ( this,Skin::BT_PL_ON_N,Skin::BT_PL_ON_P,
                                    Skin::BT_PL_OFF_N,Skin::BT_PL_OFF_P );
    m_plButton->move ( 241,58 );
    m_plButton->show();

    m_repeatButton = new ToggleButton ( this,Skin::REPEAT_ON_N,Skin::REPEAT_ON_P,
                                        Skin::REPEAT_OFF_N,Skin::REPEAT_OFF_P );
    connect(m_repeatButton,SIGNAL(clicked(bool)),this,SIGNAL(repeatableToggled(bool)));

    m_repeatButton->move ( 210,89 );
    m_repeatButton->show();

    m_shuffleButton = new ToggleButton ( this,Skin::SHUFFLE_ON_N,Skin::SHUFFLE_ON_P,
                                         Skin::SHUFFLE_OFF_N,Skin::SHUFFLE_OFF_P );
    connect(m_shuffleButton,SIGNAL(clicked(bool)),this,SIGNAL(shuffleToggled(bool)));
    m_shuffleButton->move ( 164,89 );
    m_shuffleButton->show();

    m_kbps = new SymbolDisplay( this,3 );
    m_kbps -> move ( 111,43 );
    m_kbps -> show();

    m_freq = new SymbolDisplay( this,2 );
    m_freq -> move ( 156,43 );
    m_freq -> show();

    TextScroller *m_text = new TextScroller ( this );
    m_text->resize ( 154,15 );
    m_text->move ( 109,23 );
    m_text->show();

    m_monoster = new MonoStereo ( this );
    m_monoster->move ( 212,41 );
    m_monoster->show();

    m_playstatus = new PlayStatus(this);
    m_playstatus->move(24,28);
    m_playstatus->show();

    m_volumeBar = new VolumeBar(this);
    connect(m_volumeBar, SIGNAL(sliderMoved(int)),SLOT(updateVolume()));
    m_volumeBar->move(107,57);
    m_volumeBar->show();

    m_balanceBar = new BalanceBar(this);
    connect(m_balanceBar, SIGNAL(sliderMoved(int)),SLOT(updateVolume()));
    m_balanceBar->move(177,57);
    m_balanceBar->show();
    m_timeIndicator = new TimeIndicator(this);
    m_timeIndicator->move(34,26);
    m_timeIndicator->show();
}


MainDisplay::~MainDisplay()
{
    QSettings settings ( QDir::homePath() +"/.qmmp/qmmprc", QSettings::IniFormat );
    settings.setValue ( "Playlist/visible",m_plButton->isChecked() );
    settings.setValue ( "Equalizer/visible",m_eqButton->isChecked() );
}

void MainDisplay::setTime ( int t )
{
    posbar->setValue ( t );
    m_timeIndicator->setTime(t);
}
void MainDisplay::setMaxTime ( long mt ) // TODO: should be removed
{
    posbar->setMax ( mt );
    m_timeIndicator->setSongDuration(mt);
}


void MainDisplay::updateSkin()
{
    setPixmap ( m_skin->getMain() );
}

void MainDisplay::setEQ ( QWidget* w )
{
    m_equlizer = w;
    m_eqButton->setON ( m_equlizer->isVisible() );
    connect ( m_eqButton, SIGNAL ( clicked ( bool ) ), m_equlizer, SLOT ( setVisible ( bool ) ) );
}

void MainDisplay::setPL ( QWidget* w )
{
    m_playlist = w;
    m_plButton->setON ( m_playlist->isVisible() );
    connect ( m_plButton, SIGNAL ( clicked ( bool ) ), m_playlist, SLOT ( setVisible ( bool ) ) );
}

void MainDisplay::setInfo(const OutputState &st)
{


    switch ( ( int ) st.type() )
    {
    case OutputState::Info:
        {
            //if ( seeking )
            // break;
            setTime ( st.elapsedSeconds() );
            m_kbps->display ( st.bitrate() );
            m_freq->display ( st.frequency() /1000 );
            m_monoster->setChannels ( st.channels() );
            update();
            break;
        }
    case OutputState::Playing:
        {
            m_playstatus->setStatus(PlayStatus::PLAY);
            m_timeIndicator->setNeedToShowTime(true);
            break;
        }
    case OutputState::Buffering:
        {
            //ui.label->setText("Buffering");
            break;
        }
    case OutputState::Paused:
        {
            m_playstatus->setStatus(PlayStatus::PAUSE);
            break;
        }
    case OutputState::Stopped:
        {
            m_playstatus->setStatus(PlayStatus::STOP);
            m_monoster->setChannels (0);
            //m_timeIndicator->setNeedToShowTime(false);
            break;
        }
    case OutputState::Volume:
        //qDebug("volume %d, %d", st.rightVolume(), st.leftVolume());
        int maxVol = qMax(st.leftVolume(),st.rightVolume());
        m_volumeBar->setValue(maxVol);
        if (maxVol && !m_volumeBar->isPressed())
            m_balanceBar->setValue((st.rightVolume()-st.leftVolume())*100/maxVol);
        break;

    }
}

bool MainDisplay::isPlaylistVisible() const
{
    return m_plButton->isChecked();
}

bool MainDisplay::isEqualizerVisible() const
{
    return m_eqButton->isChecked();
}

void MainDisplay::updateVolume()
{
    m_mw->setVolume(m_volumeBar->value(), m_balanceBar->value());
}

void MainDisplay::wheelEvent (QWheelEvent *e)
{
    m_mw->setVolume(m_volumeBar->value()+e->delta()/10, m_balanceBar->value());
}

bool MainDisplay::isRepeatable() const
{
    return m_repeatButton->isChecked();
}

bool MainDisplay::isShuffle() const
{
    return m_shuffleButton->isChecked();
}

void MainDisplay::setIsRepeatable(bool yes)
{
    m_repeatButton->setON(yes);
}

void MainDisplay::setIsShuffle(bool yes)
{
    m_shuffleButton->setON(yes);
}


void MainDisplay::hideTimeDisplay()
{
    m_timeIndicator->setNeedToShowTime(false);
}



void MainDisplay::mousePressEvent(QMouseEvent *e)
{
    if( e->button() == Qt::RightButton)
    {
        m_mw->menu()->exec(e->globalPos());
    }
    PixmapWidget::mousePressEvent(e);
}


