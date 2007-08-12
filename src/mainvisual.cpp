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
#include <QTimer>
#include <QSettings>
#include <QPainter>
#include <buffer.h>
#include <constants.h>
#include <output.h>
#include <math.h>

#include "skin.h"
#include "fft.h"
#include "inlines.h"
#include "mainvisual.h"


MainVisual *MainVisual::pointer = 0;

MainVisual *MainVisual::getPointer()
{
    if ( !pointer )
        qFatal ( "MainVisual: this object not created!" );
    return pointer;
}

MainVisual::MainVisual ( QWidget *parent, const char *name )
        : QWidget ( parent ), vis ( 0 ), playing ( FALSE ), fps ( 20 )
{
    //setVisual(new MonoScope);
    setVisual ( new StereoAnalyzer );
    timer = new QTimer ( this );
    connect ( timer, SIGNAL ( timeout() ), this, SLOT ( timeout() ) );
    timer->setInterval ( 1000 / fps );
    timer->start();
    nodes.clear();
    pointer = this;

}

MainVisual::~MainVisual()
{
    delete vis;
    vis = 0;
}

/*void MainVisual::setVisual( const QString &visualname )
{
    VisualBase *newvis = 0;

    if ( visualname == "Mono Scope" )
   newvis = new MonoScope;
    else if (visualname == "Stereo Scope" )
   newvis = new StereoScope;
#ifdef FFTW
    else if ( visualname == "Mono Analyzer" )
   newvis = new MonoAnalyzer;
    else if ( visualname == "Stereo Analyzer" )
   newvis =  new StereoAnalyzer;
    else if ( visualname == "Mono Topograph" )
   newvis = new MonoTopograph;
    else if ( visualname == "Stereo Topograph" )
   newvis = new StereoTopograph;
    else if ( visualname == "Stereo Spectroscope" )
   newvis = new StereoSpectroscope;
    else if ( visualname == "Mono Spectroscope" )
   newvis = new MonoSpectroscope;
#endif // FFTW

    setVisual( newvis );
}*/

void MainVisual::setVisual ( VisualBase *newvis )
{
    //delete vis;

    vis = newvis;
    if ( vis )
        vis->resize ( size() );

    // force an update
    //timer->stop();
//     timer->start( 1000 / fps );
}

void MainVisual::configChanged ( QSettings &settings )
{
    /*QString newvis = settings.readEntry( "/MQ3/Visual/mode", "Mono Analyzer" );
    setVisual( newvis );*/

    //fps = settings.readNumEntry("/MQ3/Visual/frameRate", 20);
    fps = 20;
    timer->stop();
    timer->start ( 1000 / fps );
    if ( vis )
        vis->configChanged ( settings );
}

void MainVisual::prepare()
{
    //nodes.setAutoDelete(TRUE);
    nodes.clear();  //TODO memory leak??
    //nodes.setAutoDelete(FALSE);
}

void MainVisual::add ( Buffer *b, unsigned long w, int c, int p )
{
    if(!timer->isActive ())
        return; 
    long len = b->nbytes, cnt;
    short *l = 0, *r = 0;

    len /= c;
    len /= ( p / 8 );
    if ( len > 512 )
        len = 512;
    //len = 512;
    cnt = len;

    if ( c == 2 )
    {
        l = new short[len];
        r = new short[len];

        if ( p == 8 )
            stereo16_from_stereopcm8 ( l, r, b->data, cnt );
        else if ( p == 16 )
            stereo16_from_stereopcm16 ( l, r, ( short * ) b->data, cnt );
    }
    else if ( c == 1 )
    {
        l = new short[len];

        if ( p == 8 )
            mono16_from_monopcm8 ( l, b->data, cnt );
        else if ( p == 16 )
            mono16_from_monopcm16 ( l, ( short * ) b->data, cnt );
    }
    else
        len = 0;

    nodes.append ( new VisualNode ( l, r, len, w ) );
}

