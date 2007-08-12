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
#ifndef MAINVISUAL_H
#define MAINVISUAL_H

#include <QWidget>
#include <QResizeEvent>
#include <visualization.h>
#include <constants.h>

#include "logscale.h"

class QSettings;
class QTimer;
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
    virtual ~VisualBase() {}
    // return true if the output should stop
    virtual bool process( VisualNode *node ) = 0;
    virtual void draw( QPainter *, const QColor & ) = 0;
    virtual void resize( const QSize &size ) = 0;
    virtual void configChanged( QSettings & ) = 0;
};

class StereoScope : public VisualBase
{
public:
    StereoScope();
    virtual ~StereoScope();

    void resize( const QSize &size );
    void configChanged(QSettings &settings);
    bool process( VisualNode *node );
    void draw( QPainter *p, const QColor &back );

protected:
    QColor startColor, targetColor;
    //QMemArray<double> magnitudes;
    QVector <double> magnitudes;
    QSize size;
    bool rubberband;
    double falloff;
    int fps;
};

class MonoScope : public StereoScope
{
public:
    MonoScope();
    virtual ~MonoScope();

    bool process( VisualNode *node );
    void draw( QPainter *p, const QColor &back );
};




class MainVisual : public QWidget, public Visualization
{
    Q_OBJECT

public:
    MainVisual( QWidget *parent = 0, const char * = 0 );
    virtual ~MainVisual();

    //static Prefs *createPrefs( const QString &visualname,
      //          QWidget *parent, const char *name );
    static MainVisual *getPointer();

    VisualBase *visual() const { return vis; }
    void setVisual( VisualBase *newvis );
    //void setVisual( const QString &visualname );

    void add(Buffer *, unsigned long, int, int);
    void prepare();

    void configChanged(QSettings &settings);

    QSize minimumSizeHint() const { return sizeHint(); }
    QSize sizeHint() const { return QSize(4*4*4*2, 3*3*3*2); }

    void paintEvent( QPaintEvent * );
    void resizeEvent( QResizeEvent * );
    //void customEvent( QCustomEvent * );

    static QStringList visuals();

    void setFrameRate( int newfps );
    int frameRate() const { return fps; }

protected:
    void hideEvent ( QHideEvent *);
    void showEvent ( QShowEvent *);

public slots:
void timeout();

private:
    static MainVisual *pointer;
    VisualBase *vis;
    QPixmap pixmap;
    //QPtrList<VisualNode> nodes;
    QList <VisualNode*> nodes;
    QTimer *timer;
    bool playing;
    int fps;
};

class Skin;

class StereoAnalyzer : public VisualBase
{
public:
    StereoAnalyzer();
    virtual ~StereoAnalyzer();


    void resize( const QSize &size );
    void configChanged(QSettings &settings);
    bool process( VisualNode *node );
    void draw( QPainter *p, const QColor &back );

protected:
    QColor startColor, targetColor;
    //QMemArray<QRect> rects;
    QVector <QRect> rects;
    QVector <double> magnitudes;
    //QMemArray<double> magnitudes;
    QSize size;
    LogScale scale;
    double scaleFactor, falloff;
    int analyzerBarWidth, fps;
    //rfftw_plan plan;
    //fftw_real lin[512], rin[512], lout[1024], rout[1024];
    int intern_vis_data[19];

private:
    Skin *m_skin;
};

#endif
