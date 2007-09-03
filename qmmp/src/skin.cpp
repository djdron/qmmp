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
#include <QDir>
#include <QSettings>
#include <QPainter>
#include <QPolygon>

#include "skin.h"




Skin *Skin::pointer = 0;

Skin *Skin::getPointer()
{
    if ( !pointer )
        pointer = new Skin();
    return pointer;
}

QPixmap Skin::getPixmap ( const QString& name, QDir dir )
{
    dir.setFilter ( QDir::Files | QDir::Hidden | QDir::NoSymLinks );
    QFileInfoList f = dir.entryInfoList();
    for ( int j = 0; j < f.size(); ++j )
    {
        QFileInfo fileInfo = f.at ( j );
        QString fn = fileInfo.fileName().toLower();
        if ( fn.section ( ".",0,0 ) == name )
        {
            return QPixmap ( fileInfo.filePath() );
        }
    }
    return QPixmap();
}

Skin::Skin ( QObject *parent )
        : QObject ( parent )
{
    pointer = this;
    QSettings settings(QDir::homePath()+"/.qmmp/qmmprc", QSettings::IniFormat);
    QString path = settings.value("skin_path","").toString();
    if (path.isEmpty() || !QDir(path).exists ())
        path = ":/default";
    setSkin (QDir::cleanPath(path));
    /* skin directory */
    QDir skinDir(QDir::homePath()+"/.qmmp");
    skinDir.mkdir ("skins");
}


Skin::~Skin()
{}

void Skin::setSkin ( const QString& path )
{
    QSettings settings(QDir::homePath()+"/.qmmp/qmmprc", QSettings::IniFormat);
    settings.setValue("skin_path",path);

    qDebug ( path.toAscii() );    //TODO don't clear lists
    m_skin_dir = QDir ( path );

    m_pledit_txt.clear();
    loadPLEdit();
    loadMain();
    buttons.clear();
    loadButtons();
    loadShufRep();
    titlebar.clear();
    loadTitleBar();
    loadPosBar();
    m_numbers.clear();
    loadNumbers();
    m_pl_parts.clear();
    loadPlayList();
    m_eq_parts.clear();
    m_eq_bar.clear();
    m_eq_spline.clear();
    loadEqMain();
    loadVisColor();
    loadLetters();
    loadMonoSter();
    loadVolume();
    loadBalance();
    loadRegion();

    emit skinChanged();
}

void Skin::loadMain()
{
    QPixmap *pixmap = getPixmap ("main");
    if (!pixmap)
        pixmap = getDummyPixmap("main");

    m_main = pixmap->copy ( 0,0,275,116 );
    delete pixmap;
}

void Skin::loadButtons()
{

    QPixmap *pixmap = getPixmap ("cbuttons");

    if (!pixmap)
        pixmap = getDummyPixmap("cbuttons");

    buttons[BT_PREVIOUS_N] = pixmap->copy ( 0, 0,23,18 );
    buttons[BT_PREVIOUS_P] = pixmap->copy ( 0,18,23,18 );

    buttons[BT_PLAY_N] = pixmap->copy ( 23, 0,23,18 );
    buttons[BT_PLAY_P] = pixmap->copy ( 23,18,23,18 );

    buttons[BT_PAUSE_N] = pixmap->copy ( 46, 0,23,18 );
    buttons[BT_PAUSE_P] = pixmap->copy ( 46,18,23,18 );

    buttons[BT_STOP_N] = pixmap->copy ( 69, 0,23,18 );
    buttons[BT_STOP_P] = pixmap->copy ( 69,18,23,18 );

    buttons[BT_NEXT_N] = pixmap->copy ( 92, 0,22,18 );
    buttons[BT_NEXT_P] = pixmap->copy ( 92,16,22,18 );

    buttons[BT_EJECT_N] = pixmap->copy ( 114, 0,22,16 );
    buttons[BT_EJECT_P] = pixmap->copy ( 114,16,22,16 );
    delete pixmap;

}