void MainVisual::timeout()
{
    VisualNode *node = 0;

    if ( /*playing &&*/ output() )
    {
        //output()->mutex()->lock ();
        //long olat = output()->latency();
        //long owrt = output()->written();
        //output()->mutex()->unlock();

        //long synctime = owrt < olat ? 0 : owrt - olat;

        mutex()->lock ();
        VisualNode *prev = 0;
        while ( ( !nodes.isEmpty() ) )
        {
            node = nodes.first();
            /*if ( node->offset > synctime )
               break;*/

            if ( prev )
                delete prev;
            nodes.removeFirst();

            prev = node;

        }
        mutex()->unlock();
        node = prev;
    }

    bool stop = TRUE;
    if ( vis )
        stop = vis->process ( node );
    delete node;

    if ( vis )
    {
        pixmap.fill ( Qt::transparent );
        QPainter p ( &pixmap );
        //vis->draw( &p, backgroundColor() );
        vis->draw ( &p, "Green" );
    }
    else
        //pixmap.fill( backgroundColor() );
        pixmap.fill ( "Red" );
    //bitBlt(this, 0, 0, &pixmap);
//     QPainter painter(this);
//     painter.drawPixmap(0,0,pixmap);
    update();
    /*if (! playing && stop)
    timer->stop();*/
}

void MainVisual::paintEvent ( QPaintEvent * )
{
    //bitBlt(this, 0, 0, &pixmap);
    QPainter painter ( this );
    painter.drawPixmap ( 0,0,pixmap );
}

void MainVisual::resizeEvent ( QResizeEvent *event )
{
    pixmap = QPixmap ( event->size() );
    /*pixmap.resize(event->size());
    pixmap.fill(backgroundColor());
    QWidget::resizeEvent( event );*/
    if ( vis )
        vis->resize ( size() );
}

/*void MainVisual::customEvent(QCustomEvent *event)
{
    switch (event->type()) {
    case OutputEvent::Playing:
   playing = TRUE;
   // fall through intended

    case OutputEvent::Info:
    case OutputEvent::Buffering:
    case OutputEvent::Paused:
   if (! timer->isActive())
       timer->start(1000 / fps);

   break;

    case OutputEvent::Stopped:
    case OutputEvent::Error:
   playing = FALSE;
   break;

    default:
   ;
    }
}*/

QStringList MainVisual::visuals()
{
    QStringList list;
    list << "Stereo Scope";
    list << "Mono Scope";
#ifdef FFTW
    list << "Stereo Analyzer";
    list << "Mono Analyzer";
    list << "Stereo Topograph";
    list << "Mono Topograph";
    list << "Stereo Spectroscope";
    list << "Mono Spectroscope";
#endif // FFTW

    return list;
}

void MainVisual::hideEvent ( QHideEvent *)
{
    timer->stop();
}

void MainVisual::showEvent ( QShowEvent *)
{
    timer->start();
}

MonoScope::MonoScope()
{}

MonoScope::~MonoScope()
{}

bool MonoScope::process ( VisualNode *node )
{
    bool allZero = TRUE;
    int i;
    long s, index, indexTo, step = 512 / size.width();
    double *magnitudesp = magnitudes.data();
    double val, tmp;

    if ( node )
    {

        index = 0;
        for ( i = 0; i < size.width(); i++ )
        {
            indexTo = index + step;

            if ( rubberband )
            {
                val = magnitudesp[ i ];
                if ( val < 0. )
                {
                    val += falloff;
                    if ( val > 0. )
                        val = 0.;
                }
                else
                {
                    val -= falloff;
                    if ( val < 0. )
                        val = 0.;
                }
            }
            else
                val = 0.;

            for ( s = index; s < indexTo && s < node->length; s++ )
            {
                tmp = ( double ( node->left[s] ) +
                        ( node->right ? double ( node->right[s] ) : 0 ) *
                        double ( size.height() / 2 ) ) / 65536.;
                if ( tmp > 0 )
                    val = ( tmp > val ) ? tmp : val;
                else
                    val = ( tmp < val ) ? tmp : val;
            }

            if ( val != 0. )
                allZero = FALSE;
            magnitudesp[ i ] = val;
            index = indexTo;
        }
    }
    else if ( rubberband )
    {
        for ( i = 0; i < size.width(); i++ )
        {
            val = magnitudesp[ i ];
            if ( val < 0 )
            {
                val += 2;
                if ( val > 0. )
                    val = 0.;
            }
            else
            {
                val -= 2;
                if ( val < 0. )
                    val = 0.;
            }

            if ( val != 0. )
                allZero = FALSE;
            magnitudesp[ i ] = val;
        }
    }
    else
    {
        for ( i = 0; i < size.width(); i++ )
            magnitudesp[ i ] = 0.;
    }

    return allZero;
}

