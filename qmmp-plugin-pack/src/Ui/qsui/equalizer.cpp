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

#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSlider>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>
#include <QPainter>
#include <QDialogButtonBox>
#include <qmmp/qmmpsettings.h>
#include "equalizer.h"

Equalizer::Equalizer(QWidget *parent) : QDialog(parent)
{
    setWindowTitle(tr("Equalizer"));

    m_layout = new QVBoxLayout(this);
    setLayout(m_layout);
    m_layout->setSpacing(5);
    m_layout->setMargin(5);
    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    buttonsLayout->setSpacing(5);
    m_enabled = new QCheckBox(tr("Enable equalizer"), this);
    buttonsLayout->addWidget(m_enabled);
    QPushButton *reset = new QPushButton(tr("Reset"), this);
    connect(reset, SIGNAL(clicked()), SLOT(resetSettings()));
    buttonsLayout->addWidget(reset);
    QDialogButtonBox *dialogButtons = new QDialogButtonBox(QDialogButtonBox::Close, Qt::Horizontal, this);
    dialogButtons->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    connect(dialogButtons, SIGNAL(rejected()), SLOT(reject()));
    buttonsLayout->addWidget(dialogButtons);

    QGridLayout *slidersLayout = new QGridLayout;

    QStringList names;
    names << tr("Preamp") << "25" << "40" << "63" << "100" << "160" << "250"
           << "400" << "630" << "1k" << "1,6k" << "2,5k"
           << "4k" << "6,3k" << "10k" << "16k";

    for(int i = 0; i < 16; ++i)
    {
        QSlider *slider = new QSlider(this);
        slider->setRange(-20, 20);
        QLabel *label = new QLabel(this);
        label->setFrameShape(QFrame::Box);
        label->setText(names.at(i));
        label->setMinimumWidth(30);
        slidersLayout->addWidget(slider, 1, i, Qt::AlignHCenter);
        slidersLayout->addWidget(label, 2, i, Qt::AlignHCenter);
        QLabel *label2 = new QLabel(this);
        label2->setNum(0);
        slidersLayout->addWidget(label2, 0, i, Qt::AlignHCenter);
        connect(slider, SIGNAL(valueChanged(int)), label2, SLOT(setNum(int)));
        m_sliders << slider;

    }
    m_layout->addItem(slidersLayout);
    m_layout->addItem(buttonsLayout);
    resize(600, 300);
    readSettigs();
    foreach(QSlider *slider, m_sliders)
        connect(slider, SIGNAL(valueChanged(int)), SLOT(writeSettings()));
    connect(m_enabled, SIGNAL(toggled(bool)), SLOT(writeSettings()));
}

void Equalizer::readSettigs()
{
    EqSettings settings = QmmpSettings::instance()->eqSettings();
    m_enabled->setChecked(settings.isEnabled());
    m_sliders.at(0)->setValue(settings.preamp());
    for(int i = 0; i < EqSettings::EQ_BANDS_15; ++i)
    {
        m_sliders.at(i+1)->setValue(settings.gain(i));
    }
}

void Equalizer::writeSettings()
{
    EqSettings settings(EqSettings::EQ_BANDS_15);
    settings.setPreamp(m_sliders.at(0)->value());
    settings.setEnabled(m_enabled->isChecked());
    for(int i = 0; i < EqSettings::EQ_BANDS_15; ++i)
    {
        settings.setGain(i, m_sliders.at(i+1)->value());
    }
    QmmpSettings::instance()->setEqSettings(settings);
}

void Equalizer::resetSettings()
{
    foreach(QSlider *slider, m_sliders)
    {
        slider->blockSignals(true);
        slider->setValue(0);
        slider->blockSignals(false);
    }
    writeSettings();
}
