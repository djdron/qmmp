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
#ifndef DISPLAY_H
#define DISPLAY_H

#include <QPixmap>

class TimeIndicator;


#include "pixmapwidget.h"

/**
   @author Ilya Kotov <forkotov02@hotmail.ru>
*/
class QPushButton;
class QLabel;

class TitleBar;
class PositionBar;
class Number;
class Skin;
class ToggleButton;
class OutputState;
class NumberDisplay;
class SymbolDisplay;
class MonoStereo;
class PlayStatus;
class VolumeBar;
class BalanceBar;
class MainWindow;

class MainDisplay : public PixmapWidget
{
    Q_OBJECT
public:
    MainDisplay(QWidget *parent = 0);

    ~MainDisplay();

    void setMaxTime(long);
    void setEQ(QWidget*);
    void setPL(QWidget*);
    void setInfo(const OutputState &st);
    bool isEqualizerVisible()const;
    bool isPlaylistVisible()const;
    bool isRepeatable()const;
    bool isShuffle()const;
    void setIsRepeatable(bool);
    void setIsShuffle(bool);

public slots:
    void setTime(int);
    void hideTimeDisplay();
signals:
    void repeatableToggled(bool);
    void shuffleToggled(bool);
protected:
    void wheelEvent(QWheelEvent *);
    void mousePressEvent(QMouseEvent*);

private slots:
    void updateSkin();
    void updateVolume();

private:
    QWidget* m_equlizer;
    QWidget* m_playlist;
    QPixmap pixmap;
    QPushButton *button;
    QLabel *label;
    Skin *m_skin;
    TitleBar *titleBar;
    PositionBar *posbar;
    ToggleButton *m_eqButton;
    ToggleButton *m_plButton;
    ToggleButton *m_shuffleButton;
    ToggleButton *m_repeatButton;
    SymbolDisplay* m_kbps;
    SymbolDisplay* m_freq;
    MonoStereo* m_monoster;
    PlayStatus* m_playstatus;
    VolumeBar* m_volumeBar;
    BalanceBar* m_balanceBar;
    MainWindow* m_mw;
    TimeIndicator* m_timeIndicator;
};

#endif