void MonoScope::draw ( QPainter *p, const QColor &back )
{
    double *magnitudesp = magnitudes.data();
    double r, g, b, per;

    p->fillRect ( 0, 0, size.width(), size.height(), back );
    for ( int i = 1; i < size.width(); i++ )
    {
        per = double ( magnitudesp[ i ] ) /
              double ( size.height() / 4 );
        if ( per < 0.0 )
            per = -per;
        if ( per > 1.0 )
            per = 1.0;
        else if ( per < 0.0 )
            per = 0.0;

        r = startColor.red() + ( targetColor.red() -
                                 startColor.red() ) * ( per * per );
        g = startColor.green() + ( targetColor.green() -
                                   startColor.green() ) * ( per * per );
        b = startColor.blue() + ( targetColor.blue() -
                                  startColor.blue() ) * ( per * per );

        if ( r > 255.0 )
            r = 255.0;
        else if ( r < 0.0 )
            r = 0;

        if ( g > 255.0 )
            g = 255.0;
        else if ( g < 0.0 )
            g = 0;

        if ( b > 255.0 )
            b = 255.0;
        else if ( b < 0.0 )
            b = 0;

        p->setPen ( QColor ( int ( r ), int ( g ), int ( b ) ) );
        p->drawLine ( i - 1, size.height() / 2 + int ( magnitudesp[ i - 1 ] ),
                      i, int ( size.height() / 2 + magnitudesp[ i ] ) );
        //qDebug("draw %d", int(magnitudesp[ i ]));
    }
}

StereoScope::StereoScope()
        : rubberband ( true ), falloff ( 1.0 ), fps ( 20 )
{
    startColor = Qt::white;

    /*val = settings.readEntry("/MQ3/Scope/targetColor");
    if (! val.isEmpty())
    targetColor = QColor(val);
    else*/
    targetColor = Qt::red;
}

StereoScope::~StereoScope()
{}

void StereoScope::resize ( const QSize &newsize )
{
    size = newsize;

    uint os = magnitudes.size();
    magnitudes.resize ( size.width() * 2 );
    for ( ; os < magnitudes.size(); os++ )
        magnitudes[os] = 0.0;
}

void StereoScope::configChanged ( QSettings &settings )
{
    QString val;

    // need the framerate for the fall off speed
    //fps = settings.readNumEntry("/MQ3/Visual/frameRate", 20);
    fps = 20;
    /*val = settings.readEntry("/MQ3/Scope/startColor");
    if (! val.isEmpty())
        startColor = QColor(val);
    else*/
    startColor = Qt::green;

    /*val = settings.readEntry("/MQ3/Scope/targetColor");
    if (! val.isEmpty())
    targetColor = QColor(val);
    else*/
    targetColor = Qt::red;

    //rubberband = settings.readBoolEntry( "/MQ3/Scope/enableRubberBand", true );
    rubberband = TRUE;
    //val = settings.readEntry( "/MQ3/Scope/fallOffSpeed", "Normal" );
    val == "Normal";
    if ( val == "Slow" )
        falloff = .125;
    else if ( val == "Fast" )
        falloff = .5;
    else
        falloff = .25;

    falloff *= ( 80. / double ( fps ) );

    resize ( size );
}

