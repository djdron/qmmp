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
#include <QPainter>
#include <QResizeEvent>
#include <QMenu>
#include <QSettings>

#include "dock.h"
#include "button.h"
#include "playlistmodel.h"
#include "playlisttitlebar.h"
#include "skin.h"

PlayListTitleBar::PlayListTitleBar(QWidget *parent)
        : PixmapWidget(parent)
{
    m_active = FALSE;
    m_resize = FALSE;
    m_shade2 = 0;
    m_model = 0;
    m_shaded = FALSE;
    m_align = FALSE;
    m_skin = Skin::getPointer();
    setSizeIncrement(25,1);
    connect(m_skin, SIGNAL(skinChanged()), this, SLOT(updateSkin()));
    m_pl = qobject_cast<PlayList*>(parent);
    m_mw = qobject_cast<MainWindow*>(m_pl->parent());

    m_close = new Button(this,Skin::PL_BT_CLOSE_N, Skin::PL_BT_CLOSE_P);
    connect (m_close, SIGNAL(clicked()), m_pl, SIGNAL(closed()));
    m_close->move(264,3);
    m_shade = new Button(this, Skin::PL_BT_SHADE1_N, Skin::PL_BT_SHADE1_P);
    connect(m_shade, SIGNAL(clicked()), SLOT(shade()));
    m_shade->move(255,3);
    resize(275,20);
    setMinimumWidth(275);
    readSettings();
    QSettings settings (QDir::homePath() +"/.qmmp/qmmprc", QSettings::IniFormat);
    m_pl->resize (settings.value ("PlayList/size", QSize (275, 116)).toSize());
    if (settings.value ("PlayList/shaded", FALSE).toBool())
        shade();
    resize(m_pl->width(),height());
    m_align = TRUE;
}


PlayListTitleBar::~PlayListTitleBar()
{
    QSettings settings (QDir::homePath() +"/.qmmp/qmmprc", QSettings::IniFormat);
    settings.setValue ("PlayList/size", QSize (m_pl->width(), m_shaded ? m_height:m_pl->height()));
    settings.setValue ("PlayList/shaded", m_shaded);
}

void PlayListTitleBar::drawPixmap(int sx)
{
    m_close->move(264+sx*25,3);
    m_shade->move(255+sx*25,3);
    if (m_shade2)
        m_shade2->move(255+sx*25,3);
    QPixmap pixmap(275+sx*25,20);
    pixmap.fill("black");
    QPainter paint;
    paint.begin(&pixmap);
    if (m_shaded)
    {
        paint.drawPixmap(0,0,m_skin->getPlPart(Skin::PL_TITLEBAR_SHADED2));
        for (int i = 1; i<sx+9; i++)
        {
            paint.drawPixmap(25*i,0,m_skin->getPlPart(Skin::PL_TFILL_SHADED));
        }
    }

    if (m_active)
    {
        if (m_shaded)
        {
            m_shade2->show();
            paint.drawPixmap(225+sx*25,0,m_skin->getPlPart(Skin::PL_TITLEBAR_SHADED1_A));
        }
        else
        {
            paint.drawPixmap(0,0,m_skin->getPlPart(Skin::PL_CORNER_UL_A));
            for (int i = 1; i<sx+10; i++)
            {
                paint.drawPixmap(25*i,0,m_skin->getPlPart(Skin::PL_TFILL1_A));
            }
            paint.drawPixmap(100-12+12*sx,0,m_skin->getPlPart(Skin::PL_TITLEBAR_A));
            paint.drawPixmap(250+sx*25,0,m_skin->getPlPart(Skin::PL_CORNER_UR_A));
            m_close->show();
            m_shade->show();
        }
    }
    else
    {
        if (m_shaded)
        {
            m_shade2->hide();
            paint.drawPixmap(275-50+sx*25,0,m_skin->getPlPart(Skin::PL_TITLEBAR_SHADED1_I));
        }
        else
        {
            paint.drawPixmap(0,0,m_skin->getPlPart(Skin::PL_CORNER_UL_I));
            for (int i = 1; i<sx+10; i++)
            {
                paint.drawPixmap(25*i,0,m_skin->getPlPart(Skin::PL_TFILL1_I));
            }
            paint.drawPixmap(100-12+12*sx,0,m_skin->getPlPart(Skin::PL_TITLEBAR_I));
            paint.drawPixmap(250+sx*25,0,m_skin->getPlPart(Skin::PL_CORNER_UR_I));
            m_close->hide();
            m_shade->hide();
        }
    }
    if (m_shaded)
    {
        QColor col;
        col.setNamedColor(QString(m_skin->getPLValue("mbbg")));
        paint.setBrush(QBrush(col));
        paint.setPen(col);
        paint.drawRect(8,1, 235 + sx*25, 11);
        //draw text
        paint.setFont(m_font);
        paint.setPen(QString(m_skin->getPLValue("mbfg")));
        paint.drawText(9, 11, m_truncatedText);
    }
    paint.end();
    setPixmap(pixmap);
}

