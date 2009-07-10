/***************************************************************************
 *   Copyright (C) 2007-2008 by Ilya Kotov                                 *
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
#ifndef MAINVISUAL_H
#define MAINVISUAL_H

#include <QWidget>
#include <QResizeEvent>
#include <qmmp/visual.h>

#include "logscale.h"

class QSettings;
class QTimer;
class QMenu;
class QActionGroup;

class Buffer;


class VisualNode
{
public:
    VisualNode(short *l, short *r, unsigned long n, unsigned long o)
            : left(l), right(r), length(n), offset(o)
    {
        // left and right are allocated and then passed to this class
        // the code that allocated left and right should give up all ownership
    }

    ~VisualNode()
    {
        delete [] left;
        delete [] right;
    }

    short *left, *right;
    long length, offset;
};

class VisualBase
{
public:
    virtual ~VisualBase()
    {};
    virtual void clear() = 0;
    virtual bool process(VisualNode *node) = 0;
    virtual void draw(QPainter *) = 0;
    virtual const QString name() = 0;
};

class Skin;

class MainVisual : public Visual
{
    Q_OBJECT

public:
    MainVisual( QWidget *parent = 0);
    virtual ~MainVisual();

    static MainVisual *getPointer();

    void setVisual( VisualBase *newvis );

    void add(Buffer *, unsigned long, int, int);
    void clear();
    void paintEvent(QPaintEvent *);

protected:
    virtual void hideEvent (QHideEvent *);
    virtual void showEvent (QShowEvent *);
    virtual void mousePressEvent (QMouseEvent *);

public slots:
    void timeout();

private slots:
    void updateSettings();

private:
    void drawBackGround();
    void createMenu();
    void readSettings();
    static MainVisual *pointer;
    VisualBase *m_vis;
    QPixmap m_pixmap;
    QPixmap m_bg;
    QList <VisualNode*> m_nodes;
    QTimer *m_timer;
    bool m_playing;
    Skin *m_skin;
    //menu and actions
    QMenu *m_menu;
    //action groups
    QActionGroup *m_visModeGroup;
    QActionGroup *m_fpsGroup;
    QActionGroup *m_peaksFalloffGroup;
    QActionGroup *m_analyzerFalloffGroup;
    QActionGroup *m_analyzerModeGroup;
    QActionGroup *m_analyzerTypeGroup;
    QAction *m_peaksAction;
    QAction *m_transparentAction;
};

namespace mainvisual
{
class Analyzer : public VisualBase
{
public:
    Analyzer();
    virtual ~Analyzer();

    void clear();
    bool process(VisualNode *node);
    void draw(QPainter *p);
    const QString name() 
    {
        return "Analyzer";
    };

private:
    QSize m_size;
    int m_analyzerBarWidth, m_fps;
    double m_intern_vis_data[75];
    double m_peaks[75];
    double m_peaks_falloff;
    double m_analyzer_falloff;
    bool m_show_peaks;
    bool m_lines;
    int m_mode;
    Skin *m_skin;
};

class Scope : public VisualBase
{
public:
    Scope();
    virtual ~Scope();

    void clear();
    bool process(VisualNode *node);
    void draw(QPainter *p);
    const QString name() 
    {
        return "Scope";
    };

private:
    int m_intern_vis_data[75];
    Skin *m_skin;
};
}

#endif
