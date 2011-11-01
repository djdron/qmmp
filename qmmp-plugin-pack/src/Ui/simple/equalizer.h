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

#ifndef EQUALIZER_H
#define EQUALIZER_H

#include <QDialog>
#include <QList>

class QVBoxLayout;
class QCheckBox;
class QSlider;

class Equalizer : public QDialog
{
    Q_OBJECT
public:
    explicit Equalizer(QWidget *parent = 0);


private slots:
    void writeSettings();
    void resetSettings();

private:
    void readSettigs();
    QVBoxLayout *m_layout;
    QCheckBox *m_enabled;
    QList <QSlider *> m_sliders;

};

#endif // EQUALIZER_H
