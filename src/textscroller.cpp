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
#include <QPainter>
#include <QTimer>
#include <QSettings>

#include "skin.h"
#include "textscroller.h"
#include "version.h"

TextScroller *TextScroller::pointer = 0;

TextScroller *TextScroller::getPointer()
{
    return pointer;
}


TextScroller::TextScroller ( QWidget *parent )
        : PixmapWidget ( parent )
{
    pointer = this;
    m_skin = Skin::getPointer();
    m_pixmap = QPixmap ( 154,15 );
    x = 0;
    m_text = "Qt-based Multimedia Player (Qmmp " + QString(QMMP_STR_VERSION) + ")";
    m_update = FALSE;
    readSettings();
    m_timer = new QTimer ( this );
    connect ( m_timer, SIGNAL ( timeout() ),SLOT ( addOffset() ) );
    m_timer -> start ( 50 );
    updateSkin();
    connect(m_skin, SIGNAL(skinChanged()), this, SLOT(updateSkin()));
}


TextScroller::~TextScroller()
{}

void TextScroller::addOffset()
{
    m_pixmap.fill ( Qt::transparent );
    QPainter paint ( &m_pixmap );
    x--;
    paint.setPen(m_color);
    paint.setFont(m_font);
    paint.drawText ( 154+x,12, m_text );
    paint.drawText ( 154+x+m_metrics->width ( m_text ) + 15,12, m_text );
    if ( 154 + x < - m_metrics->width ( m_text ) - 15 +1)
    {
        x=-154;
    }
    setPixmap ( m_pixmap );
}

void TextScroller::setText(const QString& text)
{
    if (m_text != text)
    {
        m_text = text;
        x = -50;
    }
}

void TextScroller::updateSkin()
{
    m_color.setNamedColor(m_skin->getPLValue("normal"));
}

void TextScroller::readSettings()
{
    QSettings settings(QDir::homePath()+"/.qmmp/qmmprc", QSettings::IniFormat);
    QString fontname = settings.value("MainWindow/Font","").toString();
    if (fontname.isEmpty ())
        fontname = QFont("Helvetica [Cronyx]", 9).toString();
    m_font.fromString(fontname);

    if (m_update)
    {
        delete m_metrics;
        m_metrics = new QFontMetrics(m_font);
    }
    else
    {
        m_update = TRUE;
        m_metrics = new QFontMetrics(m_font);
    }
}
