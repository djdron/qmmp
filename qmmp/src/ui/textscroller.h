/***************************************************************************
 *   Copyright (C) 2006-2009 by Ilya Kotov                                 *
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
#ifndef TEXTSCROLLER_H
#define TEXTSCROLLER_H

#include <QWidget>

/**
	@author Ilya Kotov <forkotov02@hotmail.ru>
*/

class QTimer;
class QMenu;

class Skin;

class TextScroller : public QWidget
{
Q_OBJECT
public:
    TextScroller(QWidget *parent = 0);

    ~TextScroller();

    static TextScroller *getPointer();
    void setText(const QString&);
    void readSettings();


public slots:
    void setProgress(int);

private slots:
    void addOffset();
    void updateSkin();
    void setAutoscroll(bool);

protected:
    void hideEvent (QHideEvent *);
    void showEvent (QShowEvent *);
    void paintEvent (QPaintEvent *);
    void mousePressEvent (QMouseEvent *);
    void mouseReleaseEvent (QMouseEvent *);
    void mouseMoveEvent (QMouseEvent *);

private:
    bool m_pressing;
    int press_pos;
    bool m_update;
    static TextScroller *pointer;
    QPixmap m_pixmap;
    int x;
    QFont m_font;
    QFontMetrics *m_metrics;
    QString m_text;
    QString m_scrollText;
    Skin *m_skin;
    QColor m_color;
    QTimer *m_timer;
    int m_progress;
    bool m_autoscroll;
    bool m_bitmapConf;
    bool m_bitmap;
    QMenu *m_menu;
};

#endif