void PlayListTitleBar::resizeEvent(QResizeEvent *e)
{
    truncate();
    drawPixmap((e->size().width()-275)/25);
}

void PlayListTitleBar::mousePressEvent(QMouseEvent* event)
{
    switch ((int) event->button ())
    {
    case Qt::LeftButton:
    {
        pos = event->pos();

        if (m_shaded && (width() - 30) < pos.x() && pos.x() < (width() - 22))
        {
            m_resize = TRUE;
            m_pl->setCursor (Qt::SizeHorCursor);
        }


        break;
    }
    case Qt::RightButton:
    {
        m_mw->menu()->exec(event->globalPos());
    }
    }
}

void PlayListTitleBar::mouseReleaseEvent(QMouseEvent*)
{
    Dock::getPointer()->updateDock();
    m_resize = FALSE;
    m_pl->setCursor (Qt::ArrowCursor);
}

void PlayListTitleBar::mouseMoveEvent(QMouseEvent* event)
{
    QPoint npos = (event->globalPos()-pos);
    QPoint oldpos = npos;
    if (m_shaded && m_resize)
    {
        m_pl->resize((event->x() + 25), m_pl->height());
        resize((event->x() + 25), height());
    }
    else
        Dock::getPointer()->move(m_pl, npos);
}

void PlayListTitleBar::setActive(bool a)
{
    m_active = a;
    drawPixmap((width()-275)/25);
}


void PlayListTitleBar::setModel(PlayListModel *model)
{
    m_model = model;
    connect (m_model, SIGNAL(listChanged()), SLOT(showCurrent()));
}

void PlayListTitleBar::readSettings()
{
    QSettings settings (QDir::homePath() +"/.qmmp/qmmprc", QSettings::IniFormat);
    QString fontname = settings.value("PlayList/Font","").toString();
    if (fontname.isEmpty ())
        fontname = QFont("Helvetica [Cronyx]", 8).toString();
    m_font.fromString(fontname);
    m_font.setPointSize(8);
}

void PlayListTitleBar::updateSkin()
{
    drawPixmap((width()-275)/25);
}

void PlayListTitleBar::shade()
{
    m_shaded = !m_shaded;

    if (m_shaded)
    {
        m_height = m_pl->height();
        m_pl->setFixedHeight(14);
        m_shade->hide();
        m_shade2 = new Button(this, Skin::PL_BT_SHADE2_N, Skin::PL_BT_SHADE2_P);
        m_shade2->move(254,3);
        connect(m_shade2, SIGNAL(clicked()), SLOT(shade()));
        m_shade2->show();
    }
    else
    {
        m_pl->setMinimumSize (275,116);
        m_pl->setMaximumSize (10000,10000);
        m_pl->resize(width(),m_height);
        m_shade2->deleteLater();
        m_shade2 = 0;
        m_shade->show();
    }
    showCurrent();
    update();
    if (m_align)
        Dock::getPointer()->align(m_pl, m_shaded? -m_height+14: m_height-14);
}

void PlayListTitleBar::showCurrent()
{
    if (m_model)
    {
        PlayListItem* info = m_model->currentItem();
        if (info)
        {
            m_text = QString("%1. ").arg(m_model->currentRow()+1);
            m_text.append(info->title());
            m_text.append(QString("  (%1:%2)").arg(info->length()/60)
                          .arg(info->length()%60, 2, 10, QChar('0')));
        }
        else
            m_text.clear();
    }
    truncate();
    drawPixmap((width()-275)/25);
}

void PlayListTitleBar::truncate()
{
    m_truncatedText = m_text;
    QFontMetrics metrics(m_font);
    bool truncate = FALSE;
    while (metrics.width(m_truncatedText) > (this->width() - 35))
    {
        truncate = TRUE;
        m_truncatedText = m_truncatedText.left(m_truncatedText.length()-1);
    }
    if (truncate)
        m_truncatedText = m_truncatedText.left(m_truncatedText.length()-3).trimmed()+"...";
}