void Skin::loadTitleBar()
{

    QPixmap *pixmap = getPixmap ("titlebar");

    if (!pixmap)
        pixmap = getDummyPixmap("titlebar");

    buttons[BT_MENU_N] = pixmap->copy ( 0,0,9,9 );
    buttons[BT_MENU_P] = pixmap->copy ( 0,9,9,9 );
    buttons[BT_MINIMIZE_N] = pixmap->copy ( 9,0,9,9 );
    buttons[BT_MINIMIZE_P] = pixmap->copy ( 9,9,9,9 );
    buttons[BT_CLOSE_N] = pixmap->copy ( 18,0,9,9 );
    buttons[BT_CLOSE_P] = pixmap->copy ( 18,9,9,9 );
    buttons[BT_SHADE1_N] = pixmap->copy ( 0,18,9,9 );
    buttons[BT_SHADE1_P] = pixmap->copy ( 9,18,9,9 );
    buttons[BT_SHADE2_N] = pixmap->copy ( 0,27,9,9 );
    buttons[BT_SHADE2_P] = pixmap->copy ( 9,27,9,9 );
    titlebar[TITLEBAR_A] = pixmap->copy ( 27, 0,275,14 );
    titlebar[TITLEBAR_I] = pixmap->copy ( 27,15,275,14 );
    titlebar[STATUSBAR_A] = pixmap->copy ( 27,29,275,14 );
    titlebar[STATUSBAR_I] = pixmap->copy ( 27,42,275,14 );
    delete pixmap;

}

void Skin::loadPosBar()
{

    QPixmap *pixmap = getPixmap ("posbar");

    if (!pixmap)
        pixmap = getDummyPixmap("posbar");

    if (pixmap->width() > 249)
    {
        buttons[BT_POSBAR_N] = pixmap->copy ( 248,0,29, pixmap->height());
        buttons[BT_POSBAR_P] = pixmap->copy ( 278,0,29, pixmap->height());
    }
    else
    {
        QPixmap dummy(29, pixmap->height());
        dummy.fill(Qt::transparent);
        buttons[BT_POSBAR_N] = dummy;
        buttons[BT_POSBAR_P] = dummy;
    }
    posbar = pixmap->copy ( 0,0,248,pixmap->height() );
    delete pixmap;

}

void Skin::loadNumbers()
{
    QPixmap *pixmap = getPixmap ( "numbers" );
    if ( !pixmap )
    {
        pixmap = getPixmap ( "nums_ex" );
    }
    for ( uint i = 0; i < 10; i++ )
    {
        m_numbers << pixmap->copy ( i*9, 0, 9, 13 );
    }
    if (pixmap->width() > 107)
    {
        m_numbers << pixmap->copy(99, 0, 9,13 );
    }
    else
    {   // We didn't find "-" symbol. So we have to extract it from "2".
        // Winamp uses this method too.
        QPixmap pix = pixmap->copy(90,0,9,13);
        QPixmap minus = pixmap->copy(18,6,9,1);
        QPainter paint(&pix);
        paint.drawPixmap(0,6, minus);
        m_numbers << pix;
    }
    delete pixmap;
}

void Skin::loadPlayList()
{

    QPixmap *pixmap = getPixmap ("pledit");

    if (!pixmap)
        pixmap = getDummyPixmap("pledit");

    m_pl_parts[PL_CORNER_UL_A] = pixmap->copy ( 0,0,25,20 );
    m_pl_parts[PL_CORNER_UL_I] = pixmap->copy ( 0,21,25,20 );

    m_pl_parts[PL_CORNER_UR_A] = pixmap->copy ( 153,0,25,20 );
    m_pl_parts[PL_CORNER_UR_I] = pixmap->copy ( 153,21,25,20 );

    m_pl_parts[PL_TITLEBAR_A] = pixmap->copy ( 26,0,100,20 );
    m_pl_parts[PL_TITLEBAR_I] = pixmap->copy ( 26,21,100,20 );

    m_pl_parts[PL_TFILL1_A] = pixmap->copy ( 127,0,25,20 );
    m_pl_parts[PL_TFILL1_I] = pixmap->copy ( 127,21,25,20 );

    //m_pl_parts[PL_TFILL2_A] = pixmap->copy();//FIXME: Ã¯Â¿Â½Ã¯Â¿Â½Ã¯Â¿Â½Ã¯Â¿Â½Ã¯Â¿Â½
    //m_pl_parts[PL_TFILL2_I] = pixmap->copy();

    m_pl_parts[PL_LFILL] = pixmap->copy ( 0,42,12,29 );
    m_pl_parts[PL_RFILL] = pixmap->copy ( 31,42,20,29 ); //???

    m_pl_parts[PL_LSBAR] = pixmap->copy ( 0,72,125,38 );
    m_pl_parts[PL_RSBAR] = pixmap->copy ( 126,72,150,38 );
    m_pl_parts[PL_SFILL1] = pixmap->copy ( 179,0,25,38 );
    m_pl_parts[PL_SFILL2] = pixmap->copy ( 250,21,75,38 );

    m_pl_parts[PL_CONTROL] = pixmap->copy(129,94,60,8);

    buttons[PL_BT_ADD] = pixmap->copy ( 11,80,25,18 );
    buttons[PL_BT_SUB] = pixmap->copy ( 40,80,25,18 );
    buttons[PL_BT_SEL] = pixmap->copy ( 70,80,25,18 );
    buttons[PL_BT_SORT] = pixmap->copy ( 99,80,25,18 );
    buttons[PL_BT_LST] = pixmap->copy(229, 80, 25, 18);
    buttons[PL_BT_SCROLL_N] = pixmap->copy ( 52,53,8,18 );
    buttons[PL_BT_SCROLL_P] = pixmap->copy ( 61,53,8,18 );

}

