/***************************************************************************
 *   Copyright (C) 2009 by Ilya Kotov                                      *
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


#include <QDoubleSpinBox>
#include <QSlider>
#include <QHBoxLayout>
#include "ladspaplugin.h"
#include "ladspaslider.h"

LADSPASlider::LADSPASlider(double min, double max, double step, LADSPA_Data *value,
                           LADSPAHost *host, QWidget *parent) : QWidget(parent)
{
    m_min = min;
    m_max = max;
    m_step = step;
    m_host = host;
    m_value = value;
    m_slider = new QSlider(Qt::Horizontal, this);
    m_spinBox = new QDoubleSpinBox(this);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(m_slider);
    layout->addWidget(m_spinBox);
    layout->setContentsMargins (0, 0, 0, 0);
    setLayout(layout);

    m_spinBox->setRange(min, max);
    m_spinBox->setSingleStep(step);
    m_spinBox->setValue(*value);

    m_slider->setRange(0, (max-min)/step);
    m_slider->setSingleStep(1);
    m_slider->setPageStep(10);
    m_slider->setValue((*value-min)/step);

    connect(m_spinBox, SIGNAL(valueChanged (double)), SLOT(setValue(double)));
    connect(m_slider, SIGNAL(sliderMoved (int)),SLOT(setValue(int)));
}

void LADSPASlider::setValue(double v)
{
    *m_value = v;
    m_slider->setValue((v-m_min)/m_step);
}

void LADSPASlider::setValue(int v)
{
    m_spinBox->setValue(v*m_step + m_min);
}
