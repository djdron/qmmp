/***************************************************************************
 *   Copyright (C) 2006-2008 by Ilya Kotov                                 *
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
#ifndef EQWIDGET_H
#define EQWIDGET_H

#include "pixmapwidget.h"
#include <qmmp/output.h>

/**
   @author Ilya Kotov <forkotov02@hotmail.ru>
*/

class QMenu;
class Skin;
class EqTitleBar;
class EqSlider;
class ToggleButton;
class EQGraph;
class Button;
class EQPreset;
class PlayListItem;
class SoundCore;

class EqWidget : public PixmapWidget
{
    Q_OBJECT
public:
    EqWidget(QWidget *parent = 0);

    ~EqWidget();

    /*!
    * necessary for auto-load presets
    */
    void loadPreset(const QString &name);
    void setMimimalMode(bool b = true);

signals:
    void closed();

private slots:
    void updateSkin();
    void readEq();
    void writeEq();
    void showPresetsMenu();
    void reset();
    void showEditor();
    void savePreset();
    void saveAutoPreset();
    void setPreset(EQPreset*);
    void deletePreset(EQPreset*);
    void importWinampEQF();

private:
    void updatePositions();
    void readSettings();
    void writeSettings();
    void createActions();
    void updateMask();
    EQPreset *findPreset(const QString &name);
    //events
    virtual void keyPressEvent (QKeyEvent *);
    virtual void changeEvent(QEvent*);
    virtual void closeEvent(QCloseEvent*);
#ifdef Q_WS_X11
    virtual bool event (QEvent *event);
#endif

    Skin *m_skin;
    EqTitleBar *m_titleBar;
    EqSlider *m_preamp;
    Button *m_presetButton;
    QList<EqSlider*> m_sliders;
    QPoint m_pos;
    ToggleButton *m_on;
    ToggleButton *m_autoButton;
    EQGraph *m_eqg;
    QMenu *m_presetsMenu;
    QList<EQPreset*> m_presets;
    QList<EQPreset*> m_autoPresets;
    QString m_autoName;
    bool m_shaded;
};

#endif