bool StereoScope::process ( VisualNode *node )
{
    bool allZero = TRUE;
    int i;
    long s, index, indexTo, step = 256 / size.width();
    double *magnitudesp = magnitudes.data();
    double valL, valR, tmpL, tmpR;

    if ( node )
    {
        index = 0;
        for ( i = 0; i < size.width(); i++ )
        {
            indexTo = index + step;

            if ( rubberband )
            {
                valL = magnitudesp[ i ];
                valR = magnitudesp[ i + size.width() ];
                if ( valL < 0. )
                {
                    valL += falloff;
                    if ( valL > 0. )
                        valL = 0.;
                }
                else
                {
                    valL -= falloff;
                    if ( valL < 0. )
                        valL = 0.;
                }
                if ( valR < 0. )
                {
                    valR += falloff;
                    if ( valR > 0. )
                        valR = 0.;
                }
                else
                {
                    valR -= falloff;
                    if ( valR < 0. )
                        valR = 0.;
                }
            }
            else
                valL = valR = 0.;

            for ( s = index; s < indexTo && s < node->length; s++ )
            {
                tmpL = ( ( node->left ?
                           double ( node->left[s] ) : 0. ) *
                         double ( size.height() / 4 ) ) / 32768.;
                tmpR = ( ( node->right ?
                           double ( node->right[s] ) : 0. ) *
                         double ( size.height() / 4 ) ) / 32768.;
                if ( tmpL > 0 )
                    valL = ( tmpL > valL ) ? tmpL : valL;
                else
                    valL = ( tmpL < valL ) ? tmpL : valL;
                if ( tmpR > 0 )
                    valR = ( tmpR > valR ) ? tmpR : valR;
                else
                    valR = ( tmpR < valR ) ? tmpR : valR;
            }

            if ( valL != 0. || valR != 0. )
                allZero = FALSE;

            magnitudesp[ i ] = valL;
            magnitudesp[ i + size.width() ] = valR;

            index = indexTo;
        }
    }
    else if ( rubberband )
    {
        for ( i = 0; i < size.width(); i++ )
        {
            valL = magnitudesp[ i ];
            if ( valL < 0 )
            {
                valL += 2;
                if ( valL > 0. )
                    valL = 0.;
            }
            else
            {
                valL -= 2;
                if ( valL < 0. )
                    valL = 0.;
            }

            valR = magnitudesp[ i + size.width() ];
            if ( valR < 0. )
            {
                valR += falloff;
                if ( valR > 0. )
                    valR = 0.;
            }
            else
            {
                valR -= falloff;
                if ( valR < 0. )
                    valR = 0.;
            }

            if ( valL != 0. || valR != 0. )
                allZero = FALSE;

            magnitudesp[ i ] = valL;
            magnitudesp[ i + size.width() ] = valR;
        }
    }
    else
    {
        for ( i = 0; ( unsigned ) i < magnitudes.size(); i++ )
            magnitudesp[ i ] = 0.;
    }

    return allZero;
}

void StereoScope::draw ( QPainter *p, const QColor &back )
{
    double *magnitudesp = magnitudes.data();
    double r, g, b, per;

    p->fillRect ( 0, 0, size.width(), size.height(), back );
    for ( int i = 1; i < size.width(); i++ )
    {
        // left
        per = double ( magnitudesp[ i ] * 2 ) /
              double ( size.height() / 4 );
        if ( per < 0.0 )
            per = -per;
        if ( per > 1.0 )
            per = 1.0;
        else if ( per < 0.0 )
            per = 0.0;

        r = startColor.red() + ( targetColor.red() -
                                 startColor.red() ) * ( per * per );
        g = startColor.green() + ( targetColor.green() -
                                   startColor.green() ) * ( per * per );
        b = startColor.blue() + ( targetColor.blue() -
                                  startColor.blue() ) * ( per * per );

        if ( r > 255.0 )
            r = 255.0;
        else if ( r < 0.0 )
            r = 0;

        if ( g > 255.0 )
            g = 255.0;
        else if ( g < 0.0 )
            g = 0;

        if ( b > 255.0 )
            b = 255.0;
        else if ( b < 0.0 )
            b = 0;

        p->setPen ( QColor ( int ( r ), int ( g ), int ( b ) ) );
        /*p->drawLine ( i - 1, ( size.height() / 4 ) + int ( magnitudesp[ i - 1 ] ),
                      i, ( size.height() / 4 ) + int ( magnitudesp[ i ] ) );*/

        //right
        per = double ( magnitudesp[ i + size.width() ] * 2 ) /
              double ( size.height() / 4 );
        if ( per < 0.0 )
            per = -per;
        if ( per > 1.0 )
            per = 1.0;
        else if ( per < 0.0 )
            per = 0.0;

        r = startColor.red() + ( targetColor.red() -
                                 startColor.red() ) * ( per * per );
        g = startColor.green() + ( targetColor.green() -
                                   startColor.green() ) * ( per * per );
        b = startColor.blue() + ( targetColor.blue() -
                                  startColor.blue() ) * ( per * per );

        if ( r > 255.0 )
            r = 255.0;
        else if ( r < 0.0 )
            r = 0;

        if ( g > 255.0 )
            g = 255.0;
        else if ( g < 0.0 )
            g = 0;

        if ( b > 255.0 )
            b = 255.0;
        else if ( b < 0.0 )
            b = 0;

        p->setPen ( QColor ( int ( r ), int ( g ), int ( b ) ) );
        /*p->drawLine ( i - 1, ( size.height() * 3 / 4 ) +
                      int ( magnitudesp[ i + size.width() - 1 ] ),
                      i, ( size.height() * 3 / 4 ) + int ( magnitudesp[ i + size.width() ] ) );*/
    }
}