QPixmap *Skin::getPixmap ( const QString& name )
{
    m_skin_dir.setFilter ( QDir::Files | QDir::Hidden | QDir::NoSymLinks );
    QFileInfoList f = m_skin_dir.entryInfoList();
    for ( int j = 0; j < f.size(); ++j )
    {
        QFileInfo fileInfo = f.at ( j );
        QString fn = fileInfo.fileName().toLower();
        if ( fn.section ( ".",0,0 ) == name )
        {
            return new QPixmap ( fileInfo.filePath() );
        }
    }
    return 0;
}

void Skin::loadPLEdit()
{
    m_skin_dir.setFilter ( QDir::Files | QDir::Hidden | QDir::NoSymLinks );
    QString path;
    QFileInfoList list = m_skin_dir.entryInfoList();
    for ( int i = 0; i < list.size(); ++i )
    {
        QFileInfo fileInfo = list.at ( i );
        if ( fileInfo.fileName().toLower() == "pledit.txt" )
        {
            path = fileInfo.filePath ();
            break;
        }
    }

    if ( path.isNull () )
    {
        qDebug ( "Skin: Cannot find pledit.txt" );
        return;
    }


    QFile file ( path );

    if ( !file.open ( QIODevice::ReadOnly | QIODevice::Text ) )
        return;

    while ( !file.atEnd () )
    {
        QByteArray line = file.readLine ();
        QList<QByteArray> l = line.split ( '=' );
        if ( l.count () == 2 )
        {
            m_pledit_txt[l[0].toLower () ] = l[1].trimmed();
        }
        else if ( line.length() == 0 )
        {
            break;
        }
    }

}

void Skin::loadEqMain()
{
    QPixmap *pixmap = getPixmap ("eqmain");

    if (!pixmap)
        pixmap = getDummyPixmap("eqmain");

    m_eq_parts[ EQ_MAIN ] = pixmap->copy ( 0,0,275,116 );
    m_eq_parts[ EQ_TITLEBAR_A ] = pixmap->copy ( 0,134,275,14 );
    m_eq_parts[ EQ_TITLEBAR_I ] = pixmap->copy ( 0,149,275,14 );
    m_eq_parts[ EQ_GRAPH ] = pixmap->copy ( 0,294,113,19 );
    for ( int i = 0; i < 14; ++i )
    {
        m_eq_bar << pixmap->copy ( 13 + i*15,164,14,63 );
    }
    for ( int i = 0; i < 14; ++i )
    {
        m_eq_bar << pixmap->copy ( 13 + i*15,229,14,63 );
    }
    buttons[ EQ_BT_BAR_N ] = pixmap->copy ( 0,164,11,11 );
    buttons[ EQ_BT_BAR_P ] = pixmap->copy ( 0,164+12,11,11 );

    buttons[ EQ_BT_ON_N ] = pixmap->copy ( 69,119,28,12 );
    buttons[ EQ_BT_ON_P ] = pixmap->copy ( 128,119,28,12 );
    buttons[ EQ_BT_OFF_N ] = pixmap->copy ( 10, 119,28,12 );
    buttons[ EQ_BT_OFF_P ] = pixmap->copy ( 187,119,28,12 );

    buttons[ EQ_BT_PRESETS_N ] = pixmap->copy ( 224,164,44,12 );
    buttons[ EQ_BT_PRESETS_P ] = pixmap->copy ( 224,176,44,12 );

    buttons[ EQ_BT_AUTO_1_N ] = pixmap->copy ( 94,119,33,12 );
    buttons[ EQ_BT_AUTO_1_P ] = pixmap->copy ( 153,119,33,12 );
    buttons[ EQ_BT_AUTO_0_N ] = pixmap->copy ( 35, 119,33,12 );
    buttons[ EQ_BT_AUTO_0_P ] = pixmap->copy ( 212,119,33,12 );

    for ( int i = 0; i < 19; ++i )
    {
        m_eq_spline << pixmap->copy ( 115, 294+i, 1, 1 );
    }
    delete pixmap;

}

