/***************************************************************************
 *   Copyright (C) 2006-2008 by Ilya Kotov                                 *
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
#include <QSettings>
#include <QEvent>
#include <QMenu>
#include <QInputDialog>
#include <QCloseEvent>
#include <qmmpui/filedialog.h>
#include <qmmp/soundcore.h>

#include "skin.h"
#include "eqslider.h"
#include "eqtitlebar.h"
#include "togglebutton.h"
#include "eqgraph.h"
#include "button.h"
#include "eqpreset.h"
#include "preseteditor.h"
#include "mainwindow.h"
#include "playlist.h"
#include "eqwidget.h"



EqWidget::EqWidget (QWidget *parent)
        : PixmapWidget (parent)
{
    m_skin = Skin::getPointer();
    setWindowFlags (Qt::Dialog | Qt::FramelessWindowHint);
    setPixmap (m_skin->getEqPart (Skin::EQ_MAIN));
    //setPixmap(QPixmap(275,116));
    m_titleBar = new EqTitleBar (this);
    m_titleBar -> move (0,0);
    m_titleBar -> show();
    connect (m_skin, SIGNAL (skinChanged()), this, SLOT (updateSkin()));

    m_preamp = new EqSlider (this);
    m_preamp->show();
    m_preamp->move (21,38);
    connect (m_preamp,SIGNAL (sliderMoved (int)),SLOT (setPreamp ()));

    m_on = new ToggleButton (this,Skin::EQ_BT_ON_N,Skin::EQ_BT_ON_P,
                             Skin::EQ_BT_OFF_N,Skin::EQ_BT_OFF_P);
    m_on->show();
    m_on->move (14,18);
    connect (m_on, SIGNAL (clicked(bool)), SIGNAL(valueChanged()));

    m_autoButton = new ToggleButton(this, Skin::EQ_BT_AUTO_1_N, Skin::EQ_BT_AUTO_1_P,
                                    Skin::EQ_BT_AUTO_0_N, Skin::EQ_BT_AUTO_0_P);
    m_autoButton->move(39, 18);
    m_autoButton->show();

    m_eqg = new EQGraph(this);
    m_eqg->move(87,17);

    m_presetsMenu = new QMenu(this);

    m_presetButton = new Button (this, Skin::EQ_BT_PRESETS_N, Skin::EQ_BT_PRESETS_P);
    m_presetButton->move(217,18);
    m_presetButton->show();

    connect(m_presetButton, SIGNAL(clicked()), SLOT(showPresetsMenu()));

    for (int i = 0; i<10; ++i)
    {
        m_sliders << new EqSlider (this);
        m_sliders.at (i)->move (78+i*18,38);
        m_sliders.at (i)->show();
        connect (m_sliders.at (i), SIGNAL (sliderMoved (int)),SLOT (setGain()));
    }
    readSettings();
    createActions();
    connect(SoundCore::instance(), SIGNAL(volumeChanged(int, int)), m_titleBar, SLOT(setVolume(int, int)));
}

EqWidget::~EqWidget()
{
    while (!m_presets.isEmpty())
        delete m_presets.takeFirst();
    while (!m_autoPresets.isEmpty())
        delete m_autoPresets.takeFirst();
}

int EqWidget::preamp()
{
    return m_preamp->value();
}

int EqWidget::gain (int g)
{
    return m_sliders.at (g)->value();
}

void EqWidget::changeEvent (QEvent * event)
{
    if (event->type() == QEvent::ActivationChange)
    {
        m_titleBar->setActive(isActiveWindow());
    }
}

void EqWidget::closeEvent (QCloseEvent* e)
{
    if (e->spontaneous ())
        emit closed();
    writeSettings();
}

void EqWidget::updateSkin()
{
    m_titleBar->setActive (FALSE);
    setPixmap (m_skin->getEqPart (Skin::EQ_MAIN));
}

void EqWidget::readSettings()
{
    QSettings settings (QDir::homePath() +"/.qmmp/qmmprc", QSettings::IniFormat);
    settings.beginGroup ("Equalizer");
    //geometry
    move (settings.value ("pos", QPoint (100, 216)).toPoint());
    //equalizer
    for (int i = 0; i < m_sliders.size(); ++i)
        m_sliders.at(i)->setValue(settings.value("band_"+
                                  QString("%1").arg(i), 0).toInt());
    m_preamp->setValue(settings.value("preamp", 0).toInt());
    m_on->setON(settings.value("enabled", FALSE).toBool());
    settings.endGroup();
    setGain();
    //equalizer presets
    QSettings eq_preset (QDir::homePath() +"/.qmmp/eq.preset", QSettings::IniFormat);
    for (int i = 1; TRUE; ++i)
    {
        if (eq_preset.contains("Presets/Preset"+QString("%1").arg(i)))
        {
            QString name = eq_preset.value("Presets/Preset"+QString("%1").arg(i),
                                           tr("preset")).toString();
            EQPreset *preset = new EQPreset();
            preset->setText(name);
            eq_preset.beginGroup(name);
            for (int j = 0; j < 10; ++j)
            {
                preset->setGain(j,eq_preset.value("Band"+QString("%1").arg(j),
                                                  0).toInt());
            }
            preset->setPreamp(eq_preset.value("Preamp",0).toInt());
            m_presets.append(preset);
            eq_preset.endGroup();
        }
        else
            break;
    }
    //equalizer auto-load presets
    QSettings eq_auto (QDir::homePath() +"/.qmmp/eq.auto_preset", QSettings::IniFormat);
    for (int i = 1; TRUE; ++i)
    {
        if (eq_auto.contains("Presets/Preset"+QString("%1").arg(i)))
        {
            QString name = eq_auto.value("Presets/Preset"+QString("%1").arg(i),
                                         tr("preset")).toString();
            EQPreset *preset = new EQPreset();
            preset->setText(name);
            eq_auto.beginGroup(name);
            for (int j = 0; j < 10; ++j)
            {
                preset->setGain(j,eq_auto.value("Band"+QString("%1").arg(j),
                                                0).toInt());
            }
            preset->setPreamp(eq_auto.value("Preamp",0).toInt());
            m_autoPresets.append(preset);
            eq_auto.endGroup();
        }
        else
            break;
    }
}

void EqWidget::writeSettings()
{
    QSettings settings (QDir::homePath() +"/.qmmp/qmmprc", QSettings::IniFormat);
    settings.beginGroup ("Equalizer");
    //geometry
    settings.setValue ("pos", this->pos());
    //equalizer
    for (int i = 0; i < m_sliders.size(); ++i)
        settings.setValue("band_"+QString("%1").arg(i), m_sliders.at(i)->value());
    settings.setValue("preamp", m_preamp->value());
    settings.setValue("enabled",m_on->isChecked());
    settings.endGroup();
    //equalizer presets
    QSettings eq_preset (QDir::homePath() +"/.qmmp/eq.preset", QSettings::IniFormat);
    eq_preset.clear ();
    for (int i = 0; i < m_presets.size(); ++i)
    {
        eq_preset.setValue("Presets/Preset"+QString("%1").arg(i+1),
                           m_presets.at(i)->text());
        eq_preset.beginGroup(m_presets.at(i)->text());
        for (int j = 0; j < 10; ++j)
        {
            eq_preset.setValue("Band"+QString("%1").arg(j),m_presets.at(i)->gain(j));
        }
        eq_preset.setValue("Preamp",m_presets.at(i)->preamp());
        eq_preset.endGroup();
    }
    //equalizer auto-load presets
    QSettings eq_auto (QDir::homePath() +"/.qmmp/eq.auto_preset",
                       QSettings::IniFormat);
    eq_auto.clear();
    for (int i = 0; i < m_autoPresets.size(); ++i)
    {
        eq_auto.setValue("Presets/Preset"+QString("%1").arg(i+1),
                         m_autoPresets.at(i)->text());
        eq_auto.beginGroup(m_autoPresets.at(i)->text());
        for (int j = 0; j < 10; ++j)
        {
            eq_auto.setValue("Band"+QString("%1").arg(j),m_autoPresets.at(i)->gain(j));
        }
        eq_auto.setValue("Preamp",m_autoPresets.at(i)->preamp());
        eq_auto.endGroup();
    }
}

void EqWidget::setPreamp ()
{
    emit valueChanged();
}

void EqWidget::setGain()
{
    m_eqg->clear();
    for (int i=0; i<10; ++i)
    {
        int value = m_sliders.at(i)->value();
        m_eqg->addValue(value);
    }
    emit valueChanged();
}

bool EqWidget::isEQEnabled()
{
    return m_on->isChecked();
}

void EqWidget::createActions()
{
    m_presetsMenu->addAction(tr("&Load/Delete"),this, SLOT(showEditor()));
    m_presetsMenu->addSeparator();
    m_presetsMenu->addAction(tr("&Save Preset"),this,SLOT(savePreset()));
    m_presetsMenu->addAction(tr("&Save Auto-load Preset"),this,SLOT(saveAutoPreset()));
    m_presetsMenu->addAction(tr("&Import"),this,SLOT(importWinampEQF()));
    m_presetsMenu->addSeparator();
    m_presetsMenu->addAction(tr("&Clear"),this, SLOT(reset()));
}

void EqWidget::showPresetsMenu()
{
    m_presetsMenu->exec(m_presetButton->mapToGlobal(QPoint(0, 0)));
}

void EqWidget::reset()
{
    for (int i = 0; i < m_sliders.size(); ++i)
        m_sliders.at(i)->setValue(0);
    m_preamp->setValue(0);
    setGain();
}

void EqWidget::showEditor()
{
    PresetEditor *editor = new PresetEditor(this);
    editor->addPresets(m_presets);
    editor->addAutoPresets(m_autoPresets);
    connect (editor, SIGNAL(presetLoaded(EQPreset*)), SLOT(setPreset(EQPreset*)));
    connect (editor, SIGNAL(presetDeleted(EQPreset*)), SLOT(deletePreset(EQPreset*)));
    editor->show();
}

void EqWidget::savePreset()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("Saving Preset"),
                                         tr("Preset name:"), QLineEdit::Normal,
                                         tr("preset #")+QString("%1").arg(m_presets.size()+1), &ok);
    if (ok)
    {
        EQPreset* preset = new EQPreset;
        preset->setText(text);
        preset->setPreamp(m_preamp->value());
        for (int i = 0; i<10; ++i)
            preset->setGain(i, m_sliders.at (i)->value());
        foreach(EQPreset *p, m_presets) //remove preset with same name
        {
            if (p->text() == text)
            {
                m_presets.removeAll(p);
                delete p;
            }
        }
        m_presets.append(preset);
    }
}

void EqWidget::saveAutoPreset()
{
    PlayList* playlist = qobject_cast<MainWindow*>(parent())->playlist();
    if (!playlist->currentItem())
        return;
    //delete preset if it already exists
    EQPreset* preset = findPreset(playlist->currentItem()->url().section("/",-1));
    if (preset)
        deletePreset(preset);
    //create new preset
    preset = new EQPreset();
    preset->setText(playlist->currentItem()->url().section("/",-1));
    preset->setPreamp(m_preamp->value());
    for (int i = 0; i<10; ++i)
    {
        preset->setGain(i, m_sliders.at (i)->value());
    }
    m_autoPresets.append(preset);
}

void EqWidget::setPreset(EQPreset* preset)
{
    for (int i = 0; i<10; ++i)
        m_sliders.at(i)->setValue(preset->gain(i));
    m_preamp->setValue(preset->preamp());
    setGain();
}

void EqWidget::deletePreset(EQPreset* preset)
{
    int p = m_presets.indexOf(preset);
    if (p != -1)
    {
        delete m_presets.takeAt(p);
        return;
    }
    p = m_autoPresets.indexOf(preset);
    if (p != -1)
    {
        delete m_autoPresets.takeAt(p);
        return;
    }
}

void EqWidget::loadPreset(const QString &name)
{
    if (m_autoButton->isChecked())
    {
        EQPreset *preset = findPreset(name);
        if (preset)
            setPreset(preset);
        else
            reset();
    }
}

EQPreset *EqWidget::findPreset(const QString &name)
{
    foreach(EQPreset *preset, m_autoPresets)
    {
        if (preset->text() == name)
            return preset;
    }
    return 0;
}

void EqWidget::importWinampEQF()
{
    char header[31];
    char name[257];
    char bands[11];
    QString path = FileDialog::getOpenFileName(this, tr("Import Preset"),
                   "/home",
                   QString("Winamp EQF (*.q1)"));

    QFile file(path);
    file.open(QIODevice::ReadOnly);
    file.read (header, 31);
    if (QString::fromAscii(header).contains("Winamp EQ library file v1.1"))
    {

        while (file.read (name, 257))
        {
            EQPreset* preset = new EQPreset;
            preset->setText(QString::fromAscii(name));

            file.read(bands,11);

            for (int i = 0; i<10; ++i)
            {
                preset->setGain(i, 20 - bands[i]*40/64);
            }
            preset->setPreamp(20 - bands[10]*40/64);
            m_presets.append(preset);
        }

    }
    file.close();

}