StereoAnalyzer::StereoAnalyzer()
        : scaleFactor ( 1.0 ), falloff ( 1.0 ), analyzerBarWidth ( 4 ), fps ( 20 )
{
    //plan =  rfftw_create_plan(512, FFTW_REAL_TO_COMPLEX, FFTW_ESTIMATE);
    startColor = Qt::green;
    targetColor = Qt::red;
    falloff = .5;
    for ( int i = 0; i< 19; ++i )
        intern_vis_data[i] = 0;
    m_skin = Skin::getPointer();
}

StereoAnalyzer::~StereoAnalyzer()
{
    //rfftw_destroy_plan(plan);
}

void StereoAnalyzer::resize ( const QSize &newsize )
{
    size = newsize;

    scale.setMax ( 192, size.width() / analyzerBarWidth );

    rects.resize ( scale.range() );
    int i = 0, w = 0;
    for ( ; ( unsigned ) i < rects.count(); i++, w += analyzerBarWidth )
        rects[i].setRect ( w, size.height() / 2, analyzerBarWidth - 1, 1 );

    int os = magnitudes.size();
    magnitudes.resize ( scale.range() * 2 );
    for ( ; ( unsigned ) os < magnitudes.size(); os++ )
        magnitudes[os] = 0.0;

    // scaleFactor = double( size.height() / 2 ) / log( 512.0 );
}

void StereoAnalyzer::configChanged ( QSettings &settings )
{
    QString val;

    // need the framerate for the fall off speed
    /*fps = settings.readNumEntry("/MQ3/Visual/frameRate", 20);

    val = settings.readEntry("/MQ3/Analyzer/startColor");
    if (! val.isEmpty())
        startColor = QColor(val);
    else
    startColor = Qt::green;

    val = settings.readEntry("/MQ3/Analyzer/targetColor");
    if (! val.isEmpty())
    targetColor = QColor(val);
    else
    targetColor = Qt::red;

    analyzerBarWidth = settings.readNumEntry( "/MQ3/Analyzer/barWidth", 4 );

    val = settings.readEntry( "/MQ3/Analyzer/fallOffSpeed", "Normal" );
    if ( val == "Slow" )
    falloff = .25;
    else if ( val == "Fast" )
    falloff = 1.;
    else
    falloff = .5;

    falloff *= ( 80. / double( fps ) );

    resize( size );*/
}