void Skin::loadVisColor()
{
    QList <QColor> colors;
    m_skin_dir.setFilter ( QDir::Files | QDir::Hidden | QDir::NoSymLinks );
    QString path;
    QFileInfoList list = m_skin_dir.entryInfoList();
    for ( int i = 0; i < list.size(); ++i )
    {
        QFileInfo fileInfo = list.at ( i );
        if ( fileInfo.fileName().toLower() == "viscolor.txt" )
        {
            path = fileInfo.filePath ();
            break;
        }
    }

    if ( path.isNull () )
    {
        qDebug ( "Skin: Cannot find viscolor.txt" );
        return;
    }


    QFile file ( path );

    if ( !file.open ( QIODevice::ReadOnly | QIODevice::Text ) )
        return;

    int j = 0;
    while ( !file.atEnd () && j<24 )
    {
        j++;
        QByteArray line = file.readLine ();
        QString tmp = QString::fromAscii ( line );
        tmp = tmp.trimmed ();
        int i = tmp.indexOf ( "//" );
        if ( i>0 )
            tmp.truncate ( tmp.indexOf ( "//" ) );
        QStringList list = tmp.split ( "," );
        if ( list.count () >= 3 )
        {
            //colors
            int r = list.at ( 0 ).toInt();
            int g = list.at ( 1 ).toInt();
            int b = list.at ( 2 ).toInt();
            colors << QColor ( r,g,b );
        }
        else if ( line.length() == 0 )
        {
            break;
        }
    }
    if ( colors.size() <24 )
    {
        qWarning ( "Skin: cannot parse viscolor.txt" );
        while ( colors.size() <24 )
            colors << QColor ( 0,0,0 );
    }
    m_vis_bars.clear();
    for ( j = 17; j > 1; --j )
        m_vis_bars << colors.at ( j );

}

void Skin::loadShufRep()
{
    QPixmap *pixmap = getPixmap ("shufrep");

    if (!pixmap)
        pixmap = getDummyPixmap("shufrep");

    buttons[ BT_EQ_ON_N ] = pixmap->copy ( 0,73,23,12 );
    buttons[ BT_EQ_ON_P ] = pixmap->copy ( 46,73,23,12 );
    buttons[ BT_EQ_OFF_N ] = pixmap->copy ( 0,61,23,12 );
    buttons[ BT_EQ_OFF_P ] = pixmap->copy ( 46,61,23,12 );

    buttons[ BT_PL_ON_N ] = pixmap->copy ( 23,73,23,12 );
    buttons[ BT_PL_ON_P ] = pixmap->copy ( 69,73,23,12 );
    buttons[ BT_PL_OFF_N ] = pixmap->copy ( 23,61,23,12 );
    buttons[ BT_PL_OFF_P ] = pixmap->copy ( 69,61,23,12 );

    buttons[REPEAT_ON_N] = pixmap->copy ( 0,30, 28, 15 );
    buttons[REPEAT_ON_P] = pixmap->copy ( 0,45, 28, 15 );

    buttons[REPEAT_OFF_N] = pixmap->copy ( 0, 0,28,15 );
    buttons[REPEAT_OFF_P] = pixmap->copy ( 0,15,28,15 );

    buttons[SHUFFLE_ON_N] = pixmap->copy ( 28,30,46,15 );
    buttons[SHUFFLE_ON_P] = pixmap->copy ( 28,45,46,15 );

    buttons[SHUFFLE_OFF_N] = pixmap->copy ( 28, 0,46,15 );
    buttons[SHUFFLE_OFF_P] = pixmap->copy ( 28,15,46,15 );

    delete pixmap;

}

