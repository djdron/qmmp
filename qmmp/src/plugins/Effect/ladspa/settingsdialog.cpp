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

#include <QSettings>
#include <QApplication>
#include <QStyle>
#include <QStandardItemModel>
#include <QFormLayout>
#include <QWidget>
#include <QLabel>
#include <qmmp/qmmp.h>
#include "ladspaslider.h"
#include "ladspabutton.h"
#include "ladspahost.h"
#include "settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent)
        : QDialog(parent)
{
    ui.setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose, true);
    ui.loadButton->setIcon(QApplication::style()->standardIcon(QStyle::SP_ArrowRight));
    ui.unloadButton->setIcon(QApplication::style()->standardIcon(QStyle::SP_ArrowLeft));

    m_model = new QStandardItemModel(0, 2, this);
    m_model->setHeaderData(0, Qt::Horizontal, tr("UID"));
    m_model->setHeaderData(1, Qt::Horizontal, tr("Name"));
    ui.pluginsTreeView->setModel(m_model);

    if(!LADSPAHost::instance())
        new LADSPAHost(qApp);

    QList <LADSPAPlugin *> plugin_list = LADSPAHost::instance()->plugins();

    for(int i = 0; i < plugin_list.size(); ++i)
    {
        m_model->insertRow(i);
        m_model->setData(m_model->index(i, 0), (uint) plugin_list[i]->unique_id);
        m_model->setData(m_model->index(i, 1), plugin_list[i]->name);
    }
    ui.pluginsTreeView->resizeColumnToContents (0);
    ui.pluginsTreeView->resizeColumnToContents (1);
    updateRunningPlugins();
}

SettingsDialog::~SettingsDialog()
{}

void SettingsDialog::on_loadButton_clicked()
{
    LADSPAHost *l = LADSPAHost::instance();
    QModelIndex index = ui.pluginsTreeView->currentIndex ();
    if(index.isValid())
    {
        l->addPlugin(l->plugins().at(index.row()));
        updateRunningPlugins();
    }
}

void SettingsDialog::on_unloadButton_clicked()
{
    LADSPAHost *l = LADSPAHost::instance();
    QModelIndex index = ui.runningListWidget->currentIndex ();
    if(index.isValid())
    {
        l->unload(l->runningPlugins().at(index.row()));
        updateRunningPlugins();
    }
}

void SettingsDialog::on_configureButton_clicked()
{
    LADSPAHost *l = LADSPAHost::instance();
    QModelIndex index = ui.runningListWidget->currentIndex ();
    if(!index.isValid())
        return;

    LADSPAEffect *effect = l->runningPlugins().at(index.row());
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle(effect->descriptor->Name);
    QFormLayout *formLayout = new QFormLayout(dialog);
    LADSPAButton *button = 0;
    LADSPASlider *slider = 0;

    foreach(LADSPAControl *c, effect->controls)
    {
        switch ((int) c->type)
        {
        case LADSPAControl::BUTTON:
            button = new LADSPAButton(c->value, dialog);
            button->setText(c->name);
            formLayout->addRow(button);
            break;
        case LADSPAControl::SLIDER:
            slider = new LADSPASlider(c->min, c->max, c->step, c->value, dialog);
            formLayout->addRow(c->name, slider);
        }
    }
    if (effect->controls.isEmpty())
    {
        QLabel *label = new QLabel(tr("This LADSPA plugin has no user controls"), dialog);
        formLayout->addRow(label);
    }
    dialog->setLayout(formLayout);
    dialog->setFixedSize(dialog->sizeHint());
    dialog->exec();
    dialog->deleteLater();
}

void SettingsDialog::accept()
{
    QDialog::accept();
}

void SettingsDialog::updateRunningPlugins()
{
    ui.runningListWidget->clear();
    QList <LADSPAEffect *> plugin_list = LADSPAHost::instance()->runningPlugins();

    for(int i = 0; i < plugin_list.size(); ++i)
        ui.runningListWidget->addItem(plugin_list[i]->descriptor->Name);
}
