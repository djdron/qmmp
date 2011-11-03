/***************************************************************************
 *   Copyright (C) 2011 by Ilya Kotov                                      *
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
#include <QFile>
#include <QTimer>
#include "logo.h"

Logo::Logo(QWidget *parent) : Visual(parent)
{
    QPixmap pixmap(":/simple/terminus.png");
    m_letters.insert('0', pixmap.copy(0, 0, 8, 14));
    m_letters.insert('1', pixmap.copy(8, 0, 8, 14));
    m_letters.insert('2', pixmap.copy(16, 0, 8, 14));
    m_letters.insert('3', pixmap.copy(24, 0, 8, 14));
    m_letters.insert('4', pixmap.copy(32, 0, 8, 14));
    m_letters.insert('5', pixmap.copy(40, 0, 8, 14));
    m_letters.insert('6', pixmap.copy(48, 0, 8, 14));
    m_letters.insert('7', pixmap.copy(56, 0, 8, 14));
    m_letters.insert('8', pixmap.copy(64, 0, 8, 14));
    m_letters.insert('9', pixmap.copy(72, 0, 8, 14));
    m_letters.insert('A', pixmap.copy(80, 0, 8, 14));
    m_letters.insert('B', pixmap.copy(88, 0, 8, 14));
    m_letters.insert('C', pixmap.copy(96, 0, 8, 14));
    m_letters.insert('D', pixmap.copy(104, 0, 8, 14));
    m_letters.insert('E', pixmap.copy(112, 0, 8, 14));
    m_letters.insert('F', pixmap.copy(120, 0, 8, 14));
    m_letters.insert('/', pixmap.copy(128, 0, 8, 14));
    m_letters.insert('|', pixmap.copy(136, 0, 8, 14));
    m_letters.insert('\\', pixmap.copy(144, 0, 8, 14));
    m_letters.insert('_', pixmap.copy(152, 0, 8, 14));
    m_letters.insert('-', pixmap.copy(160, 0, 8, 14));
    m_letters.insert('X', pixmap.copy(168, 0, 8, 14));
    m_letters.insert(' ', pixmap.copy(176, 0, 8, 14));

    QFile file(":/ascii_logo.txt");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    while(!file.atEnd())
    {
        m_source_lines.append(file.readLine());
    }
    max_steps = 50;
    QTimer *m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), SLOT(updateLetters()));
    m_timer->setInterval(50);
    m_timer->start();
    updateLetters();
    Visual::add(this);
    m_buf_size = 0;
}

Logo::~Logo()
{
    Visual::remove(this);
}

void Logo::add(unsigned char *data, qint64 size, int chan)
{
    Q_UNUSED(chan);
    m_buf_size = qMin(size, (qint64)2048);
    memcpy(m_buf, data, m_buf_size);
}

void Logo::clear()
{
    m_buf_size = 0;
    update();
}

void Logo::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.fillRect(rect(), "black");


    for(int row = 0; row < m_lines.count(); ++row)
    {
        QString text = m_lines.at(row);
        for(int i = 0; i < text.size(); ++i)
        {
            painter.drawPixmap(50 + i*8,row*14, m_letters.value(text[i]));
        }
    }
}

void Logo::updateLetters()
{
    m_lines.clear();
    mutex()->lock();
    int at = 0, value = 0;
    foreach(QString line, m_source_lines)
    {
        while(line.contains("X"))
        {
            value = 0;
            if(at < m_buf_size)
                value = m_buf[at] / 16;
            line.replace(line.indexOf("X"), 1, QString("%1").arg(value, 0, 16).toUpper());
            at++;
        }
        m_lines.append(line);
    }
    mutex()->unlock();
    update();
}