void Skin::loadLetters ( void )
{
    QPixmap *img = getPixmap("text");

    if (!img)
        img = getDummyPixmap("text");

    QList<QList<QPixmap> > ( letters );
    for ( int i = 0; i < 3; i++ )
    {
        QList<QPixmap> ( l );
        for ( int j = 0; j < 31; j++ )
        {
            l.append ( img->copy ( j*5, i*6, 5, 6 ) );
        }
        letters.append ( l );
    }

    delete img;


    /* alphabet */
    for ( uint i = 97; i < 123; i++ )
    {
        m_letters.insert(i, letters[0][i-97]);
    }

    /* digits */
    for ( uint i = 0; i <= 9; i++ )
    {
        m_letters.insert ( i+48, letters[1][i] );
    }

    /* special characters */
    m_letters.insert('"',  letters[0][27]);
    m_letters.insert('@',  letters[0][28]);
    m_letters.insert(':',  letters[1][12]);
    m_letters.insert('(',  letters[1][13]);
    m_letters.insert(')',  letters[1][14]);
    m_letters.insert('-',  letters[1][15]);
    m_letters.insert('\'', letters[1][16]);
    m_letters.insert('`',  letters[1][16]);
    m_letters.insert('!',  letters[1][17]);
    m_letters.insert('_',  letters[1][18]);
    m_letters.insert('+',  letters[1][19]);
    m_letters.insert('\\', letters[1][20]);
    m_letters.insert('/',  letters[1][21]);
    m_letters.insert('[',  letters[1][22]);
    m_letters.insert(']',  letters[1][23]);
    m_letters.insert('^',  letters[1][24]);
    m_letters.insert('&',  letters[1][25]);
    m_letters.insert('%',  letters[1][26]);
    m_letters.insert('.',  letters[1][27]);
    m_letters.insert(',',  letters[1][27]);
    m_letters.insert('=',  letters[1][28]);
    m_letters.insert('$',  letters[1][29]);
    m_letters.insert('#',  letters[1][30]);

    m_letters.insert(229, letters[2][0]);
    m_letters.insert(246, letters[2][1]);
    m_letters.insert(228, letters[2][2]);
    m_letters.insert('?', letters[2][3]);
    m_letters.insert('*', letters[2][4]);
    m_letters.insert(' ', letters[2][5]);

    /* text background */
    //m_items->insert (TEXTBG, letters[2][6]);
}

void Skin::loadMonoSter()
{
    QPixmap *pixmap = getPixmap("monoster");

    if (!pixmap)
        pixmap = getDummyPixmap("monoster");

    m_ms_parts.clear();
    m_ms_parts[ MONO_A ] = pixmap->copy ( 29,0,27,12 );
    m_ms_parts[ MONO_I ] = pixmap->copy ( 29,12,27,12 );
    m_ms_parts[ STEREO_A ] = pixmap->copy ( 0,0,27,12 );
    m_ms_parts[ STEREO_I ] = pixmap->copy ( 0,12,27,12 );

    delete pixmap;

    m_parts.clear();
    QPainter paint;
    pixmap = getPixmap("playpaus");

    if (!pixmap)
        pixmap = getDummyPixmap("playpaus");

    QPixmap part(11, 9);
    paint.begin(&part);
    paint.drawPixmap (0, 0, 3, 9, *pixmap, 36, 0, 3, 9);
    paint.drawPixmap (3, 0, 8, 9, *pixmap,  1, 0, 8, 9);
    paint.end();
    m_parts [PLAY] = part.copy();

    part = QPixmap(11, 9);
    paint.begin(&part);
    paint.drawPixmap (0, 0, 2, 9, *pixmap, 27, 0, 2, 9);
    paint.drawPixmap (2, 0, 9, 9, *pixmap,  9, 0, 9, 9);
    paint.end();
    m_parts [PAUSE] = part.copy();

    part = QPixmap(11, 9);
    paint.begin(&part);
    paint.drawPixmap (0, 0, 2, 9, *pixmap, 27, 0, 2, 9);
    paint.drawPixmap (2, 0, 9, 9, *pixmap, 18, 0, 9, 9);
    paint.end();
    m_parts [STOP]  = part.copy();

    delete pixmap;
}

