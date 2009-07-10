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
#ifndef ANALYZER_H
#define ANALYZER_H

#include <QWidget>
#include <QResizeEvent>
#include <qmmp/visual.h>
#include <QDir>

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

class Analyzer : public Visual
{
    Q_OBJECT

public:
    Analyzer( QWidget *parent = 0);
    virtual ~Analyzer();

    void add(Buffer *, unsigned long, int, int);
    void clear();
    void paintEvent( QPaintEvent * );

protected:
    virtual void hideEvent (QHideEvent *);
    virtual void showEvent (QShowEvent *);
    virtual void closeEvent (QCloseEvent *);

public slots:
    void timeout();

private:
    bool process(VisualNode *node);
    void draw(QPainter *p);
    QPixmap m_pixmap;
    QPixmap m_bg;
    QList <VisualNode*> m_nodes;
    QTimer *m_timer;
    int m_fps;
    double m_intern_vis_data[75];
    double m_peaks[75];
    double m_peaks_falloff;
    double m_analyzer_falloff;
    bool m_show_peaks;
    //colors
    QColor m_color1;
    QColor m_color2;
    QColor m_color3;
    QColor m_bgColor;
    QColor m_peakColor;
};


#endif