bool StereoAnalyzer::process ( VisualNode *node )
{
    bool allZero = TRUE;
    uint i;
    long w = 0, index;
    QRect *rectsp = rects.data();
    double *magnitudesp = magnitudes.data();
    double magL, magR, tmp;
    static fft_state *state = 0;
    if ( !state )
        state = fft_init();
    //if(node)
    //float tmp_out[512];
    short dest[256];

    const int xscale[] =
        {
            0, 1, 2, 3, 4, 5, 6, 7, 8, 11, 15, 20, 27,
            36, 47, 62, 82, 107, 141, 184, 255
        };

    if ( node )
    {
        i = node->length;
        //fast_real_set_from_short(lin, node->left, node->length);
        //if (node->right)
        //fast_real_set_from_short(rin, node->right, node->length);
        //fft_perform ( node->left, tmp_out, state );
        /*for(int j = 0; j<256; ++j  )
           dest*/

        //calc_mono_freq ( short dest[2][256], short src[2][512], int nch )

        calc_freq ( dest, node->left );

    }
    else
        return FALSE;
    const double y_scale = 3.60673760222;   /* 20.0 / log(256) */
    int max = 19, y, j;
    //int intern_vis_data[19];

    for ( int i = 0; i < max; i++ )
    {
        for ( j = xscale[i], y = 0; j < xscale[i + 1]; j++ )
        {
            if ( dest[j] > y )
                y = dest[j];
        }
        y >>= 7;
        if ( y != 0 )
        {
            intern_vis_data[i] = log ( y ) * y_scale;
            //qDebug("%d",y);
            if ( intern_vis_data[i] > 15 )
                intern_vis_data[i] = 15;
            if ( intern_vis_data[i] < 0 )
                intern_vis_data[i] = 0;
        }
    }
    return TRUE;

//     fast_reals_set(lin + i, rin + i, 0, 512 - i);

    //rfftw_one(plan, lin, lout);
    //rfftw_one(plan, rin, rout);

    /*index = 1;
    for ( i = 0; i < rects.count(); i++, w += analyzerBarWidth )
    {
       magL = (log(lout[index] * lout[index] +
              lout[512 - index] * lout[512 - index]) - 22.0) *
              scaleFactor;
       magR = (log(rout[index] * rout[index] +
              rout[512 - index] * rout[512 - index]) - 22.0) *
              scaleFactor;*/
    //magL = fftperform()*/
    /*magL = ( log ( tmp_out[index] * tmp_out[index] +
                   tmp_out[512 - index] * tmp_out[512 - index] ) - 22.0 ) *
           scaleFactor;

    if ( magL > size.height() )
       magL = size.height() ;
    if ( magL < magnitudesp[i] )
    {
       tmp = magnitudesp[i] - falloff;
       if ( tmp < magL )
          tmp = magL;
       magL = tmp;
    }
    if ( magL < 1. )
       magL = 1.;

    if ( magR > size.height() )
       magR = size.height();
    if ( magR < magnitudesp[i + scale.range() ] )
    {
       tmp = magnitudesp[i + scale.range() ] - falloff;
       if ( tmp < magR )
          tmp = magR;
       magR = tmp;
    }
    if ( magR < 1. )
       magR = 1.;

    if ( magR != 1 || magL != 1 )
       allZero = FALSE;

    magnitudesp[i] = magL;
    magnitudesp[i + scale.range() ] = magR;

    //rectsp[i].setTop ( size.height() / 2 - int ( magL ) );
    //rectsp[i].setBottom ( size.height() / 2 + int ( magR ) );
    rectsp[i].setTop ( size.height() - int ( magL ) );
    rectsp[i].setBottom ( size.height() );

    index = scale[i];
    }

    return allZero;*/
}

void StereoAnalyzer::draw ( QPainter *p, const QColor &back )
{
    p->setPen ( "Cyan" );
    //p->fillRect ( 0, 0, size.width(), size.height(), Qt::transparent );
    //int intern_vis_data[19];
    //qDebug("%d",int(intern_vis_data[3]));
    //p->fill(Qt::transparent);
    for ( int j= 0; j<19; ++j )
    {
        for ( int i = 0; i<=intern_vis_data[j]; ++i )
        {
            p->setPen ( m_skin->getVisBarColor ( i ) );
            p->drawLine ( j*4,size.height()-i, ( j+1 ) *4-2,size.height()-i );
        }
    }
    for ( int i = 0; i< 19; ++i )
        intern_vis_data[i] = 0;
    //update();
    /*QRect *rectsp = rects.data();
    double r, g, b, per;

    p->fillRect ( 0, 0, size.width(), size.height(), back );
    for ( uint i = 0; i < rects.count(); i++ )
    {
       per = double ( rectsp[i].height() - 2 ) / double ( size.height() );
       if ( per > 1.0 )
          per = 1.0;
       else if ( per < 0.0 )
          per = 0.0;

       r = startColor.red() + ( targetColor.red() -
                                startColor.red() ) * ( per * per );
       g = startColor.green() + ( targetColor.green() -
                                  startColor.green() ) * ( per * per );
       b = startColor.blue() + ( targetColor.blue() -
                                 startColor.blue() ) * ( per * per );

       if ( r > 255.0 )
          r = 255.0;
       else if ( r < 0.0 )
          r = 0;

       if ( g > 255.0 )
          g = 255.0;
       else if ( g < 0.0 )
          g = 0;

       if ( b > 255.0 )
          b = 255.0;
       else if ( b < 0.0 )
          b = 0;

       p->fillRect ( rectsp[i], QColor ( int ( r ), int ( g ), int ( b ) ) );
    }*/

}