void Skin::loadVolume()
{
    QPixmap *pixmap = getPixmap("volume");

    if (!pixmap)
        pixmap = getDummyPixmap("volume");

    m_volume.clear();
    for (int i = 0; i < 28; ++i)
        m_volume.append(pixmap->copy ( 0,i*15,66,13 ));
    if (pixmap->height() > 425)
    {
        buttons [BT_VOL_N] = pixmap->copy (15,422,14, pixmap->height() - 422);
        buttons [BT_VOL_P] = pixmap->copy (0, 422,14, pixmap->height() - 422);
    }
    else
    {
        buttons [BT_VOL_N] = QPixmap();
        buttons [BT_VOL_P] = QPixmap();
    }
    delete pixmap;
}

void Skin::loadBalance()
{
    QPixmap *pixmap = getPixmap ( "balance" );
    if (!pixmap)
        pixmap = getPixmap ( "volume" );

    if (!pixmap)
        pixmap = getDummyPixmap("balance");

    m_balance.clear();
    for (int i = 0; i < 28; ++i)
        m_balance.append(pixmap->copy ( 9,i*15,38,13 ));
    if (pixmap->height() > 427)
    {
        buttons [BT_BAL_N] = pixmap->copy (15, 422,14,pixmap->height()-422);
        buttons [BT_BAL_P] = pixmap->copy (0,422,14,pixmap->height()-422);
    }
    else
    {
        buttons [BT_BAL_N] = QPixmap();
        buttons [BT_BAL_P] = QPixmap();
    }
    delete pixmap;
}

void Skin::loadRegion()
{
    m_mwRegion = QRegion();
    m_plRegion = QRegion();
    m_skin_dir.setFilter ( QDir::Files | QDir::Hidden | QDir::NoSymLinks );
    QString path;
    QFileInfoList list = m_skin_dir.entryInfoList();
    for ( int i = 0; i < list.size(); ++i )
    {
        QFileInfo fileInfo = list.at ( i );
        if ( fileInfo.fileName().toLower() == "region.txt" )
        {
            path = fileInfo.filePath ();
            break;
        }
    }

    if ( path.isNull () )
    {
        qDebug ( "Skin: cannot find region.txt. Transparenty disabled" );
        return;
    }
    m_mwRegion = createRegion(path, "Normal");
    m_plRegion = createRegion(path, "Equalizer");
}

QRegion Skin::createRegion(const QString &path, const QString &key)
{
    QRegion region;
    QSettings settings(path, QSettings::IniFormat);
    QStringList numPoints = settings.value(key+"/NumPoints").toStringList();
    QStringList value = settings.value(key+"/PointList").toStringList();
    QStringList numbers;
    foreach(QString str, value)
    numbers << str.split(" ", QString::SkipEmptyParts);

    QList <QRegion> regions;

    QList<QString>::iterator n;
    n = numbers.begin();
    for (int i = 0; i < numPoints.size(); ++i)
    {
        QList <int> lp;
        for (int j = 0; j < numPoints.at(i).toInt()*2; j++)
        {
            lp << n->toInt();
            n ++;
        }
        QVector<QPoint> points;

        for (int l = 0; l < lp.size(); l+=2)
        {
            points << QPoint(lp.at(l), lp.at(l+1));
        }
        region = region.united(QRegion(QPolygon(points)));
    }
    return region;
}

QPixmap * Skin::getDummyPixmap(const QString& name)
{
    QDir dir (":/default");
    dir.setFilter ( QDir::Files | QDir::Hidden | QDir::NoSymLinks );
    QFileInfoList f = dir.entryInfoList();
    for ( int j = 0; j < f.size(); ++j )
    {
        QFileInfo fileInfo = f.at ( j );
        QString fn = fileInfo.fileName().toLower();
        if ( fn.section ( ".",0,0 ) == name )
        {
            return new QPixmap ( fileInfo.filePath() );
        }
    }
    qFatal("Skin:: default skin corrupted");
    return 0;
}

